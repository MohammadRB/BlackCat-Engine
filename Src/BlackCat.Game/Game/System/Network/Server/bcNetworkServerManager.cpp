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
			: bc_server_socket_state_machine(m_socket),
			m_port(p_port),
			m_socket_is_listening(false),
			m_hook(&p_hook),
			m_actor_network_id_counter(0),
			m_messages_buffer(p_network_system)
		{
			m_socket = platform::bc_non_block_socket
			(
				platform::bc_socket_address::inter_network,
				platform::bc_socket_type::data_gram,
				platform::bc_socket_protocol::udp
			);
			
			bc_server_socket_bind_event l_bind_event{ m_port };
			bc_server_socket_state_machine::process_event(l_bind_event);
		}

		bc_network_server_manager::bc_network_server_manager(bc_network_server_manager&& p_other) noexcept
			: bc_server_socket_state_machine(std::move(p_other)),
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
			bc_server_socket_state_machine::operator=(std::move(p_other));
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

		void bc_network_server_manager::send_message(bc_network_message_ptr p_command)
		{
			{
				core_platform::bc_mutex_guard l_lock(m_clients_lock);

				for(auto& l_client : m_clients)
				{
					l_client.add_message(p_command);
				}
			}
		}

		void bc_network_server_manager::update(const core_platform::bc_clock::update_param& p_clock)
		{
			bc_server_socket_update_event l_update_event{ p_clock };
			bc_server_socket_state_machine::process_event(l_update_event);

			for(auto& l_client : m_clients)
			{
				l_client.update(p_clock);
			}

			if (!m_socket_is_listening)
			{
				return;
			}

			for (auto& l_client : m_clients)
			{
				if(!l_client.get_socket_is_ready())
				{
					continue;
				}

				const auto l_elapsed_since_last_sync = p_clock.m_total_elapsed - l_client.get_last_sync_time();
				if (l_elapsed_since_last_sync < l_client.get_rtt_time())
				{
					_receive_from_client(p_clock, l_client);
				}
				else
				{
					_send_to_client(p_clock, l_client);
				}
			}
		}

		void bc_network_server_manager::client_disconnected(bc_network_server_manager_client& p_client)
		{
			m_hook->client_disconnected();
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

		void bc_network_server_manager::on_enter(bc_server_socket_accepting_state& p_state)
		{
			bc_network_server_manager_client l_client(*this, p_state.get_client_socket());
			m_clients.push_back(std::move(l_client));
			m_hook->client_connected();
		}

		void bc_network_server_manager::_send_to_client(const core_platform::bc_clock::update_param& p_clock, bc_network_server_manager_client& p_client)
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

				const auto l_serialized_messages = m_messages_buffer.serialize(p_clock.m_total_elapsed, p_client.get_messages());
				p_client.send(*l_serialized_messages.first, l_serialized_messages.second);

				for (auto& l_message : p_client.get_messages())
				{
					m_hook->message_sent(*l_message);

					if (l_message->need_acknowledgment())
					{
						p_client.add_message_with_acknowledgment(l_message);
					}
				}
			}
		}

		void bc_network_server_manager::_receive_from_client(const core_platform::bc_clock::update_param& p_clock, bc_network_server_manager_client& p_client)
		{
			m_memory_buffer.set_position(core::bc_stream_seek::start, 0);
			const auto l_bytes_received = p_client.receive(m_memory_buffer);
			if(!l_bytes_received)
			{
				return;
			}

			m_memory_buffer.set_position(core::bc_stream_seek::start, 0);
			const auto l_deserialized_messages = m_messages_buffer.deserialize(m_memory_buffer, l_bytes_received);

			for (const auto& l_message : l_deserialized_messages.second)
			{
				l_message->execute(bc_network_message_server_context());
				m_hook->message_received(*l_message);
			}

			p_client.add_rtt_time(l_deserialized_messages.first);
			p_client.set_last_sync_time(p_clock.m_total_elapsed);
		}
	}
}