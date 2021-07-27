// [06/01/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Utility/bcLogger.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/Component/bcNetworkComponent.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Network/Server/bcNetworkServerManager.h"
#include "Game/System/Network/Message/bcAcknowledgeNetworkMessage.h"
#include "Game/System/Network/Message/bcClientConnectNetworkMessage.h"
#include "Game/System/Network/Message/bcActorReplicateNetworkMessage.h"
#include "Game/System/Network/Message/bcActorRemoveNetworkMessage.h"
#include "Game/System/Network/Message/bcActorSyncNetworkMessage.h"

namespace black_cat
{
	namespace game
	{
		bc_network_server_manager::bc_network_server_manager(bc_game_system& p_game_system, bc_network_system& p_network_system, bci_network_server_manager_hook& p_hook, bcUINT16 p_port)
			: bc_server_socket_state_machine(*BC_NEW(platform::bc_non_block_socket, core::bc_alloc_type::unknown)
			(
				platform::bc_socket_address::inter_network,
				platform::bc_socket_type::data_gram,
				platform::bc_socket_protocol::udp
			)),
			m_game_system(&p_game_system),
			m_port(p_port),
			m_socket_is_listening(false),
			m_hook(&p_hook),
			m_actor_network_id_counter(0),
			m_messages_buffer(p_network_system)
		{
			m_socket.reset(&bc_server_socket_state_machine::get_socket());
			
			bc_server_socket_bind_event l_bind_event{ m_port };
			bc_server_socket_state_machine::process_event(l_bind_event);
		}

		bc_network_server_manager::bc_network_server_manager(bc_network_server_manager&& p_other) noexcept
			: bc_server_socket_state_machine(std::move(p_other)),
			m_game_system(p_other.m_game_system),
			m_port(p_other.m_port),
			m_socket_is_listening(p_other.m_socket_is_listening),
			m_socket(std::move(p_other.m_socket)),
			m_clients(std::move(p_other.m_clients)),
			m_hook(p_other.m_hook),
			m_actor_network_id_counter(p_other.m_actor_network_id_counter.load()),
			m_network_actors(std::move(p_other.m_network_actors)),
			m_memory_buffer(std::move(p_other.m_memory_buffer)),
			m_messages_buffer(std::move(p_other.m_messages_buffer))
		{
		}

		bc_network_server_manager::~bc_network_server_manager() = default;

		bc_network_server_manager& bc_network_server_manager::operator=(bc_network_server_manager&& p_other) noexcept
		{
			bc_server_socket_state_machine::operator=(std::move(p_other));
			m_game_system = p_other.m_game_system;
			m_port = p_other.m_port;
			m_socket_is_listening = p_other.m_socket_is_listening;
			m_socket = std::move(p_other.m_socket);
			m_clients = std::move(p_other.m_clients);
			m_hook = p_other.m_hook;
			m_actor_network_id_counter.store(p_other.m_actor_network_id_counter.load());
			m_network_actors = std::move(p_other.m_network_actors);
			m_memory_buffer = std::move(p_other.m_memory_buffer);
			m_messages_buffer = std::move(p_other.m_messages_buffer);
			
			return *this;
		}

		void bc_network_server_manager::add_actor(bc_actor& p_actor)
		{
			auto* l_network_component = p_actor.get_component<bc_network_component>();
			if (!l_network_component)
			{
				core::bc_log(core::bc_log_type::error, bcL("actor without network component cannot be added to network sync process"));
				return;
			}
			
			auto l_actor_network_id = m_actor_network_id_counter.fetch_add(1);
			
			{
				core_platform::bc_mutex_guard l_lock(m_actors_lock);
				m_network_actors.insert(std::make_pair(l_actor_network_id, p_actor));
			}
			
			l_network_component->set_network_id(l_actor_network_id);
			
			send_message(bc_actor_replicate_network_message(p_actor));
		}

		void bc_network_server_manager::remove_actor(bc_actor& p_actor)
		{
			auto* l_network_component = p_actor.get_component<bc_network_component>();
			if (l_network_component->get_network_id() == bc_actor::invalid_id)
			{
				return;
			}

			{
				core_platform::bc_mutex_guard l_lock(m_actors_lock);

				const auto l_ite = m_network_actors.find(l_network_component->get_network_id());
				if (l_ite == std::cend(m_network_actors))
				{
					core::bc_log(core::bc_log_type::error, bcL("actor network id was not found"));
					return;
				}

				m_network_actors.erase(l_ite);
			}

			l_network_component->set_network_id(bc_actor::invalid_id);
			// TODO
			send_message(bc_actor_remove_network_message());
		}

		void bc_network_server_manager::send_message(bc_network_message_ptr p_message)
		{
			{
				core::bc_mutex_test_guard l_lock(m_clients_lock);

				for(auto& l_client : m_clients)
				{
					core_platform::bc_lock_guard<bc_network_server_manager_client> l_client_lock(l_client);
					
					l_client.add_message(p_message);
				}
			}
		}
		
		void bc_network_server_manager::update(const bc_network_manager_update_context& p_context)
		{
			bc_server_socket_update_event l_update_event{ p_context.m_clock };
			bc_server_socket_state_machine::process_event(l_update_event);

			if (!m_socket_is_listening)
			{
				return;
			}

			{
				core::bc_mutex_test_guard l_lock(m_clients_lock);

				// Because during message execution new clients might be added we must not use range for loop
				for (auto l_ite = 0U; l_ite < m_clients.size(); ++l_ite)
				{
					auto& l_client = m_clients[l_ite];

					const auto l_elapsed_since_last_sync = bc_current_packet_time() - l_client.get_last_sync_time();
					if (l_elapsed_since_last_sync > l_client.get_rtt_time())
					{
						_send_to_client(l_client);
					}
				}

				_receive_from_clients();
			}
		}

		void bc_network_server_manager::on_enter(bc_server_socket_error_state& p_state)
		{
			m_socket_is_listening = false;
			m_hook->error_occurred(p_state.get_last_exception());
		}

		void bc_network_server_manager::on_enter(bc_server_socket_listening_state& p_state)
		{
			m_socket_is_listening = true;
			m_hook->started_listening(m_port);
		}

		void bc_network_server_manager::client_connected(const platform::bc_network_address& p_address)
		{
			// Client lock is acquired during update function
			m_clients.push_back(bc_network_server_manager_client(p_address));
			m_hook->client_connected();
		}

		void bc_network_server_manager::client_disconnected(const platform::bc_network_address& p_address)
		{
			const auto l_ite = std::find_if
			(
				std::begin(m_clients),
				std::end(m_clients),
				[&](const bc_network_server_manager_client& p_client)
				{
					return p_client.get_address() == p_address;
				}
			);
			if (l_ite == std::end(m_clients))
			{
				core::bc_log(core::bc_log_type::warning) << "no client were found to disconnect" << core::bc_lend;
				return;
			}

			m_clients.erase(l_ite);
			m_hook->client_disconnected();
		}

		void bc_network_server_manager::acknowledge_message(const platform::bc_network_address& p_address, bc_network_message_id p_message_id)
		{
			// Client lock is already acquired in the receive function 
			auto* l_client = _find_client(p_address);
			auto l_messages = l_client->get_messages_waiting_acknowledgment();
			const auto l_msg_ite = std::find_if
			(
				std::begin(l_messages),
				std::end(l_messages),
				[&](const bc_message_with_time& p_msg)
				{
					return p_msg.m_message->get_id() == p_message_id;
				}
			);

			if (l_msg_ite == std::end(l_messages))
			{
				core::bc_log(core::bc_log_type::warning) << "no message was found with id " << p_message_id << " to acknowledge" << core::bc_lend;
				return;
			}

			auto& l_message = *(*l_msg_ite).m_message;
			l_message.acknowledge(bc_network_message_server_context
				{
					p_address,
					*this
				});

			l_client->erase_message_with_acknowledgment(p_message_id);
		}
		
		bc_actor bc_network_server_manager::create_actor(const bcCHAR* p_entity_name)
		{
			auto l_actor = m_game_system->get_scene()->create_actor(p_entity_name, core::bc_matrix4f::identity());

			return l_actor;
		}

		bc_actor bc_network_server_manager::get_actor(bc_actor_network_id p_actor_network_id)
		{
			{
				core_platform::bc_mutex_guard l_lock(m_actors_lock);

				const auto l_ite = m_network_actors.find(p_actor_network_id);
				if (l_ite == std::end(m_network_actors))
				{
					return bc_actor();
				}

				return l_ite->second;
			}
		}

		void bc_network_server_manager::_retry_messages_with_acknowledgment(bc_network_packet_time p_current_time, bc_network_server_manager_client& p_client)
		{
			for (auto& l_msg : p_client.get_messages_waiting_acknowledgment())
			{
				const auto l_elapsed_since_last_send = p_current_time - l_msg.m_time;
				if (l_elapsed_since_last_send > p_client.get_rtt_time() * 3)
				{
					l_msg.m_time = p_current_time;
					p_client.add_message(l_msg.m_message);
				}
			}
		}

		void bc_network_server_manager::_send_to_client(bc_network_server_manager_client& p_client)
		{
			const auto l_current_time = bc_current_packet_time();
			
			{
				core_platform::bc_lock_guard<bc_network_server_manager_client> l_lock(p_client);
				
				for (auto& l_actor : m_network_actors)
				{
					p_client.add_message(bc_make_network_message(bc_actor_sync_network_message()));
				}

				_retry_messages_with_acknowledgment(l_current_time, p_client);
				
				auto l_client_messages = p_client.get_messages();
				if(l_client_messages.empty())
				{
					return;
				}
				
				const auto [l_stream, l_stream_size] = m_messages_buffer.serialize(l_current_time, l_client_messages);
				bc_server_socket_send_event l_send_event{p_client.get_address(), *l_stream, l_stream_size};
				bc_server_socket_state_machine::process_event(l_send_event);
				
				for (auto& l_message : l_client_messages)
				{
					m_hook->message_sent(*l_message);

					if (l_message->need_acknowledgment())
					{
						p_client.add_message_with_acknowledgment(bc_message_with_time 
						{
							l_current_time,
							l_message
						});
					}
				}

				p_client.clear_messages();
			}
		}

		void bc_network_server_manager::_receive_from_clients()
		{
			m_memory_buffer.set_position(core::bc_stream_seek::start, 0);
			platform::bc_network_address l_address;

			bc_server_socket_receive_event l_receive_event{ l_address, m_memory_buffer, 0 };
			bc_server_socket_state_machine::process_event(l_receive_event);
			
			const auto l_bytes_received = l_receive_event.m_bytes_received;
			if(!l_bytes_received)
			{
				return;
			}

			auto* l_client = _find_client(l_address);

			m_memory_buffer.set_position(core::bc_stream_seek::start, 0);
			const auto [l_packet_time, l_messages] = m_messages_buffer.deserialize(*this, m_memory_buffer, l_bytes_received);

			if (!l_client)
			{
				// Check to see if a new client connection is requested
				for (const auto& l_message : l_messages)
				{
					if (!core::bci_message::is<bc_client_connect_network_message>(*l_message))
					{
						continue;
					}

					l_message->execute(bc_network_message_server_context
					{
						l_address,
						*this
					});

					// after message execution client should have added
					l_client = _find_client(l_address);
					l_client->add_message(bc_make_network_message(bc_acknowledge_network_message(l_message->get_id())));

					m_hook->message_received(*l_message);
					break;
				}
								
				return;
			}

			const bc_network_message_id l_last_executed_message_id = l_client->get_last_executed_message_id();
			bc_network_message_id l_max_message_id = 0;

			{
				core_platform::bc_lock_guard<bc_network_server_manager_client> l_lock(*l_client);

				for (const auto& l_message : l_messages)
				{
					if (l_message->get_id() < l_last_executed_message_id)
					{
						// discard out of order message
						continue;
					}

					if (l_message->need_acknowledgment())
					{
						l_client->add_message(bc_make_network_message(bc_acknowledge_network_message(l_message->get_id())));
					}

					l_message->execute(bc_network_message_server_context
					{
						l_address,
						*this
					});

					m_hook->message_received(*l_message);
					l_max_message_id = std::max(l_max_message_id, l_message->get_id());
				}
			}
			
			l_client->set_last_executed_message_id(l_max_message_id);
			l_client->add_rtt_time(bc_elapsed_packet_time(l_packet_time));
			l_client->set_last_sync_time(bc_current_packet_time());
		}

		bc_network_server_manager_client* bc_network_server_manager::_find_client(const platform::bc_network_address& p_address)
		{
			const auto l_ite = std::find_if
			(
				std::begin(m_clients),
				std::end(m_clients),
				[&](auto& p_client)
				{
					return p_address == p_client.get_address();
				}
			);
			if (l_ite == std::end(m_clients))
			{
				return nullptr;
			}

			return &*l_ite;
		}
	}
}