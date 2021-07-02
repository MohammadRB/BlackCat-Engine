// [06/01/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Utility/bcLogger.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/Component/bcNetworkComponent.h"
#include "Game/System/Network/Server/bcNetworkServerManager.h"
#include "Game/System/Network/Message/bcActorReplicateNetworkMessage.h"
#include "Game/System/Network/Message/bcActorSyncNetworkMessage.h"

namespace black_cat
{
	namespace game
	{
		bc_network_server_manager::bc_network_server_manager(bc_network_system& p_network_system, bci_network_server_manager_hook& p_hook, bcUINT16 p_port)
			: bc_server_socket1_state_machine(*BC_NEW(platform::bc_non_block_socket, core::bc_alloc_type::unknown)
			(
				platform::bc_socket_address::inter_network,
				platform::bc_socket_type::data_gram,
				platform::bc_socket_protocol::udp
			)),
			m_port(p_port),
			m_socket_is_listening(false),
			m_hook(&p_hook),
			m_actor_network_id_counter(0),
			m_messages_buffer(p_network_system)
		{
			m_socket.reset(&bc_server_socket1_state_machine::get_socket());
			
			bc_server_socket1_bind_event l_bind_event{ m_port };
			bc_server_socket1_state_machine::process_event(l_bind_event);
		}

		bc_network_server_manager::bc_network_server_manager(bc_network_server_manager&& p_other) noexcept
			: bc_server_socket1_state_machine(std::move(p_other)),
			m_port(p_other.m_port),
			m_socket_is_listening(p_other.m_socket_is_listening),
			m_socket(std::move(p_other.m_socket)),
			m_clients(std::move(p_other.m_clients)),
			m_hook(p_other.m_hook),
			m_actor_network_id_counter(p_other.m_actor_network_id_counter.load()),
			m_new_actors(std::move(p_other.m_new_actors)),
			m_network_actors(std::move(p_other.m_network_actors)),
			m_memory_buffer(std::move(p_other.m_memory_buffer)),
			m_messages_buffer(std::move(p_other.m_messages_buffer))
		{
		}

		bc_network_server_manager::~bc_network_server_manager() = default;

		bc_network_server_manager& bc_network_server_manager::operator=(bc_network_server_manager&& p_other) noexcept
		{
			bc_server_socket1_state_machine::operator=(std::move(p_other));
			m_port = p_other.m_port;
			m_socket_is_listening = p_other.m_socket_is_listening;
			m_socket = std::move(p_other.m_socket);
			m_clients = std::move(p_other.m_clients);
			m_hook = p_other.m_hook;
			m_actor_network_id_counter.store(p_other.m_actor_network_id_counter.load());
			m_new_actors = std::move(p_other.m_new_actors);
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
			
			auto l_network_id = m_actor_network_id_counter.fetch_add(1);

			{
				core_platform::bc_mutex_guard l_lock(m_actors_lock);
				m_new_actors.push_back(p_actor);
				m_network_actors.insert(std::make_pair(l_network_id, p_actor));
			}

			l_network_component->set_network_id(l_network_id);
		}

		void bc_network_server_manager::remove_actor(bc_actor& p_actor)
		{
			auto* l_network_component = p_actor.get_component<bc_network_component>();
			if (l_network_component->get_network_id() != bc_actor::invalid_id)
			{
				{
					core_platform::bc_mutex_guard l_lock(m_actors_lock);

					const auto l_ite = m_network_actors.find(l_network_component->get_network_id());
					if (l_ite == std::cend(m_network_actors))
					{
						core::bc_log(core::bc_log_type::error, bcL("actor network id was not found"));
						return;
					}

					m_network_actors.erase(l_ite);
					return;
				}
			}
		}

		void bc_network_server_manager::send_message(bc_network_message_ptr p_message)
		{
			{
				core_platform::bc_mutex_guard l_lock(m_clients_lock);

				for(auto& l_client : m_clients)
				{
					l_client.add_message(p_message);
				}
			}
		}

		void bc_network_server_manager::update(const core_platform::bc_clock::update_param& p_clock)
		{
			bc_server_socket1_update_event l_update_event{ p_clock };
			bc_server_socket1_state_machine::process_event(l_update_event);

			if (!m_socket_is_listening)
			{
				return;
			}

			for (auto& l_client : m_clients)
			{
				const auto l_elapsed_since_last_sync = bc_current_packet_time() - l_client.get_last_sync_time();
				if (l_elapsed_since_last_sync > l_client.get_rtt_time())
				{
					_send_to_client(l_client);
				}
			}

			_receive_from_clients();
		}

		void bc_network_server_manager::client_disconnected(bc_network_server_manager_client1& p_client)
		{
			m_hook->client_disconnected();
		}

		void bc_network_server_manager::on_enter(bc_server_socket1_error_state& p_state)
		{
			m_socket_is_listening = false;
			m_hook->error_occurred(p_state.get_last_exception());
		}

		void bc_network_server_manager::on_enter(bc_server_socket1_listening_state& p_state)
		{
			m_socket_is_listening = true;
			m_hook->started_listening(m_port);
		}

		void bc_network_server_manager::_send_to_client(bc_network_server_manager_client1& p_client)
		{
			{
				core_platform::bc_mutex_guard l_lock(m_clients_lock);

				// TODO
				for (auto& l_actor : m_new_actors)
				{
					p_client.add_message(bc_make_network_message(bc_actor_replicate_network_message()));
				}
				
				for (auto& l_actor : m_network_actors)
				{
					p_client.add_message(bc_make_network_message(bc_actor_sync_network_message()));
				}

				auto l_client_messages = p_client.get_messages();
				if(l_client_messages.empty())
				{
					return;
				}
				
				const auto l_serialized_messages = m_messages_buffer.serialize(bc_current_packet_time(), l_client_messages);
				bc_server_socket1_send_event l_send_event{p_client.get_address(), *l_serialized_messages.first, l_serialized_messages.second};
				bc_server_socket1_state_machine::process_event(l_send_event);
				
				for (auto& l_message : l_client_messages)
				{
					m_hook->message_sent(*l_message);

					if (l_message->need_acknowledgment())
					{
						p_client.add_message_with_acknowledgment(l_message);
					}
				}

				m_new_actors.clear();
				p_client.clear_messages();
			}
		}

		void bc_network_server_manager::_receive_from_clients()
		{
			m_memory_buffer.set_position(core::bc_stream_seek::start, 0);
			platform::bc_network_address l_address;

			bc_server_socket1_receive_event l_receive_event{ l_address, m_memory_buffer, 0 };
			bc_server_socket1_state_machine::process_event(l_receive_event);
			
			const auto l_bytes_received = l_receive_event.m_bytes_received;
			if(!l_bytes_received)
			{
				return;
			}

			const auto l_client_ite = std::find_if
			(
				std::begin(m_clients), 
				std::end(m_clients), 
				[&](const bc_network_server_manager_client1& p_client)
				{
					return p_client.get_address() == l_address;
				}
			);

			// TODO 
			/*if(l_client_ite == std::end(m_clients))
			{
				BC_ASSERT(false);
				return;
			}*/

			auto l = l_address.get_traits();
			
			m_memory_buffer.set_position(core::bc_stream_seek::start, 0);
			const auto [l_packet_time, l_messages] = m_messages_buffer.deserialize(m_memory_buffer, l_bytes_received);

			for (const auto& l_message : l_messages)
			{
				l_message->execute(bc_network_message_server_context());
				m_hook->message_received(*l_message);
			}

			//l_client_ite->add_rtt_time(bc_elapsed_packet_time(l_packet_time));
			//l_client_ite->set_last_sync_time(bc_current_packet_time());
		}
	}
}