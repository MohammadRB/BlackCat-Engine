// [05/27/2021 MRB]

#pragma once

#include "PlatformImp/Network/bcSocket.h"
#include "Game/System/Network/bcNetworkManager.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_network_client_manager : public bci_network_manager
		{
		public:
			bc_network_client_manager();

			bc_network_client_manager(bc_network_client_manager&&) noexcept;

			~bc_network_client_manager() override;

			bc_network_client_manager& operator=(bc_network_client_manager&&) noexcept;

			void add_actor(bc_actor& p_actor) override;
			
			void remove_actor(bc_actor& p_actor) override;

			void update(const core_platform::bc_clock& p_clock, core::bc_span<bc_actor> p_actors) override;
			
		private:
			platform::bc_socket m_socket;
		};
	}	
}