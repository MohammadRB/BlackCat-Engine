// [06/05/2021 MRB]

#pragma once

#include "Game/System/Network/Message/bcNetworkMessage.h"

namespace black_cat
{
	namespace game
	{
		class bc_actor_sync_network_message : public bci_network_message
		{
			BC_NETWORK_MESSAGE(act_syc)
			
		public:
			bc_actor_sync_network_message();

			bc_actor_sync_network_message(bc_actor_sync_network_message&&) noexcept;

			~bc_actor_sync_network_message() override;

			bc_actor_sync_network_message& operator=(bc_actor_sync_network_message&&) noexcept;
		
		private:
			void serialize_message(core::bc_json_key_value& p_params) const override;
			
			void deserialize_message(const core::bc_json_key_value& p_params) override;
		};

		inline bc_actor_sync_network_message::bc_actor_sync_network_message()
			: bci_network_message(message_name())
		{
		}

		inline bc_actor_sync_network_message::bc_actor_sync_network_message(bc_actor_sync_network_message&&) noexcept = default;

		inline bc_actor_sync_network_message::~bc_actor_sync_network_message() = default;

		inline bc_actor_sync_network_message& bc_actor_sync_network_message::operator=(bc_actor_sync_network_message&&) noexcept = default;

		inline void bc_actor_sync_network_message::serialize_message(core::bc_json_key_value& p_params) const
		{
		}

		inline void bc_actor_sync_network_message::deserialize_message(const core::bc_json_key_value& p_params)
		{
		}
	}	
}