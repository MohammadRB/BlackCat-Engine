// [06/01/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Utility/bcLogger.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/Component/bcNetworkComponent.h"
#include "Game/System/Network/Client/bcNetworkClientManager.h"
#include "Game/System/Network/Message/bcAcknowledgeNetworkMessage.h"
#include "Game/System/Network/Message/bcClientConnectNetworkMessage.h"
#include "Game/System/Network/Message/bcActorReplicateNetworkMessage.h"
#include "Game/System/Network/Message/bcActorSyncNetworkMessage.h"
#include "Game/System/Network/Message/bcActorRemoveNetworkMessage.h"

namespace black_cat
{
	namespace game
	{
		bc_network_client_manager::bc_network_client_manager(bc_network_system& p_network_system, bci_network_client_manager_hook& p_hook, const platform::bc_network_address& p_address)
			: bc_client_socket_state_machine(*BC_NEW(platform::bc_non_block_socket, core::bc_alloc_type::unknown)
			(
				platform::bc_socket_address::inter_network,
				platform::bc_socket_type::data_gram,
				platform::bc_socket_protocol::udp
			)),
			m_address(p_address),
			m_socket_is_connected(false),
			m_socket_is_ready(false),
			m_hook(&p_hook),
			m_last_sync_time(0),
			m_rtt_sampler(100),
			m_messages_buffer(p_network_system)
		{
			m_socket.reset(&bc_client_socket_state_machine::get_socket());

			bc_client_socket_connect_event l_connect_event{ m_address };
			bc_client_socket_state_machine::process_event(l_connect_event);
		}

		bc_network_client_manager::bc_network_client_manager(bc_network_client_manager&& p_other) noexcept
			: bc_client_socket_state_machine(std::move(p_other)),
			m_address(p_other.m_address),
			m_socket_is_connected(p_other.m_socket_is_connected),
			m_socket_is_ready(p_other.m_socket_is_ready),
			m_socket(std::move(p_other.m_socket)),
			m_hook(p_other.m_hook),
			m_last_sync_time(p_other.m_last_sync_time),
			m_rtt_sampler(p_other.m_rtt_sampler),
			m_sync_actors(std::move(p_other.m_sync_actors)),
			m_network_actors(std::move(p_other.m_network_actors)),
			m_messages(std::move(p_other.m_messages)),
			m_messages_waiting_acknowledgment(std::move(p_other.m_messages_waiting_acknowledgment)),
			m_memory_buffer(std::move(p_other.m_memory_buffer)),
			m_messages_buffer(std::move(p_other.m_messages_buffer))
		{
		}

		// TODO send disconnect msg
		bc_network_client_manager::~bc_network_client_manager() = default;

		bc_network_client_manager& bc_network_client_manager::operator=(bc_network_client_manager&& p_other) noexcept
		{
			bc_client_socket_state_machine::operator=(std::move(p_other));
			m_address = p_other.m_address;
			m_socket_is_connected = p_other.m_socket_is_connected;
			m_socket_is_ready = p_other.m_socket_is_ready;
			m_socket = std::move(p_other.m_socket);
			m_hook = p_other.m_hook;
			m_last_sync_time = p_other.m_last_sync_time;
			m_rtt_sampler = p_other.m_rtt_sampler;

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

			// TODO
			send_message(bc_actor_replicate_network_message());
		}

		void bc_network_client_manager::remove_actor(bc_actor& p_actor)
		{
			auto* l_network_component = p_actor.get_component<bc_network_component>();
			if(l_network_component->get_network_id() == bc_actor::invalid_id)
			{
				return;
			}
				
			{
				core::bc_mutex_test_guard l_lock(m_actors_lock);

				const auto l_ite = m_network_actors.find(l_network_component->get_network_id());
				if (l_ite == std::cend(m_network_actors))
				{
					core::bc_log(core::bc_log_type::error, bcL("actor network id was not found"));
					return;
				}

				m_network_actors.erase(l_ite);
				// TODO
				send_message(bc_actor_remove_network_message());
			}
		}

		void bc_network_client_manager::send_message(bc_network_message_ptr p_message)
		{
			{
				core_platform::bc_mutex_guard l_lock(m_messages_lock);
				m_messages.push_back(p_message);
			}
		}

		void bc_network_client_manager::acknowledge_message(bc_network_message_id p_message_id)
		{
			{
				core_platform::bc_mutex_guard l_lock(m_messages_lock);

				const auto l_message_ite = std::find_if
				(
					std::cbegin(m_messages_waiting_acknowledgment),
					std::cend(m_messages_waiting_acknowledgment),
					[=](const bc_message_with_time& p_msg)
					{
						return p_msg.m_message->get_id() == p_message_id;
					}
				);
				if (l_message_ite == std::end(m_messages_waiting_acknowledgment))
				{
					core::bc_log(core::bc_log_type::warning) << "no message was found with id " << p_message_id << " to acknowledge";
					return;
				}

				auto& l_message = *(*l_message_ite).m_message;
				l_message.acknowledge(bc_network_message_client_context
				{
					*this
				});

				m_messages_waiting_acknowledgment.erase(l_message_ite);
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

			const auto l_elapsed_since_last_sync = bc_current_packet_time() - m_last_sync_time;
			if(l_elapsed_since_last_sync > m_rtt_sampler.average_value())
			{
				_send_to_server();
			}

			_receive_from_server();
		}

		void bc_network_client_manager::connection_approved()
		{
			m_hook->connected_to_server(m_address);
		}

		void bc_network_client_manager::actor_replicated(bc_actor& p_actor)
		{
			{
				core::bc_mutex_test_guard l_lock(m_actors_lock);

				m_sync_actors.push_back(p_actor);
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
			m_hook->connecting_to_server(m_address);
		}

		void bc_network_client_manager::on_enter(bc_client_socket_connected_state& p_state)
		{
			if (!m_socket_is_connected)
			{
				m_socket_is_connected = true;
				send_message(bc_client_connect_network_message());
			}
			m_socket_is_ready = true;
		}

		void bc_network_client_manager::on_enter(bc_client_socket_sending_state& p_state)
		{
			m_socket_is_ready = false;
		}

		void bc_network_client_manager::_retry_messages_with_acknowledgment(bc_network_packet_time p_current_time)
		{
			for(auto& l_msg : m_messages_waiting_acknowledgment)
			{
				const auto l_elapsed_since_last_send = p_current_time - l_msg.m_time;
#ifndef BC_DEBUG
				if (l_elapsed_since_last_send > m_rtt_sampler.average_value() * 3)
				{
					l_msg.m_time = p_current_time;
					m_messages.push_back(l_msg.m_message);
				}
#endif				
			}
		}

		void bc_network_client_manager::_send_to_server()
		{
			const auto l_current_time = bc_current_packet_time();
			
			{
				core_platform::bc_mutex_guard l_lock(m_messages_lock);

				// TODO
				for (auto& l_actor : m_sync_actors)
				{
					m_messages.push_back(bc_make_network_message(bc_actor_sync_network_message()));
				}

				_retry_messages_with_acknowledgment(l_current_time);
				
				if(m_messages.empty())
				{
					return;
				}

				const auto [l_stream, l_stream_size] = m_messages_buffer.serialize(l_current_time, core::bc_make_span(m_messages));
				bc_client_socket_send_event l_send_event{ *l_stream, l_stream_size, 0 };
				bc_client_socket_state_machine::process_event(l_send_event);

				for (auto& l_message : m_messages)
				{
					m_hook->message_sent(*l_message);

					if (l_message->need_acknowledgment())
					{
						m_messages_waiting_acknowledgment.push_back(bc_message_with_time
						{
							l_current_time,
							std::move(l_message)
						});
					}
				}

				m_messages.clear();
			}
		}
		
		void bc_network_client_manager::_receive_from_server()
		{
			m_memory_buffer.set_position(core::bc_stream_seek::start, 0);
			
			bc_client_socket_receive_event l_receive_event{ m_memory_buffer, 0 };
			bc_client_socket_state_machine::process_event(l_receive_event);

			if(!l_receive_event.m_bytes_received)
			{
				return;
			}

			m_memory_buffer.set_position(core::bc_stream_seek::start, 0);
			const auto [l_packet_time, l_messages] = m_messages_buffer.deserialize(l_receive_event.m_stream, l_receive_event.m_bytes_received);

			for(const auto& l_message : l_messages)
			{
				if (l_message->need_acknowledgment())
				{
					send_message(bc_acknowledge_network_message(l_message->get_id()));
				}
				
				l_message->execute(bc_network_message_client_context
				{
					*this
				});
				
				m_hook->message_received(*l_message);
			}
			
			m_rtt_sampler.add_sample(bc_elapsed_packet_time(l_packet_time));
			m_last_sync_time = bc_current_packet_time();
		}
	}	
}