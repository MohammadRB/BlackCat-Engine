// [01/06/2021 MRB]

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
			bci_network_message_visitor& p_message_visitor,
			bcUINT16 p_port,
			bcUINT32 p_timeout_ms)
			: bc_server_socket_state_machine
			(
				*BC_NEW(platform::bc_non_block_socket, core::bc_alloc_type::unknown)
				(
					platform::bc_socket_address::inter_network,
					platform::bc_socket_type::data_gram,
					platform::bc_socket_protocol::udp
				)
			),
			m_event_manager(&p_event_manager),
			m_game_system(&p_game_system),
			m_hook(&p_hook),
			m_message_visitor(&p_message_visitor),
			m_port(p_port),
			m_timeout_ms(p_timeout_ms),
			m_socket_is_listening(false),
			m_clients(32, core::bc_alloc_type::program),
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
			m_hook(p_other.m_hook),
			m_message_visitor(p_other.m_message_visitor),
			m_port(p_other.m_port),
			m_timeout_ms(p_other.m_timeout_ms),
			m_socket(std::move(p_other.m_socket)),
			m_socket_is_listening(p_other.m_socket_is_listening),
			m_clients(std::move(p_other.m_clients)),
			m_actor_network_id_counter(p_other.m_actor_network_id_counter.load()),
			m_network_actors(std::move(p_other.m_network_actors)),
			m_memory_buffer(std::move(p_other.m_memory_buffer)),
			m_messages_buffer(std::move(p_other.m_messages_buffer)),
			m_scene_name(std::move(p_other.m_scene_name)),
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
			m_hook = p_other.m_hook;
			m_message_visitor = p_other.m_message_visitor;
			m_port = p_other.m_port;
			m_timeout_ms = p_other.m_timeout_ms;

			m_socket = std::move(p_other.m_socket);
			m_socket_is_listening = p_other.m_socket_is_listening;

			m_clients = std::move(p_other.m_clients);

			m_actor_network_id_counter.store(p_other.m_actor_network_id_counter.load());
			m_network_actors = std::move(p_other.m_network_actors);

			m_memory_buffer = std::move(p_other.m_memory_buffer);
			m_messages_buffer = std::move(p_other.m_messages_buffer);

			m_scene_name = std::move(p_other.m_scene_name);
			m_scene_change_event = std::move(p_other.m_scene_change_event);
			m_scene_change_event.reassign(core::bc_event_manager::delegate_type(*this, &bc_network_server_manager::_event_handler));
			
			return *this;
		}

		bc_network_type bc_network_server_manager::get_network_type() const noexcept
		{
			return bc_network_type::server;
		}

		bc_network_state bc_network_server_manager::get_network_state() const noexcept
		{
			return m_socket_is_listening ? bc_network_state::connected : bc_network_state::error;
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
				platform::bc_mutex_guard l_lock(m_actors_lock);
				m_network_actors.insert(std::make_pair(l_actor_network_id, p_actor));
			}
			
			l_network_component->set_network_ids(l_network_component->get_network_client_id(), l_actor_network_id);
			
			send_message(bc_actor_replicate_network_message(p_actor));
		}

		void bc_network_server_manager::remove_actor_from_sync(bc_actor& p_actor)
		{
			auto* l_network_component = p_actor.get_component<bc_network_component>();
			if (!l_network_component || l_network_component->get_network_id() == g_invalid_actor_network_id)
			{
				core::bc_log(core::bc_log_type::error, bcL("actor without network component or invalid network id cannot be removed from network sync process"));
				return;
			}

			const auto l_network_id = l_network_component->get_network_id();
			
			{
				platform::bc_mutex_guard l_lock(m_actors_lock);

				const auto l_ite = m_network_actors.find(l_network_id);
				if (l_ite == std::cend(m_network_actors))
				{
					core::bc_log(core::bc_log_type::error, bcL("actor was not found in network list to remove"));
					return;
				}

				m_network_actors.erase(l_ite);
			}

			l_network_component->set_as_invalid_network_state();

			send_message(bc_actor_remove_network_message(l_network_id));
		}

		void bc_network_server_manager::actor_removed(bc_actor& p_actor)
		{
			auto* l_network_component = p_actor.get_component<bc_network_component>();
			if (!l_network_component || l_network_component->get_network_id() == g_invalid_actor_network_id)
			{
				core::bc_log(core::bc_log_type::error, bcL("actor without network component or invalid network id cannot be removed from network sync process"));
				return;
			}

			const auto l_client_network_id = l_network_component->get_network_client_id();
			const auto l_network_id = l_network_component->get_network_id();
			
			{
				platform::bc_mutex_guard l_lock(m_actors_lock);

				const auto l_ite = m_network_actors.find(l_network_id);
				if (l_ite == std::cend(m_network_actors))
				{
					core::bc_log(core::bc_log_type::error, bcL("actor was not found in network list to remove"));
					return;
				}

				m_network_actors.erase(l_ite);
			}

			if(l_client_network_id != bc_network_client::invalid_id)
			{
				auto* l_client = _find_client(l_client_network_id);

				{
					platform::bc_lock_guard l_lock(*l_client);
					l_client->erase_replicated_actor(p_actor);
				}
			}

			l_network_component->set_as_invalid_network_state();
		}

		void bc_network_server_manager::send_message(bc_network_message_ptr p_message)
		{
			if (get_network_state() != bc_network_state::connected)
			{
				core::bc_log(core::bc_log_type::error, bcL("Network is in error state. Message will be discarded."));
				return;
			}

			{
				platform::bc_shared_mutex_shared_guard l_lock(m_clients_lock);

				_add_message_to_clients(std::move(p_message));
			}
		}

		void bc_network_server_manager::send_message(const platform::bc_network_address& p_address, bc_network_message_ptr p_message)
		{
			if(get_network_state() != bc_network_state::connected)
			{
				core::bc_log(core::bc_log_type::error, bcL("Network is in error state. message will be discarded."));
				return;
			}

			auto* l_client = _find_client(p_address);

			{
				platform::bc_lock_guard l_client_lock(*l_client);

				if (!l_client->get_ready_for_sync())
				{
					return;
				}

				l_client->add_message(std::move(p_message));
			}
		}

		void bc_network_server_manager::update(const bc_network_manager_update_context& p_context)
		{
			bc_server_socket_state_machine::update(p_context.m_clock);

			if (!m_socket_is_listening)
			{
				return;
			}

			_test_clients_connectivity(p_context.m_clock);

			for(auto& l_client : m_clients)
			{
				if (p_context.m_send_rtt_message)
				{
					{
						platform::bc_lock_guard<bc_network_server_manager_client> l_client_lock(l_client);
						l_client.add_message(bc_make_network_message(bc_ping_network_message()));
					}
				}

				const auto l_elapsed_since_last_sync = static_cast<bc_network_rtt>(bc_current_packet_time() - l_client.get_last_sync_time());
				const auto l_ping_time = l_client.get_rtt_time() / 2;

				if (l_elapsed_since_last_sync > l_ping_time)
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
			if(const auto* l_client_address = p_state.get_last_client_address())
			{
				core::bc_log(core::bc_log_type::error) << "error occurred in client network connection " << *l_client_address;
				core::bc_log(core::bc_log_type::error) << (p_state.get_last_exception() ? (": " + p_state.get_last_exception()->get_full_message()) : "") << core::bc_lend;

				const auto* l_client = _find_client(*l_client_address);
				const auto l_net_client = l_client->to_network_client();
				m_hook->error_occurred(&l_net_client, p_state.get_last_exception());

				client_disconnected(*l_client_address);
				bc_server_socket_state_machine::transfer_state<bc_server_socket_listening_state>();
			}
			else
			{
				core::bc_log(core::bc_log_type::error) << "error occurred in network connection";
				core::bc_log(core::bc_log_type::error) << (p_state.get_last_exception() ? (": " + p_state.get_last_exception()->get_full_message()) : "") << core::bc_lend;

				m_socket_is_listening = false;
				m_hook->error_occurred(nullptr, p_state.get_last_exception());
			}
		}

		void bc_network_server_manager::on_enter(bc_server_socket_listening_state& p_state)
		{
			if(!m_socket_is_listening)
			{
				core::bc_log(core::bc_log_type::info) << "Started listening on port " << m_port << core::bc_lend;

				m_socket_is_listening = true;
				m_hook->started_listening(m_port);
			}
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
				platform::bc_lock_guard l_client_lock(*l_client);

				l_client->add_rtt_time(p_rtt, p_remote_rtt);

				const auto l_ping = l_client->get_rtt_time() / 2;
				const auto l_remote_ping = l_client->get_remote_rtt_time() / 2;

				for (auto& l_actor : l_client->get_replicated_actors())
				{
					auto* l_network_component = l_actor.get_component<bc_network_component>();
					l_network_component->set_ping(l_ping, l_remote_ping);
				}
			}
		}

		void bc_network_server_manager::client_connected(const platform::bc_network_address& p_address, core::bc_string p_name, bc_client_connect_result& p_result)
		{
			if(p_name.empty())
			{
				auto [l_address_family, l_ip, l_port] = p_address.get_traits();
				p_name = "NoName" + core::bc_to_string_frame(l_port);
			}

			bc_network_server_manager_client l_client(p_address, std::move(p_name));
			bc_network_server_manager_client* l_inserted_client;

			auto l_error_message = m_hook->client_connected(l_client.to_network_client());
			if(!l_error_message.empty())
			{
				core::bc_log(core::bc_log_type::info) << "new client connection (" << l_client.get_address() << ") (" << l_client.get_name() << ") rejected with error: " << l_error_message << core::bc_lend;
				p_result.m_error_message = std::move(l_error_message);
				return;
			}

			{
				platform::bc_shared_mutex_guard l_clients_lock(m_clients_lock);

				m_clients.push_back(std::move(l_client));
				l_inserted_client = &m_clients.back();

				if (!m_scene_name.empty())
				{
					l_inserted_client->add_message(bc_make_network_message(bc_scene_change_network_message(m_scene_name)));
					l_inserted_client->set_ready_for_sync(false);
				}
			}

			p_result.m_client_id = l_inserted_client->get_id();
			core::bc_log(core::bc_log_type::info) << "new client connected (" << l_inserted_client->get_address() << ") (" << l_inserted_client->get_name() << ")" << core::bc_lend;
		}

		void bc_network_server_manager::client_disconnected(const platform::bc_network_address& p_address)
		{
			auto l_client_ite = std::end(m_clients);
			
			{
				platform::bc_shared_mutex_shared_guard l_lock(m_clients_lock);

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
				platform::bc_lock_guard l_client_lock(*l_client_ite);

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

			core::bc_log(core::bc_log_type::info) << "client disconnected (" << l_client_ite->get_address() << ") (" << l_client_ite->get_name() << ")" << core::bc_lend;
			m_hook->client_disconnected(l_client_ite->to_network_client());

			{
				platform::bc_shared_mutex_guard l_lock(m_clients_lock);
				m_clients.erase(l_client_ite);
			}
		}

		void bc_network_server_manager::acknowledge_message(const platform::bc_network_address& p_address, bc_network_message_id p_ack_id, core::bc_string p_ack_data)
		{ 
			auto* l_client = _find_client(p_address);
			bc_network_message_ptr l_message;
			
			{
				platform::bc_lock_guard l_client_lock(*l_client);

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
					//core::bc_log(core::bc_log_type::warning) << "no message was found with id " << p_ack_id << " to acknowledge" << core::bc_lend;
					return;
				}

				l_message = l_msg_ite->m_message;
				l_client->erase_message_waiting_acknowledgment(p_ack_id);
			}

			l_message->acknowledge
			(
				bc_network_message_server_acknowledge_context
				{
					*this,
					*m_message_visitor,
					l_client->to_network_client(),
					std::move(p_ack_data)
				}
			);
		}

		void bc_network_server_manager::replicate_scene(const platform::bc_network_address& p_address)
		{
			auto* l_client = _find_client(p_address);

			{
				platform::bc_mutex_guard l_actors_lock(m_actors_lock);
				platform::bc_lock_guard l_client_lock(*l_client);

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
			auto* l_client = _find_client(p_address);
			auto* l_network_component = p_actor.get_component<bc_network_component>();

			if (!l_network_component)
			{
				core::bc_log(core::bc_log_type::error, bcL("actor without network component cannot be replicated"));
				return;
			}

			const auto l_actor_network_id = m_actor_network_id_counter.fetch_add(1);
			const auto l_ping = l_client->get_rtt_time() / 2;
			const auto l_remote_ping = l_client->get_remote_rtt_time() / 2;

			l_network_component->set_network_ids(l_client->get_id(), l_actor_network_id);
			l_network_component->set_ping(l_ping, l_remote_ping);

			{
				platform::bc_mutex_guard l_lock(m_actors_lock);
				m_network_actors.insert(std::make_pair(l_actor_network_id, p_actor));
			}

			{
				platform::bc_lock_guard l_lock(*l_client);
				l_client->add_replicated_actor(p_actor);
			}

			{
				platform::bc_shared_mutex_shared_guard l_clients_lock(m_clients_lock);
				_add_message_to_clients(bc_make_network_message(bc_actor_replicate_network_message(p_actor)), &p_address);
			}
		}

		void bc_network_server_manager::remove_actor(const platform::bc_network_address& p_address, bc_actor& p_actor)
		{
			auto* l_network_component = p_actor.get_component<bc_network_component>();
			if (!l_network_component || l_network_component->get_network_id() == g_invalid_actor_network_id)
			{
				core::bc_log(core::bc_log_type::error, bcL("actor without network component or invalid network id cannot be removed from network sync process"));
				return;
			}

			const auto l_actor_network_id = l_network_component->get_network_id();

			{
				platform::bc_mutex_guard l_lock(m_actors_lock);

				const auto l_ite = m_network_actors.find(l_actor_network_id);
				if (l_ite == std::cend(m_network_actors))
				{
					core::bc_log(core::bc_log_type::error, bcL("actor was not found in network list to remove"));
					return;
				}

				m_network_actors.erase(l_ite);
			}

			auto* l_client = _find_client(p_address);

			{
				platform::bc_lock_guard l_lock(*l_client);
				l_client->erase_replicated_actor(p_actor);
			}

			{
				platform::bc_shared_mutex_shared_guard l_clients_lock(m_clients_lock);
				_add_message_to_clients(bc_make_network_message(bc_actor_remove_network_message(l_actor_network_id)), &p_address);
			}

			// Mark as invalid to prevent double removal via network component
			l_network_component->set_as_invalid_network_state();

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
				platform::bc_mutex_guard l_lock(m_actors_lock);

				const auto l_ite = m_network_actors.find(p_actor_network_id);
				if (l_ite == std::end(m_network_actors))
				{
					return { bc_actor(), false };
				}

				return { l_ite->second, false };
			}
		}

		void bc_network_server_manager::_test_clients_connectivity(const platform::bc_clock::update_param& p_clock)
		{
			auto l_ite = std::begin(m_clients);

			while(l_ite != std::end(m_clients))
			{
				auto& l_client = *l_ite;

				if(l_client.get_ready_for_sync())
				{
					l_client.add_timeout_elapsed(static_cast<bcUINT32>(p_clock.m_elapsed));
				}

				++l_ite;

				if (l_client.get_timeout_elapsed() > m_timeout_ms)
				{
					bc_server_socket_state_machine::get_state<bc_server_socket_error_state>().set_last_exception(bc_network_timeout_exception(), &l_client.get_address());
					bc_server_socket_state_machine::transfer_state<bc_server_socket_error_state>();
				}
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
					platform::bc_lock_guard<bc_network_server_manager_client> l_client_lock(l_client);
					
					if (p_message->is_in_game_message() && !l_client.get_ready_for_sync())
					{
						continue;
					}

					l_client.add_message(p_message);
				}
			}
		}

		void bc_network_server_manager::_retry_messages_waiting_acknowledgment(const platform::bc_clock::update_param& p_clock, bc_network_server_manager_client& p_client)
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

		void bc_network_server_manager::_send_to_client(const platform::bc_clock::update_param& p_clock, bc_network_server_manager_client& p_client)
		{
			core::bc_const_span<bc_network_message_ptr> l_client_messages;
			
			{
				platform::bc_lock_guard l_client_lock(p_client);

				if(p_client.get_ready_for_sync())
				{
					{
						platform::bc_mutex_guard l_actors_lock(m_actors_lock);

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
				platform::bc_lock_guard l_client_lock(p_client);

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
			m_hook->message_packet_sent(p_client.to_network_client(), *l_stream, l_stream_size, l_client_messages);
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
						platform::bc_lock_guard l_client_lock(*l_client);

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
					*m_message_visitor,
					l_client->to_network_client()
				});

				if (l_message->need_acknowledgment())
				{
					auto l_ack_data = l_message->get_acknowledgment_data();
						
					{
						platform::bc_lock_guard l_client_lock(*l_client);
							
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
			m_hook->message_packet_received(l_client->to_network_client(), m_memory_buffer, l_bytes_received, l_messages);

			return l_bytes_received;
		}

		bc_network_server_manager_client* bc_network_server_manager::_check_if_contains_client_connect_message(platform::bc_network_address& p_address, core::bc_span<bc_network_message_ptr> p_messages)
		{
			for (const auto& l_message : p_messages)
			{
				auto* l_client_connection_message = core::bci_message::as<bc_client_connect_network_message>(*l_message);
				if (!l_client_connection_message)
				{
					continue;
				}

				l_client_connection_message->execute
				(
					bc_network_message_server_context
					{
						*this,
						*m_message_visitor,
						{
							p_address,
							bc_network_client::invalid_id,
							""
						}
					}
				);

				auto l_ack_data = l_message->get_acknowledgment_data();
				auto l_ack_message = bc_make_network_message(bc_acknowledge_network_message(l_message->get_id(), l_ack_data));

				// after message execution client should have been added otherwise client connection is not approved
				auto* l_client = _find_client(p_address);

				if(l_client)
				{
					platform::bc_lock_guard l_client_lock(*l_client);

					l_client->add_message(std::move(l_ack_message));
					l_client->add_acknowledged_message(l_message->get_id(), std::move(l_ack_data));
				}
				else
				{
					const auto l_ack_message_span = core::bc_const_span<bc_network_message_ptr>(&l_ack_message, 1U);
					const auto [l_stream_size, l_stream] = m_messages_buffer.serialize(*this, l_ack_message_span);

					bc_server_socket_state_machine::update(platform::bc_clock::update_param(0, 0, 0));
					bc_server_socket_send_event l_send_event{ p_address, *l_stream, l_stream_size, 0 };
					bc_server_socket_state_machine::process_event(l_send_event);
				}

				return l_client;
			}

			return nullptr;
		}

		bc_network_server_manager_client* bc_network_server_manager::_find_client(bc_network_client_id p_id)
		{
			{
				platform::bc_shared_mutex_shared_guard l_client_lock(m_clients_lock);

				const auto l_ite = std::find_if
				(
					std::begin(m_clients),
					std::end(m_clients),
					[&](auto& p_client)
					{
						return p_id == p_client.get_id();
					}
				);
				if (l_ite == std::end(m_clients))
				{
					return nullptr;
				}

				return &*l_ite;
			}
		}

		bc_network_server_manager_client* bc_network_server_manager::_find_client(const platform::bc_network_address& p_address)
		{
			{
				platform::bc_shared_mutex_shared_guard l_client_lock(m_clients_lock);

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
			if(const auto* l_scene_change_event = core::bci_event::as<bc_event_scene_change>(p_event))
			{
				if(l_scene_change_event->is_before_change())
				{
					// Before scene change, disable clients sync to prevent sending game messages like 'actor replicate'
					// until clients load new scene and request scene replication
					{
						platform::bc_shared_mutex_shared_guard l_clients_lock(m_clients_lock);

						for (auto& l_client : m_clients)
						{
							{
								platform::bc_lock_guard l_client_lock(l_client);
								l_client.set_ready_for_sync(false);
							}
						}
					}
				}
				else
				{
					// Network actors should have been removed by their network component
					m_scene_name = l_scene_change_event->get_scene()->get_name();
					send_message(bc_scene_change_network_message(m_scene_name));

					m_hook->scene_changed(l_scene_change_event->get_scene());
				}

				return;
			}
		}
	}
}