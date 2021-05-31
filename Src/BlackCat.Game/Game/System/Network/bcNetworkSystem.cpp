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
		bc_network_system::bc_network_system() = default;

		bc_network_system::bc_network_system(bc_network_system&&) noexcept = default;

		bc_network_system::~bc_network_system()
		{
			if(m_initialized)
			{
				destroy();
			}
		}

		bc_network_system& bc_network_system::operator=(bc_network_system&&) noexcept = default;

		void bc_network_system::add_actor(bc_actor& p_actor)
		{
		}

		void bc_network_system::remove_actor(bc_actor& p_actor)
		{
		}

		void bc_network_system::update(const core_platform::bc_clock::update_param& p_clock)
		{
		}

		void bc_network_system::_initialize(bc_network_manager_type p_manager)
		{
			platform::bc_initialize_socket_library();
			m_manager = p_manager == bc_network_manager_type::server ?
				core::bc_make_unique<bc_network_server_manager>() :
				core::bc_make_unique<bc_network_client_manager>();
		}

		void bc_network_system::_destroy()
		{
			platform::bc_cleanup_socket_library();
		}
	}
}