// [05/27/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Concurrency/bcConcurrency.h"
#include "Core/Utility/bcLogger.h"
#include "PlatformImp/Network/bcNetworkDefinitions.h"
#include "Game/System/Network/bcNetworkSystem.h"
#include "Game/System/Network/Server/bcNetworkServerManager.h"
#include "Game/System/Network/Client/bcNetworkClientManager.h"

namespace black_cat
{
	namespace game
	{
		bc_network_system::bc_network_system()
			: m_event_manager(nullptr),
			m_game_system(nullptr),
			m_manager(nullptr),
			m_last_rtt_test(0)
		{
		}

		bc_network_system::bc_network_system(bc_network_system&& p_other) noexcept
			: m_event_manager(p_other.m_event_manager),
			m_game_system(p_other.m_game_system),
			m_message_factories(std::move(p_other.m_message_factories)),
			m_manager(std::move(p_other.m_manager)),
			m_last_rtt_test(p_other.m_last_rtt_test)
		{
		}

		bc_network_system::~bc_network_system()
		{
			if(is_initialized())
			{
				destroy();
			}
		}

		bc_network_system& bc_network_system::operator=(bc_network_system&& p_other) noexcept
		{
			m_event_manager = p_other.m_event_manager;
			m_game_system = p_other.m_game_system;
			m_message_factories = std::move(p_other.m_message_factories);
			m_manager = std::move(p_other.m_manager);
			m_last_rtt_test = p_other.m_last_rtt_test;

			return *this;
		}

		void bc_network_system::start_server(bci_network_server_manager_hook& p_hook, bcUINT16 p_port)
		{
			m_manager = core::bc_make_unique<bc_network_server_manager>(bc_network_server_manager(*m_event_manager, *m_game_system, *this, p_hook, p_port));
		}

		void bc_network_system::start_client(bci_network_client_manager_hook& p_hook, const platform::bc_network_address& p_address)
		{
			m_manager = core::bc_make_unique<bc_network_client_manager>(bc_network_client_manager(*m_game_system, *this, p_hook, p_address));
		}

		void bc_network_system::add_actor_to_sync(bc_actor& p_actor)
		{
			m_manager->add_actor_to_sync(p_actor);
		}

		void bc_network_system::remove_actor_from_sync(bc_actor& p_actor)
		{
			m_manager->remove_actor_from_sync(p_actor);
		}

		void bc_network_system::actor_removed(bc_actor& p_actor)
		{
			m_manager->actor_removed(p_actor);
		}

		void bc_network_system::update(const core_platform::bc_clock::update_param& p_clock)
		{
			if(!m_manager)
			{
				return;
			}

			m_last_rtt_test += p_clock.m_elapsed;
			const auto l_send_rtt = m_last_rtt_test >= 500;
			
			m_manager->update(bc_network_manager_update_context{ p_clock, l_send_rtt });

			if (l_send_rtt)
			{
				m_last_rtt_test = 0;
			}
		}

		core::bc_task<void> bc_network_system::update_async(const core_platform::bc_clock::update_param& p_clock)
		{
			auto l_task = core::bc_concurrency::start_task
			(
				core::bc_delegate<void()>
				(
					[=, &p_clock]()
					{
						update(p_clock);
					}
				)
			);

			return l_task;
		}

		bc_network_message_ptr bc_network_system::create_message_instance(bc_network_message_hash p_hash)
		{
			const auto l_ite = m_message_factories.find(p_hash);
			if (l_ite == std::cend(m_message_factories))
			{
				core::bc_log(core::bc_log_type::error) << "cannot find network message with hash " << p_hash << core::bc_lend;
				return nullptr;
			}

			auto l_command = l_ite->second();
			return l_command;
		}
		
		void bc_network_system::_initialize(bc_network_system_parameter p_param)
		{
			m_event_manager = &p_param.m_event_manager;
			m_game_system = &p_param.m_game_system;
			platform::bc_initialize_socket_library();
		}

		void bc_network_system::_destroy()
		{
			if(m_manager)
			{
				m_manager.reset();
			}

			m_event_manager = nullptr;
			m_game_system = nullptr;
			platform::bc_cleanup_socket_library();
		}
	}
}