// [05/27/2021 MRB]

#pragma once

#include "Game/System/Network/bcNetworkManager.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_network_server_manager : public bci_network_manager
		{
		public:
			bc_network_server_manager(bcUINT16 p_port);

			bc_network_server_manager(bc_network_server_manager&&) noexcept;

			~bc_network_server_manager() override;

			bc_network_server_manager& operator=(bc_network_server_manager&&) noexcept;

			void add_actor(bc_actor& p_actor) override;
			
			void remove_actor(bc_actor& p_actor) override;

			void send_command(bc_network_command_ptr p_command) override;
			
			void update(const core_platform::bc_clock& p_clock) override;
			
		private:
		};
	}
}