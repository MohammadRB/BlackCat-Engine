// [05/27/2021 MRB]

#include "Game/GamePCH.h"

#include "PlatformImp/Network/bcNetworkDefinitions.h"
#include "Game/System/Network/bcNetworkSystem.h"
#include "Game/System/Network/bcNetworkClientManager.h"
#include "Game/System/Network/bcNetworkServerManager.h"

namespace black_cat
{
	namespace game
	{
		bc_network_system::bc_network_system()
			: m_manager_type(bc_network_manager_type::client),
			m_manager(nullptr)
		{
		}

		bc_network_system::bc_network_system(bc_network_system&&) noexcept = default;

		bc_network_system::~bc_network_system()
		{
			if(m_initialized)
			{
				destroy();
			}
		}

		bc_network_system& bc_network_system::operator=(bc_network_system&&) noexcept = default;

		void bc_network_system::start_server(bcUINT16 p_port)
		{
			m_manager = core::bc_make_unique<bc_network_server_manager>(bc_network_server_manager(p_port));
		}

		void bc_network_system::start_client(const bcCHAR* p_ip, bcUINT16 p_port, bci_network_client_manager_hook& p_hook)
		{
			m_manager = core::bc_make_unique<bc_network_client_manager>(bc_network_client_manager(p_ip, p_port, p_hook));
			m_manager_type = bc_network_manager_type::client;
		}

		void bc_network_system::add_actor(bc_actor& p_actor)
		{
		}

		void bc_network_system::remove_actor(bc_actor& p_actor)
		{
		}

		bc_network_message_ptr bc_network_system::create_message_instance(bc_network_message_hash p_hash)
		{
			const auto l_ite = m_message_factories.find(p_hash);
			if(l_ite == std::cend(m_message_factories))
			{
				return nullptr;
			}

			auto l_command = l_ite->second();
			return l_command;
		}

		void bc_network_system::update(const core_platform::bc_clock::update_param& p_clock)
		{
			if(!m_manager)
			{
				return;
			}

			m_manager->update(p_clock);
		}

		void bc_network_system::_initialize()
		{
			platform::bc_initialize_socket_library();
		}

		void bc_network_system::_destroy()
		{
			if(m_manager)
			{
				m_manager.reset();
			}
			
			platform::bc_cleanup_socket_library();
		}
	}
}