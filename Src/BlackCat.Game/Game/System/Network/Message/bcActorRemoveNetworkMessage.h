// [06/06/2021 MRB]

#pragma once

#include "Game/System/Network/Message/bcNetworkMessage.h"

namespace black_cat
{
	namespace game
	{
		class bc_actor_remove_network_message : public bci_network_message
		{
			BC_NETWORK_COMMAND(act_rmv)

		public:
			bc_actor_remove_network_message();

			bc_actor_remove_network_message(bc_actor_remove_network_message&&) noexcept;

			~bc_actor_remove_network_message() override;

			bc_actor_remove_network_message& operator=(bc_actor_remove_network_message&&) noexcept;

		private:
			void serialize_message(core::bc_json_key_value& p_params) const override;

			void deserialize_message(const core::bc_json_key_value& p_params) override;
		};
	}
}