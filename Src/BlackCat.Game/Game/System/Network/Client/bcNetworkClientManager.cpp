// [06/01/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Utility/bcLogger.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/Component/bcNetworkComponent.h"
#include "Game/System/Network/Client/bcNetworkClientManager.h"
#include "Game/System/Network/Message/bcActorReplicateNetworkMessage.h"
#include "Game/System/Network/Message/bcActorSyncNetworkMessage.h"
#include "Game/System/Network/Message/bcActorRemoveNetworkMessage.h"

namespace black_cat
{
	namespace game
	{
		bc_network_client_manager::bc_network_client_manager(bc_network_system& p_network_system, bci_network_client_manager_hook& p_hook, const bcCHAR* p_ip, bcUINT16 p_port)
			: bc_client_socket_state_machine(m_socket),
			m_ip(p_ip),
			m_port(p_port),
			m_socket_is_connected(false),
			m_socket_is_ready(false),
			m_hook(&p_hook),
			m_last_sync_time(0),
			m_rtt_sampler(0),
			m_messages_buffer(p_network_system)
		{
			m_socket = platform::bc_non_block_socket
			(
				platform::bc_socket_address::inter_network,
				platform::bc_socket_type::data_gram,
				platform::bc_socket_protocol::udp
			);

			bc_client_socket_connect_event l_connect_event{ m_ip, m_port };
			bc_client_socket_state_machine::process_event(l_connect_event);
		}

		bc_network_client_manager::bc_network_client_manager(bc_network_client_manager&& p_other) noexcept
			: bc_client_socket_state_machine(std::move(p_other)),
			m_ip(p_other.m_ip),
			m_port(p_other.m_port),
			m_socket_is_connected(p_other.m_socket_is_connected),
			m_socket_is_ready(p_other.m_socket_is_ready),
			m_socket(std::move(p_other.m_socket)),
			m_hook(p_other.m_hook),
			m_last_sync_time(p_other.m_last_sync_time),
			m_rtt_sampler(p_other.m_rtt_sampler),
			m_new_actors(std::move(p_other.m_new_actors)),
			m_sync_actors(std::move(p_other.m_sync_actors)),
			m_network_actors(std::move(p_other.m_network_actors)),
			m_messages(std::move(p_other.m_messages)),
			m_messages_waiting_acknowledgment(std::move(p_other.m_messages_waiting_acknowledgment)),
			m_memory_buffer(std::move(p_other.m_memory_buffer)),
			m_messages_buffer(std::move(p_other.m_messages_buffer))
		{
		}

		bc_network_client_manager::~bc_network_client_manager() = default;

		bc_network_client_manager& bc_network_client_manager::operator=(bc_network_client_manager&& p_other) noexcept
		{
			bc_client_socket_state_machine::operator=(std::move(p_other));
			m_ip = p_other.m_ip;
			m_port = p_other.m_port;
			m_socket_is_connected = p_other.m_socket_is_connected;
			m_socket_is_ready = p_other.m_socket_is_ready;
			m_socket = std::move(p_other.m_socket);
			m_hook = p_other.m_hook;
			m_last_sync_time = p_other.m_last_sync_time;
			m_rtt_sampler = p_other.m_rtt_sampler;

			m_new_actors = std::move(p_other.m_new_actors);
			m_sync_actors = std::move(p_other.m_sync_actors);
			m_network_actors = std::move(p_other.m_network_actors);

			m_messages = std::move(p_other.m_messages);
			m_messages_waiting_acknowledgment = std::move(p_other.m_messages_waiting_acknowledgment);
			m_memory_buffer = std::move(p_other.m_memory_buffer);
			m_messages_buffer = std::move(p_other.m_messages_buffer);
			
			return *this;
		}

		void bc_network_client_manager::add_actor(bc_actor& p_actor)
		{
			auto* l_network_component = p_actor.get_component<bc_network_component>();
			if(!l_network_component)
			{
				core::bc_log(core::bc_log_type::error, bcL("actor without network component cannot be added to network sync process"));
				return;
			}

			{
				core::bc_mutex_test_guard l_lock(m_actors_lock);
				m_new_actors.push_back(p_actor);
			}
		}

		void bc_network_client_manager::remove_actor(bc_actor& p_actor)
		{
			auto* l_network_component = p_actor.get_component<bc_network_component>();
			if(l_network_component->get_network_id() != bc_actor::invalid_id)
			{
				{
					core::bc_mutex_test_guard l_lock(m_actors_lock);

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

		void bc_network_client_manager::send_message(bc_network_message_ptr p_command)
		{
			{
				core_platform::bc_mutex_guard l_lock(m_messages_lock);
				m_messages.push_back(p_command);
			}
		}

		void bc_network_client_manager::update(const core_platform::bc_clock::update_param& p_clock)
		{
			bc_client_socket_update_event l_update_event{ p_clock };
			bc_client_socket_state_machine::process_event(l_update_event);

			if(!m_socket_is_ready)
			{
				return;
			}

			const auto l_elapsed_since_last_sync = p_clock.m_total_elapsed - m_last_sync_time;
			if(l_elapsed_since_last_sync < m_rtt_sampler.average_value())
			{
				_receive_from_server(p_clock);
			}
			else
			{
				_send_to_server(p_clock);
			}
		}

		void bc_network_client_manager::on_enter(bc_client_socket_error_state& p_state)
		{
			m_hook->error_occurred(p_state.get_last_exception());
			m_socket_is_connected = false;
			m_socket_is_ready = false;
		}

		void bc_network_client_manager::on_enter(bc_client_socket_connecting_state& p_state)
		{
			m_hook->connecting_to_server(m_ip, m_port);
		}

		void bc_network_client_manager::on_enter(bc_client_socket_connected_state& p_state)
		{
			if(!m_socket_is_connected)
			{
				m_hook->connected_to_server(m_ip, m_port);
				m_socket_is_connected = true;
			}
			m_socket_is_ready = true;
		}

		void bc_network_client_manager::on_enter(bc_client_socket_sending_state& p_state)
		{
			m_socket_is_ready = false;
		}

		void bc_network_client_manager::_send_to_server(const core_platform::bc_clock::update_param& p_clock)
		{
			{
				core_platform::bc_mutex_guard l_lock(m_messages_lock);

				// TODO
				for (auto& l_actor : m_new_actors)
				{
					m_messages.push_back(bc_make_network_message(bc_actor_replicate_network_message()));
				}

				for (auto& l_actor : m_sync_actors)
				{
					m_messages.push_back(bc_make_network_message(bc_actor_sync_network_message()));
				}

				const auto l_serialized_messages = m_messages_buffer.serialize(p_clock.m_total_elapsed, core::bc_make_span(m_messages));
				bc_client_socket_send_event l_send_event{ *l_serialized_messages.first, l_serialized_messages.second, 0 };
				bc_client_socket_state_machine::process_event(l_send_event);

				for (auto& l_message : m_messages)
				{
					m_hook->message_sent(*l_message);

					if (l_message->need_acknowledgment())
					{
						m_messages_waiting_acknowledgment.push_back(std::move(l_message));
					}
				}

				m_new_actors.clear();
				m_messages.clear();
			}
		}
		
		void bc_network_client_manager::_receive_from_server(const core_platform::bc_clock::update_param& p_clock)
		{
			m_memory_buffer.set_position(core::bc_stream_seek::start, 0);
			
			bc_client_socket_receive_event l_receive_event{ m_memory_buffer, 0 };
			bc_client_socket_state_machine::process_event(l_receive_event);

			if(!l_receive_event.m_bytes_received)
			{
				return;
			}

			m_memory_buffer.set_position(core::bc_stream_seek::start, 0);
			const auto l_deserialized_messages = m_messages_buffer.deserialize(l_receive_event.m_stream, l_receive_event.m_bytes_received);

			for(const auto& l_message : l_deserialized_messages.second)
			{
				l_message->execute(bc_network_message_client_context());
				m_hook->message_received(*l_message);
			}
			
			m_rtt_sampler.add_sample(l_deserialized_messages.first);
			m_last_sync_time = p_clock.m_total_elapsed;
		}
	}	
}