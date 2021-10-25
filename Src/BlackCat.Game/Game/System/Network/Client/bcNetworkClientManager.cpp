// [06/01/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Content/bcContentManager.h"
#include "Core/Utility/bcCounterValueManager.h"
#include "Core/Utility/bcLogger.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/Component/bcNetworkComponent.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Network/Client/bcNetworkClientManager.h"
#include "Game/System/Network/Message/bcPingNetworkMessage.h"
#include "Game/System/Network/Message/bcAcknowledgeNetworkMessage.h"
#include "Game/System/Network/Message/bcClientConnectNetworkMessage.h"
#include "Game/System/Network/Message/bcActorReplicateNetworkMessage.h"
#include "Game/System/Network/Message/bcActorSyncNetworkMessage.h"
#include "Game/System/Network/Message/bcActorRemoveNetworkMessage.h"
#include "Game/System/Network/Message/bcSceneReplicateNetworkMessage.h"

namespace black_cat
{
	namespace game
	{
		bc_network_client_manager::bc_network_client_manager(bc_game_system& p_game_system,
			bc_network_system& p_network_system, 
			bci_network_client_manager_hook& p_hook, 
			const platform::bc_network_address& p_address)
			: bc_client_socket_state_machine(*BC_NEW(platform::bc_non_block_socket, core::bc_alloc_type::unknown)
				(
					platform::bc_socket_address::inter_network,
					platform::bc_socket_type::data_gram,
					platform::bc_socket_protocol::udp
				)),
			m_game_system(&p_game_system),
			m_socket_is_connected(false),
			m_socket_is_ready(false),
			m_address(p_address),
			m_hook(&p_hook),
			m_last_sync_time(0),
			m_rtt_sampler(100),
			m_remote_rtt(100),
			m_last_executed_message_id(0),
			m_executed_messages(50),
			m_memory_buffer(core::bc_alloc_type::unknown_movable),
			m_messages_buffer(p_network_system)
		{
			m_socket.reset(&bc_client_socket_state_machine::get_socket());

			bc_client_socket_connect_event l_connect_event{ m_address };
			bc_client_socket_state_machine::process_event(l_connect_event);
		}

		bc_network_client_manager::bc_network_client_manager(bc_network_client_manager&& p_other) noexcept
			: bc_client_socket_state_machine(std::move(p_other)),
			m_game_system(p_other.m_game_system),
			m_socket_is_connected(p_other.m_socket_is_connected),
			m_socket_is_ready(p_other.m_socket_is_ready),
			m_address(p_other.m_address),
			m_socket(std::move(p_other.m_socket)),
			m_hook(p_other.m_hook),
			m_last_sync_time(p_other.m_last_sync_time),
			m_rtt_sampler(p_other.m_rtt_sampler),
			m_remote_rtt(p_other.m_remote_rtt),
			m_sync_actors(std::move(p_other.m_sync_actors)),
			m_network_actors(std::move(p_other.m_network_actors)),
			m_last_executed_message_id(p_other.m_last_executed_message_id),
			m_messages(std::move(p_other.m_messages)),
			m_messages_waiting_acknowledgment(std::move(p_other.m_messages_waiting_acknowledgment)),
			m_executed_messages(std::move(p_other.m_executed_messages)),
			m_memory_buffer(std::move(p_other.m_memory_buffer)),
			m_messages_buffer(std::move(p_other.m_messages_buffer))
		{
		}

		// TODO send disconnect msg
		bc_network_client_manager::~bc_network_client_manager() = default;

		bc_network_client_manager& bc_network_client_manager::operator=(bc_network_client_manager&& p_other) noexcept
		{
			bc_client_socket_state_machine::operator=(std::move(p_other));
			m_game_system = p_other.m_game_system;
			m_socket_is_connected = p_other.m_socket_is_connected;
			m_socket_is_ready = p_other.m_socket_is_ready;
			m_address = p_other.m_address;
			m_socket = std::move(p_other.m_socket);
			m_hook = p_other.m_hook;
			m_last_sync_time = p_other.m_last_sync_time;
			m_rtt_sampler = p_other.m_rtt_sampler;
			m_remote_rtt = p_other.m_remote_rtt;

			m_sync_actors = std::move(p_other.m_sync_actors);
			m_network_actors = std::move(p_other.m_network_actors);

			m_last_executed_message_id = p_other.m_last_executed_message_id;
			m_messages = std::move(p_other.m_messages);
			m_messages_waiting_acknowledgment = std::move(p_other.m_messages_waiting_acknowledgment);
			m_memory_buffer = std::move(p_other.m_memory_buffer);
			m_executed_messages = std::move(p_other.m_executed_messages);
			m_messages_buffer = std::move(p_other.m_messages_buffer);
			
			return *this;
		}

		bc_network_type bc_network_client_manager::get_network_type() const noexcept
		{
			return bc_network_type::client;
		}

		void bc_network_client_manager::add_actor_to_sync(bc_actor& p_actor)
		{
			auto* l_network_component = p_actor.get_component<bc_network_component>();
			if(!l_network_component)
			{
				core::bc_log(core::bc_log_type::error, bcL("actor without network component cannot be added to network sync process"));
				return;
			}

			{
				core::bc_mutex_test_guard l_lock(m_actors_lock);
				m_sync_actors.push_back(p_actor);
			}

			send_message(bc_actor_replicate_network_message(p_actor));
		}

		void bc_network_client_manager::remove_actor_from_sync(bc_actor& p_actor)
		{
			auto* l_network_component = p_actor.get_component<bc_network_component>();
			const auto l_network_id = l_network_component->get_network_id();
			if(l_network_id == bc_actor::invalid_id)
			{
				return;
			}
				
			{
				core::bc_mutex_test_guard l_lock(m_actors_lock);

				const auto l_ite = std::find_if(std::cbegin(m_sync_actors), std::cend(m_sync_actors), [&](const bc_actor& p_entry)
				{
					return p_entry.get_component<bc_network_component>()->get_network_id() == l_network_id;
				});
				if (l_ite == std::cend(m_sync_actors))
				{
					core::bc_log(core::bc_log_type::error, bcL("actor was not found in sync list"));
					return;
				}

				m_sync_actors.erase(l_ite);
			}

			send_message(bc_actor_remove_network_message(l_network_id));
		}

		void bc_network_client_manager::send_message(bc_network_message_ptr p_message)
		{
			{
				core_platform::bc_mutex_guard l_lock(m_messages_lock);
				m_messages.push_back(p_message);
			}
		}

		void bc_network_client_manager::update(const bc_network_manager_update_context& p_context)
		{
			bc_client_socket_state_machine::update(p_context.m_clock);

			if(!m_socket_is_ready)
			{
				return;
			}

			if(p_context.m_send_rtt_message)
			{
				send_message(bc_ping_network_message());
			}
			
			const auto l_elapsed_since_last_sync = bc_current_packet_time() - m_last_sync_time;
			const auto l_rtt_time = m_rtt_sampler.average_value() / 2;
			if(l_elapsed_since_last_sync > l_rtt_time)
			{
				_send_to_server(p_context.m_clock);
				bc_client_socket_state_machine::update(p_context.m_clock);
			}

			_receive_from_server();

			auto l_ping_counter = L"In: " + core::bc_to_wstring(m_remote_rtt / 2, L"%.1f") + L" / Out: " + core::bc_to_wstring(l_rtt_time, L"%.1f");
			core::bc_get_service<core::bc_counter_value_manager>()->add_counter("ping", std::move(l_ping_counter));
		}

		void bc_network_client_manager::on_enter(bc_client_socket_error_state& p_state)
		{
			m_socket_is_connected = false;
			m_socket_is_ready = false;

			core::bc_log(core::bc_log_type::error)
				<< "error occurred in network connection: "
				<< (p_state.get_last_exception() ? p_state.get_last_exception()->get_full_message().c_str() : "")
				<< core::bc_lend;
			m_hook->error_occurred(p_state.get_last_exception());
		}

		void bc_network_client_manager::on_enter(bc_client_socket_connecting_state& p_state)
		{
			core::bc_log(core::bc_log_type::info) << "connecting to server " << m_address << core::bc_lend;
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

		bc_network_rtt bc_network_client_manager::get_rtt_time() noexcept
		{
			return m_rtt_sampler.average_value();
		}

		void bc_network_client_manager::add_rtt_sample(bc_network_rtt p_rtt, bc_network_rtt p_remote_rtt) noexcept
		{
			m_rtt_sampler.add_sample(p_rtt);
			m_remote_rtt = p_remote_rtt;

			{
				core::bc_mutex_test_guard l_lock(m_actors_lock);

				const auto l_rtt = m_rtt_sampler.average_value() / 2;
				const auto l_remote_rtt = m_remote_rtt / 2;
				for (auto& [l_net_id, l_actor] : m_network_actors)
				{
					auto* l_network_component = l_actor.get_component<bc_network_component>();
					l_network_component->set_ping(l_rtt, l_remote_rtt);
				}
			}
		}

		void bc_network_client_manager::connection_approved()
		{
			core::bc_log(core::bc_log_type::info) << "connected to server" << core::bc_lend;
			m_hook->connected_to_server(m_address);
		}

		void bc_network_client_manager::acknowledge_message(bc_network_message_id p_ack_id, core::bc_string p_ack_data)
		{
			bc_network_message_ptr l_message;

			{
				core_platform::bc_mutex_guard l_lock(m_messages_lock);

				const auto l_message_ite = std::find_if
				(
					std::cbegin(m_messages_waiting_acknowledgment),
					std::cend(m_messages_waiting_acknowledgment),
					[=](const bc_retry_message& p_msg)
					{
						return p_msg.m_message->get_id() == p_ack_id;
					}
				);
				if (l_message_ite == std::end(m_messages_waiting_acknowledgment))
				{
					// It is possible multiple ack message arrive but only one of them will see original message
					core::bc_log(core::bc_log_type::warning) << "no message was found with id " << p_ack_id << " to acknowledge" << core::bc_lend;
					return;
				}

				l_message = std::move((*l_message_ite).m_message);
				m_messages_waiting_acknowledgment.erase(l_message_ite);
			}

			l_message->acknowledge(bc_network_message_client_acknowledge_context{*this, std::move(p_ack_data)});
		}

		void bc_network_client_manager::load_scene(const bcECHAR* p_scene_name)
		{
			auto* l_content_manager = core::bc_get_service<core::bc_content_manager>();
			auto& l_file_system = m_game_system->get_file_system();

			try
			{
				auto l_scene = l_content_manager->load<bc_scene>
				(
					l_file_system.get_content_scene_path(p_scene_name).c_str(),
					nullptr,
					core::bc_content_loader_parameter()
				);
				m_game_system->set_scene(std::move(l_scene));

				send_message(bc_scene_replicate_network_message());
			}
			catch (...)
			{
				core::bc_log(core::bc_log_type::error) << "Error on loading scene '" << p_scene_name << "'" << core::bc_lend;
			}
		}

		void bc_network_client_manager::replicate_actor(bc_actor& p_actor)
		{
			auto* l_network_component = p_actor.get_component<bc_network_component>();
			if (!l_network_component || l_network_component->get_network_id() == bc_actor::invalid_id)
			{
				core::bc_log(core::bc_log_type::error, bcL("actor without network component or invalid network id cannot be added to network sync process"));
				return;
			}
			
			{
				core::bc_mutex_test_guard l_lock(m_actors_lock);

				m_network_actors.insert(std::make_pair(l_network_component->get_network_id(), p_actor));
			}
		}

		void bc_network_client_manager::remove_actor(bc_actor& p_actor)
		{
			auto* l_network_component = p_actor.get_component<bc_network_component>();
			if (!l_network_component || l_network_component->get_network_id() == bc_actor::invalid_id)
			{
				core::bc_log(core::bc_log_type::error, bcL("actor without network component or invalid network id cannot be removed from network sync process"));
				return;
			}

			{
				core::bc_mutex_test_guard l_lock(m_actors_lock);

				const auto l_ite = m_network_actors.find(l_network_component->get_network_id());
				if(l_ite != std::cend(m_network_actors))
				{
					m_network_actors.erase(l_ite);
				}
			}

			m_game_system->get_scene()->remove_actor(p_actor);
		}

		bc_actor bc_network_client_manager::create_actor(const bcCHAR* p_entity_name)
		{
			auto l_actor = m_game_system->get_scene()->create_actor(p_entity_name, core::bc_matrix4f::identity());
			
			return l_actor;
		}

		bci_network_message_deserialization_visitor::bc_replicated_actor bc_network_client_manager::get_actor(bc_actor_network_id p_actor_network_id)
		{
			{
				core::bc_mutex_test_guard l_lock(m_actors_lock);

				const auto l_ite = m_network_actors.find(p_actor_network_id);
				if(l_ite == std::end(m_network_actors))
				{
					return {bc_actor(), false};
				}

				const auto l_sync_ite = std::find(std::begin(m_sync_actors), std::end(m_sync_actors), l_ite->second);
				
				return { l_ite->second, l_sync_ite != std::end(m_sync_actors) };
			}
		}

		void bc_network_client_manager::_retry_messages_waiting_acknowledgment(const core_platform::bc_clock::update_param& p_clock)
		{
			const auto l_rtt_time = m_rtt_sampler.average_value();
			auto l_rtt_multiplier = 2;
#ifdef BC_DEBUG
			l_rtt_multiplier *= 3;
#endif

			for(auto l_ite = std::begin(m_messages_waiting_acknowledgment); l_ite != std::end(m_messages_waiting_acknowledgment);)
			{
				l_ite->m_elapsed += p_clock.m_elapsed;
				
				if (l_ite->m_elapsed > l_rtt_time * l_rtt_multiplier)
				{
					const bool l_is_ping_message = core::bci_message::is<bc_ping_network_message>(*l_ite->m_message);
					if(l_is_ping_message)
					{
						// No need to retry ping messages
						l_ite = m_messages_waiting_acknowledgment.erase(l_ite);
						continue;
					}

					l_ite->m_elapsed = 0;
					l_ite->m_message->set_is_retry();

					m_messages.push_back(l_ite->m_message);
				}

				++l_ite;
			}
		}

		void bc_network_client_manager::_send_to_server(const core_platform::bc_clock::update_param& p_clock)
		{
			{
				core_platform::bc_mutex_guard l_lock(m_messages_lock);

				for (auto& l_actor : m_sync_actors)
				{
					const auto* l_network_component = l_actor.get_component<bc_network_component>();
					if(l_network_component->get_network_id() == bc_actor::invalid_id)
					{
						// It is not replicated yet
						continue;
					}
					if(!l_network_component->get_sync_enabled())
					{
						continue;
					}
					
					m_messages.push_back(bc_make_network_message(bc_actor_sync_network_message(l_actor)));
				}

				_retry_messages_waiting_acknowledgment(p_clock);
				
				if(m_messages.empty())
				{
					return;
				}

				const auto [l_stream_size, l_stream] = m_messages_buffer.serialize(*this, core::bc_make_cspan(m_messages));
				bc_client_socket_send_event l_send_event{ *l_stream, l_stream_size, 0 };
				bc_client_socket_state_machine::process_event(l_send_event);

				if(l_send_event.m_bytes_sent != l_send_event.m_bytes_to_send)
				{
					return;
				}
				
				for (auto& l_message : m_messages)
				{
					if (l_message->need_acknowledgment())
					{
						const auto l_ite = std::find_if
						(
							std::cbegin(m_messages_waiting_acknowledgment),
							std::cend(m_messages_waiting_acknowledgment),
							[&](const bc_retry_message& p_entry)
							{
								return p_entry.m_message->get_id() == l_message->get_id();
							}
						);
						// If message is not already added 
						if (l_ite == std::cend(m_messages_waiting_acknowledgment))
						{
							m_messages_waiting_acknowledgment.push_back
							(
								bc_retry_message
								{
									0,
									std::move(l_message)
								}
							);
						}
					}
				}

				m_messages.clear();
				m_last_sync_time = bc_current_packet_time();

				l_stream->set_position(core::bc_stream_seek::start, 0);
				m_hook->message_packet_sent(*l_stream, l_stream_size, core::bc_make_cspan(m_messages));
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

			core::bc_span<bc_network_message_ptr> l_messages;

			try
			{
				m_memory_buffer.set_position(core::bc_stream_seek::start, 0);
				l_messages = m_messages_buffer.deserialize(*this, m_memory_buffer, l_receive_event.m_bytes_received);
			}
			catch (const std::exception& l_exception)
			{
				core::bc_log(core::bc_log_type::error) << "Deserialization of network packet encountered error: '" << l_exception.what() << "'" << core::bc_lend;
				return;
			}

			bc_network_message_id l_max_message_id = 0;
			for(const auto& l_message : l_messages)
			{
				const auto l_message_id = l_message->get_id();

				if (l_message_id < m_last_executed_message_id && !l_message->get_is_retry())
				{
					// discard out of order message
					continue;
				}

				core::bc_string* l_ack_data;
				if (l_message->get_is_retry() && (l_ack_data = m_executed_messages.find_acknowledge_data(l_message_id)) != nullptr)
				{
					// discard duplicate message
					send_message(bc_acknowledge_network_message(l_message_id, *l_ack_data));
					continue;
				}
				
				l_message->execute(bc_network_message_client_context
				{
					*this
				});

				if (l_message->need_acknowledgment())
				{
					auto l_ack_data = l_message->get_acknowledgment_data();
					send_message(bc_acknowledge_network_message(l_message_id, l_ack_data));

					// ping messages are not retried, so there is no need to keep track of them
					const bool l_is_ping_message = core::bci_message::is<bc_ping_network_message>(*l_message);
					if(!l_is_ping_message)
					{
						m_executed_messages.add_acknowledged_message(l_message_id, std::move(l_ack_data));
					}
				}
				
				l_max_message_id = std::max(l_max_message_id, l_message_id);
			}

			m_last_executed_message_id = l_max_message_id;

			m_memory_buffer.set_position(core::bc_stream_seek::start, 0);
			m_hook->message_packet_received(m_memory_buffer, l_receive_event.m_bytes_received, l_messages);
		}
	}
}