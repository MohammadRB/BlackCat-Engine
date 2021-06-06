// [06/05/2021 MRB]

#pragma once

#include "Game/System/Network/Command/bcNetworkCommand.h"

namespace black_cat
{
	namespace game
	{
		class bc_actor_sync_network_command : public bci_network_command
		{
			BC_NETWORK_COMMAND(act_syc)
			
		public:
			bc_actor_sync_network_command();

			bc_actor_sync_network_command(bc_actor_sync_network_command&&) noexcept;

			~bc_actor_sync_network_command() override;

			bc_actor_sync_network_command& operator=(bc_actor_sync_network_command&&) noexcept;
			
			void serialize(core::bc_json_key_value& p_params) const override;
			
			void deserialize(const core::bc_json_key_value& p_params) override;
			
			void execute(const bc_network_command_context& p_context) override;
		};
	}	
}