// [06/01/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Messaging/Event/bcEventManager.h"
#include "Core/Utility/bcLogger.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/Component/bcNetworkComponent.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Network/Server/bcNetworkServerManager.h"
#include "Game/System/Network/Message/bcPingNetworkMessage.h"
#include "Game/System/Network/Message/bcAcknowledgeNetworkMessage.h"
#include "Game/System/Network/Message/bcClientConnectNetworkMessage.h"
#include "Game/System/Network/Message/bcActorReplicateNetworkMessage.h"
#include "Game/System/Network/Message/bcActorRemoveNetworkMessage.h"
#include "Game/System/Network/Message/bcActorSyncNetworkMessage.h"
#include "Game/System/Network/Message/bcSceneChangeNetworkMessage.h"
#include "Game/bcEvent.h"

namespace black_cat
{
	namespace game
	{
		bc_network_server_manager::bc_network_server_manager(core::bc_event_manager& p_event_manager,
			bc_game_system& p_game_system,
			bc_network_system& p_network_system,
			bci_network_server_manager_hook& p_hook,
			bcUINT16 p_port)
			: bc_server_socket_state_machine(*BC_NEW(platform::bc_non_block_socket, core::bc_alloc_type::unknown)
			(
				platform::bc_socket_address::inter_network,
				platform::bc_socket_type::data_gram,
				platform::bc_socket_protocol::udp
			)),
			m_event_manager(&p_event_manager),
			m_game_system(&p_game_system),
			m_port(p_port),
			m_socket_is_listening(false),
			m_hook(&p_hook),
			m_actor_network_id_counter(0),
			m_memory_buffer(core::bc_alloc_type::unknown_movable),
			m_messages_buffer(p_network_system)
		{
			m_socket.reset(&bc_server_socket_state_machine::get_socket());
			m_scene_change_event = m_event_manager->register_event_listener<bc_event_scene_change>
			(
				core::bc_event_manager::delegate_type(*this, &bc_network_server_manager::_event_handler)
			);
			
			bc_server_socket_bind_event l_bind_event{ m_port };
			bc_server_socket_state_machine::process_event(l_bind_event);
		}

		bc_network_server_manager::bc_network_server_manager(bc_network_server_manager&& p_other) noexcept
			: bc_server_socket_state_machine(std::move(p_other)),
			m_event_manager(p_other.m_event_manager),
			m_game_system(p_other.m_game_system),
			m_port(p_other.m_port),
			m_socket_is_listening(p_other.m_socket_is_listening),
			m_socket(std::move(p_other.m_socket)),
			m_hook(p_other.m_hook),
			m_clients(std::move(p_other.m_clients)),
			m_actor_network_id_counter(p_other.m_actor_network_id_counter.load()),
			m_network_actors(std::move(p_other.m_network_actors)),
			m_memory_buffer(std::move(p_other.m_memory_buffer)),
			m_messages_buffer(std::move(p_other.m_messages_buffer)),
			m_scene_change_event(std::move(p_other.m_scene_change_event))
		{
			m_scene_change_event.reassign(core::bc_event_manager::delegate_type(*this, &bc_network_server_manager::_event_handler));
		}

		bc_network_server_manager::~bc_network_server_manager() = default;

		bc_network_server_manager& bc_network_server_manager::operator=(bc_network_server_manager&& p_other) noexcept
		{
			bc_server_socket_state_machine::operator=(std::move(p_other));
			m_event_manager = p_other.m_event_manager;
			m_game_system = p_other.m_game_system;
			m_port = p_other.m_port;
			m_socket_is_listening = p_other.m_socket_is_listening;
			m_socket = std::move(p_other.m_socket);
			m_hook = p_other.m_hook;
			m_clients = std::move(p_other.m_clients);
			m_actor_network_id_counter.store(p_other.m_actor_network_id_counter.load());
			m_network_actors = std::move(p_other.m_network_actors);
			m_memory_buffer = std::move(p_other.m_memory_buffer);
			m_messages_buffer = std::move(p_other.m_messages_buffer);
			m_scene_change_event = std::move(p_other.m_scene_change_event);

			m_scene_change_event.reassign(core::bc_event_manager::delegate_type(*this, &bc_network_server_manager::_event_handler));
			
			return *this;
		}

		bc_network_type bc_network_server_manager::get_network_type() const noexcept
		{
			return bc_network_type::server;
		}

		void bc_network_server_manager::add_actor_to_sync(bc_actor& p_actor)
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

		void bc_network_server_manager::remove_actor_from_sync(bc_actor& p_actor)
		{
			auto* l_network_component = p_actor.get_component<bc_network_component>();
			const auto l_network_id = l_network_component->get_network_id();

			if (l_network_id == bc_actor::invalid_id)
			{
				return;
			}
			
			{
				core_platform::bc_mutex_guard l_lock(m_actors_lock);

				const auto l_ite = m_network_actors.find(l_network_component->get_network_id());
				if (l_ite == std::cend(m_network_actors))
				{
					core::bc_log(core::bc_log_type::error, bcL("actor was not found in network list to remove"));
					return;
				}

				m_network_actors.erase(l_ite);
			}
			
			send_message(bc_actor_remove_network_message(l_network_id));
		}

		void bc_network_server_manager::actor_removed(bc_actor& p_actor)
		{
			auto* l_network_component = p_actor.get_component<bc_network_component>();
			const auto l_network_id = l_network_component->get_network_id();

			if (l_network_id == bc_actor::invalid_id)
			{
				return;
			}

			{
				core_platform::bc_mutex_guard l_lock(m_actors_lock);

				const auto l_ite = m_network_actors.find(l_network_component->get_network_id());
				if (l_ite == std::cend(m_network_actors))
				{
					core::bc_log(core::bc_log_type::error, bcL("actor was not found in network list to remove"));
					return;
				}

				m_network_actors.erase(l_ite);
			}

			l_network_component->set_network_id(bc_actor::invalid_id);
		}

		void bc_network_server_manager::send_message(bc_network_message_ptr p_message)
		{
			{
				core_platform::bc_shared_mutex_shared_guard l_lock(m_clients_lock);

				_add_message_to_clients(std::move(p_message));
			}
		}
		
		void bc_network_server_manager::update(const bc_network_manager_update_context& p_context)
		{
			bc_server_socket_state_machine::update(p_context.m_clock);

			if (!m_socket_is_listening)
			{
				return;
			}

			// Because during message execution new clients might be added we must not use range for loop
			for (auto l_ite = 0U; l_ite < m_clients.size(); ++l_ite)
			{
				auto& l_client = m_clients[l_ite];

				if (p_context.m_send_rtt_message)
				{
					{
						core_platform::bc_lock_guard<bc_network_server_manager_client> l_client_lock(l_client);
						l_client.add_message(bc_make_network_message(bc_ping_network_message()));
					}
				}

				const auto l_elapsed_since_last_sync = bc_current_packet_time() - l_client.get_last_sync_time();
				const auto l_rtt_time = l_client.get_rtt_time() / 2;
				if (l_elapsed_since_last_sync > l_rtt_time)
				{
					_send_to_client(p_context.m_clock, l_client);
					bc_server_socket_state_machine::update(p_context.m_clock);
				}
			}

			while(_receive_from_clients())
			{
				bc_server_socket_state_machine::update(p_context.m_clock);
			}
		}

		void bc_network_server_manager::on_enter(bc_server_socket_error_state& p_state)
		{
			const auto* l_client_address = p_state.get_last_client_address();
			if(l_client_address)
			{
				bc_server_socket_state_machine::transfer_state<bc_server_socket_listening_state>();
				client_disconnected(*l_client_address);
			}
			else
			{
				m_socket_is_listening = false;
			}
			
			if (l_client_address)
			{
				core::bc_log(core::bc_log_type::error) << "error occurred in client network connection " << l_client_address << ": ";
			}
			else
			{
				core::bc_log(core::bc_log_type::error) << "error occurred in network connection: ";
			}

			core::bc_log(core::bc_log_type::error) << (p_state.get_last_exception() ? p_state.get_last_exception()->get_full_message().c_str() : "") << core::bc_lend;
			m_hook->error_occurred(p_state.get_last_client_address(), p_state.get_last_exception());
		}

		void bc_network_server_manager::on_enter(bc_server_socket_listening_state& p_state)
		{
			m_socket_is_listening = true;
			m_hook->started_listening(m_port);
		}

		void bc_network_server_manager::get_rtt_time(const platform::bc_network_address& p_address, bc_network_rtt* p_rtt, bc_network_rtt* p_remote_rtt) noexcept
		{
			auto* l_client = _find_client(p_address);
			*p_rtt = l_client->get_rtt_time();
			*p_remote_rtt = l_client->get_remote_rtt_time();
		}

		void bc_network_server_manager::add_rtt_sample(const platform::bc_network_address& p_address, bc_network_rtt p_rtt, bc_network_rtt p_remote_rtt) noexcept
		{
			auto* l_client = _find_client(p_address);

			{
				core_platform::bc_lock_guard l_client_lock(*l_client);

				l_client->add_rtt_time(p_rtt);
				l_client->set_remote_rtt_time(p_remote_rtt);

				const auto l_rtt = l_client->get_rtt_time() / 2;
				const auto l_remote_rtt = l_client->get_remote_rtt_time() / 2;
				for (auto& l_actor : l_client->get_replicated_actors())
				{
					auto* l_network_component = l_actor.get_component<bc_network_component>();
					l_network_component->set_ping(l_rtt, l_remote_rtt);
				}
			}
		}

		core::bc_string bc_network_server_manager::client_connected(const platform::bc_network_address& p_address)
		{
			auto l_error_message = m_hook->client_connected(p_address);
			if(!l_error_message.empty())
			{
				core::bc_log(core::bc_log_type::info) << "new client connection rejected " << p_address << core::bc_lend;
				return l_error_message;
			}

			{
				core_platform::bc_shared_mutex_guard l_clients_lock(m_clients_lock);
				m_clients.push_back(bc_network_server_manager_client(p_address));

				if (!m_scene_name.empty())
				{
					auto& l_client = m_clients.back();
					l_client.add_message(bc_make_network_message(bc_scene_change_network_message(m_scene_name)));
					l_client.set_ready_for_sync(false);
				}
			}

			core::bc_log(core::bc_log_type::info) << "new client connected " << p_address << core::bc_lend;

			return {};
		}

		void bc_network_server_manager::client_disconnected(const platform::bc_network_address& p_address)
		{
			auto l_client_ite = std::end(m_clients);
			
			{
				core_platform::bc_shared_mutex_shared_guard l_lock(m_clients_lock);

				l_client_ite = std::find_if
				(
					std::begin(m_clients),
					std::end(m_clients),
					[&](const bc_network_server_manager_client& p_client)
					{
						return p_client.get_address() == p_address;
					}
				);
				if (l_client_ite == std::end(m_clients))
				{
					core::bc_log(core::bc_log_type::error) << "unable to find disconnected client" << core::bc_lend;
					return;
				}
			}

			core::bc_vector_frame<bc_actor> l_client_replicated_actors;

			{
				core_platform::bc_lock_guard l_client_lock(*l_client_ite);

				// Make a copy of actors because internal array will be modified in 'remove_actor'
				l_client_replicated_actors.assign
				(
					std::begin(l_client_ite->get_replicated_actors()),
					std::end(l_client_ite->get_replicated_actors())
				);
			}
			
			for (auto& l_actor : l_client_replicated_actors)
			{
				remove_actor(p_address, l_actor);
			}

			{
				core_platform::bc_shared_mutex_guard l_lock(m_clients_lock);
				m_clients.erase(l_client_ite);
			}

			core::bc_log(core::bc_log_type::info) << "client disconnected " << p_address << core::bc_lend;
			m_hook->client_disconnected(p_address);
		}

		void bc_network_server_manager::acknowledge_message(const platform::bc_network_address& p_address, bc_network_message_id p_ack_id, core::bc_string p_ack_data)
		{ 
			auto* l_client = _find_client(p_address);
			bc_network_message_ptr l_message;
			
			{
				core_platform::bc_lock_guard l_client_lock(*l_client);

				auto l_messages = l_client->get_messages_waiting_acknowledgment();
				const auto l_msg_ite = std::find_if
				(
					std::begin(l_messages),
					std::end(l_messages),
					[&](const bc_retry_message& p_msg)
					{
						return p_msg.m_message->get_id() == p_ack_id;
					}
				);

				if (l_msg_ite == std::end(l_messages))
				{
					// It is possible multiple ack message arrive but only one of them will see original message
					core::bc_log(core::bc_log_type::warning) << "no message was found with id " << p_ack_id << " to acknowledge" << core::bc_lend;
					return;
				}

				l_message = l_msg_ite->m_message;
				l_client->erase_message_waiting_acknowledgment(p_ack_id);
			}

			l_message->acknowledge
			(
				bc_network_message_server_acknowledge_context{ *this, p_address, std::move(p_ack_data) }
			);
		}

		void bc_network_server_manager::replicate_scene(const platform::bc_network_address& p_address)
		{
			auto* l_client = _find_client(p_address);

			{
				core_platform::bc_mutex_guard l_actors_lock(m_actors_lock);
				core_platform::bc_lock_guard l_client_lock(*l_client);

				auto l_client_actors = l_client->get_replicated_actors();
				
				for (auto [l_id, l_actor] : m_network_actors)
				{
					const auto l_client_actor_ite = std::find(std::begin(l_client_actors), std::end(l_client_actors), l_actor);
					if(l_client_actor_ite == std::end(l_client_actors))
					{
						l_client->add_message(bc_make_network_message(bc_actor_replicate_network_message(l_actor)));
					}
				}

				l_client->set_ready_for_sync(true);
			}
		}

		void bc_network_server_manager::replicate_actor(const platform::bc_network_address& p_address, bc_actor& p_actor)
		{
			auto* l_network_component = p_actor.get_component<bc_network_component>();
			if (!l_network_component)
			{
				core::bc_log(core::bc_log_type::error, bcL("actor without network component cannot be replicated"));
				return;
			}

			auto l_actor_network_id = m_actor_network_id_counter.fetch_add(1);
			
			{
				core_platform::bc_mutex_guard l_lock(m_actors_lock);
				m_network_actors.insert(std::make_pair(l_actor_network_id, p_actor));
				l_network_component->set_network_id(l_actor_network_id);
			}

			auto* l_client = _find_client(p_address);
			{
				core_platform::bc_lock_guard l_lock(*l_client);
				l_client->add_replicated_actor(p_actor);
			}

			{
				core_platform::bc_shared_mutex_shared_guard l_clients_lock(m_clients_lock);
				_add_message_to_clients(bc_make_network_message(bc_actor_replicate_network_message(p_actor)), &p_address);
			}
		}

		void bc_network_server_manager::remove_actor(const platform::bc_network_address& p_address, bc_actor& p_actor)
		{
			auto* l_network_component = p_actor.get_component<bc_network_component>();
			const auto l_actor_network_id = l_network_component->get_network_id();

			{
				core_platform::bc_mutex_guard l_lock(m_actors_lock);

				const auto l_ite = m_network_actors.find(l_actor_network_id);
				if (l_ite == std::cend(m_network_actors))
				{
					core::bc_log(core::bc_log_type::warning, bcL("actor was not found in replicated actor list to remove"));
					return;
				}

				m_network_actors.erase(l_ite);
			}

			auto* l_client = _find_client(p_address);
			{
				core_platform::bc_lock_guard l_lock(*l_client);
				l_client->erase_replicated_actor(p_actor);
			}

			{
				core_platform::bc_shared_mutex_shared_guard l_clients_lock(m_clients_lock);
				_add_message_to_clients(bc_make_network_message(bc_actor_remove_network_message(l_actor_network_id)), &p_address);
			}

			l_network_component->set_network_id(bc_actor::invalid_id); // Mark as invalid to prevent double removal via network component
			m_game_system->get_scene()->remove_actor(p_actor);
		}

		bc_actor bc_network_server_manager::create_actor(const bcCHAR* p_entity_name, const core::bc_matrix4f& p_transform)
		{
			auto l_actor = m_game_system->get_scene()->create_actor(p_entity_name, p_transform);

			return l_actor;
		}

		bci_network_message_deserialization_visitor::bc_replicated_actor bc_network_server_manager::get_actor(bc_actor_network_id p_actor_network_id)
		{
			{
				core_platform::bc_mutex_guard l_lock(m_actors_lock);

				const auto l_ite = m_network_actors.find(p_actor_network_id);
				if (l_ite == std::end(m_network_actors))
				{
					return { bc_actor(), false };
				}

				return { l_ite->second, false };
			}
		}

		void bc_network_server_manager::_add_message_to_clients(bc_network_message_ptr p_message, const platform::bc_network_address* p_exclude_client)
		{
			// Clients lock should be acquired by the caller
			for (auto& l_client : m_clients)
			{
				if (p_exclude_client && *p_exclude_client == l_client.get_address())
				{
					continue;
				}
				
				{
					core_platform::bc_lock_guard<bc_network_server_manager_client> l_client_lock(l_client);
					
					if (!l_client.get_ready_for_sync())
					{
						continue;
					}

					l_client.add_message(p_message);
				}
			}
		}

		void bc_network_server_manager::_retry_messages_waiting_acknowledgment(const core_platform::bc_clock::update_param& p_clock, bc_network_server_manager_client& p_client)
		{
			auto l_messages = p_client.get_messages_waiting_acknowledgment();
			auto l_messages_size = l_messages.size();
			const auto l_rtt_time = p_client.get_rtt_time();
			auto l_rtt_multiplier = 2;
#ifdef BC_DEBUG
			l_rtt_multiplier *= 3;
#endif
			
			for (auto l_ite = 0U; l_ite < l_messages_size;)
			{
				auto& l_msg = l_messages[l_ite];
				l_msg.m_elapsed += p_clock.m_elapsed;
				
				if (l_msg.m_elapsed > l_rtt_time * l_rtt_multiplier)
				{
					const bool l_is_ping_message = core::bci_message::is<bc_ping_network_message>(*l_msg.m_message);
					if (l_is_ping_message)
					{
						// No need to retry ping messages
						p_client.erase_message_waiting_acknowledgment(l_msg.m_message->get_id());
						--l_messages_size;
						continue;
					}
					
					l_msg.m_elapsed = 0;
					l_msg.m_message->set_is_retry();
					
					p_client.add_message(l_msg.m_message);
				}

				++l_ite;
			}
		}

		void bc_network_server_manager::_send_to_client(const core_platform::bc_clock::update_param& p_clock, bc_network_server_manager_client& p_client)
		{
			core::bc_const_span<bc_network_message_ptr> l_client_messages;
			
			{
				core_platform::bc_lock_guard l_client_lock(p_client);

				if(p_client.get_ready_for_sync())
				{
					{
						core_platform::bc_mutex_guard l_actors_lock(m_actors_lock);

						for (auto& l_actor : m_network_actors)
						{
							const auto* l_network_component = l_actor.second.get_component<bc_network_component>();
							if(!l_network_component->get_sync_enabled())
							{
								continue;
							}
							
							p_client.add_message(bc_make_network_message(bc_actor_sync_network_message(std::get<bc_actor>(l_actor))));
						}
					}
				}

				_retry_messages_waiting_acknowledgment(p_clock, p_client);
				
				l_client_messages = p_client.get_messages();
			}

			if (l_client_messages.empty())
			{
				return;
			}

			const auto [l_stream_size, l_stream] = m_messages_buffer.serialize(*this, l_client_messages);
			bc_server_socket_send_event l_send_event{ p_client.get_address(), *l_stream, l_stream_size, 0 };
			bc_server_socket_state_machine::process_event(l_send_event);

			if (l_send_event.m_bytes_sent != l_send_event.m_bytes_to_send)
			{
				return;
			}

			{
				core_platform::bc_lock_guard l_client_lock(p_client);

				for (const auto& l_message : l_client_messages)
				{
					if (l_message->need_acknowledgment())
					{
						p_client.add_message_waiting_acknowledgment_if_not_exist(bc_retry_message
						{
							0,
							l_message
						});
					}
				}

				p_client.clear_messages();
				p_client.set_last_sync_time(bc_current_packet_time());
			}

			l_stream->set_position(core::bc_stream_seek::start, 0);
			m_hook->message_packet_sent(p_client.get_address(), *l_stream, l_stream_size, l_client_messages);
		}

		bcSIZE bc_network_server_manager::_receive_from_clients()
		{
			m_memory_buffer.set_position(core::bc_stream_seek::start, 0);
			platform::bc_network_address l_address;

			bc_server_socket_receive_event l_receive_event{ l_address, m_memory_buffer, 0 };
			bc_server_socket_state_machine::process_event(l_receive_event);
			
			const auto l_bytes_received = l_receive_event.m_bytes_received;
			if(!l_bytes_received)
			{
				return l_bytes_received;
			}

			auto* l_client = _find_client(l_address);
			core::bc_span<bc_network_message_ptr> l_messages;

			try
			{
				m_memory_buffer.set_position(core::bc_stream_seek::start, 0);
				l_messages = m_messages_buffer.deserialize(*this, m_memory_buffer, l_bytes_received);
			}
			catch (const std::exception& l_exception)
			{
				core::bc_log(core::bc_log_type::error) << "Deserialization of network packet encountered error: '" << l_exception.what() << "'" << core::bc_lend;
				return l_bytes_received;
			}

			if (!l_client)
			{
				// Check to see if a new client connection is requested
				l_client = _check_if_contains_client_connect_message(l_address, l_messages);
				return l_bytes_received;
			}
			
			const bc_network_message_id l_last_executed_message_id = l_client->get_last_executed_message_id();
			bc_network_message_id l_max_message_id = 0;

			for (const auto& l_message : l_messages)
			{
				const auto l_message_id = l_message->get_id();

				if (l_message_id < l_last_executed_message_id && !l_message->get_is_retry())
				{
					// discard out of order message
					continue;
				}

				core::bc_string* l_ack_data;
				if (l_message->get_is_retry())
				{
					{
						core_platform::bc_lock_guard l_client_lock(*l_client);

						if((l_ack_data = l_client->find_acknowledge_data(l_message_id)) != nullptr)
						{
							// discard duplicate message
							l_client->add_message(bc_make_network_message(bc_acknowledge_network_message(l_message_id, *l_ack_data)));
							continue;
						}
					}
				}

				l_message->execute(bc_network_message_server_context
				{
					*this,
					l_address
				});

				if (l_message->need_acknowledgment())
				{
					auto l_ack_data = l_message->get_acknowledgment_data();
						
					{
						core_platform::bc_lock_guard l_client_lock(*l_client);
							
						l_client->add_message(bc_make_network_message(bc_acknowledge_network_message(l_message_id, l_ack_data)));

						// ping messages are not retried, so there is no need to keep track of them
						const bool l_is_ping_message = core::bci_message::is<bc_ping_network_message>(*l_message);
						if(!l_is_ping_message)
						{
							l_client->add_acknowledged_message(l_message_id, std::move(l_ack_data));
						}
					}
				}

				l_max_message_id = std::max(l_max_message_id, l_message_id);
			}

			l_client->set_last_executed_message_id(l_max_message_id);

			m_memory_buffer.set_position(core::bc_stream_seek::start, 0);
			m_hook->message_packet_received(l_address, m_memory_buffer, l_bytes_received, l_messages);

			return l_bytes_received;
		}

		bc_network_server_manager_client* bc_network_server_manager::_check_if_contains_client_connect_message(platform::bc_network_address& p_address, core::bc_span<bc_network_message_ptr> p_messages)
		{
			for (const auto& l_message : p_messages)
			{
				if (!core::bci_message::is<bc_client_connect_network_message>(*l_message))
				{
					continue;
				}

				l_message->execute
				(
					bc_network_message_server_context
					{
						*this,
						p_address
					}
				);

				// after message execution client should have added
				auto* l_client = _find_client(p_address);
				auto l_ack_data = l_message->get_acknowledgment_data();

				{
					core_platform::bc_lock_guard l_client_lock(*l_client);
					
					l_client->add_message(bc_make_network_message(bc_acknowledge_network_message(l_message->get_id(), l_ack_data)));
					l_client->add_acknowledged_message(l_message->get_id(), std::move(l_ack_data));
				}

				return l_client;
			}

			return nullptr;
		}

		bc_network_server_manager_client* bc_network_server_manager::_find_client(const platform::bc_network_address& p_address)
		{
			{
				core_platform::bc_shared_mutex_shared_guard l_client_lock(m_clients_lock);

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

		void bc_network_server_manager::_event_handler(core::bci_event& p_event)
		{
			auto* l_scene_change_event = core::bci_event::as<bc_event_scene_change>(p_event);
			if(l_scene_change_event)
			{
				// TODO remove network actors
				m_scene_name = l_scene_change_event->get_scene_name();
				send_message(bc_scene_change_network_message(m_scene_name));

				{
					core_platform::bc_shared_mutex_shared_guard l_clients_lock(m_clients_lock);

					for (auto& l_client : m_clients)
					{
						core_platform::bc_lock_guard l_client_lock(l_client);

						l_client.set_ready_for_sync(false);
					}
				}

				return;
			}
		}
	}
}