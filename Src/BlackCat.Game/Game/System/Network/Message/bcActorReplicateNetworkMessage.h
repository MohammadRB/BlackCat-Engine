// [06/06/2021 MRB]

#pragma once

#include "Game/System/Network/Message/bcNetworkMessage.h"

namespace black_cat
{
	namespace game
	{
		class bc_actor_replicate_network_message : public bci_network_message
		{
			BC_NETWORK_MESSAGE(act_rpl)

		public:
			bc_actor_replicate_network_message();

			bc_actor_replicate_network_message(bc_actor_replicate_network_message&&) noexcept;

			~bc_actor_replicate_network_message() override;

			bc_actor_replicate_network_message& operator=(bc_actor_replicate_network_message&&) noexcept;

			bool need_acknowledgment() const noexcept override;

		private:
			void serialize_message(core::bc_json_key_value& p_params) const override;

			void deserialize_message(const core::bc_json_key_value& p_params) override;
		};

		inline bc_actor_replicate_network_message::bc_actor_replicate_network_message()
			: bci_network_message(message_name())
		{
		}

		inline bc_actor_replicate_network_message::bc_actor_replicate_network_message(bc_actor_replicate_network_message&&) noexcept = default;

		inline bc_actor_replicate_network_message::~bc_actor_replicate_network_message() = default;

		inline bc_actor_replicate_network_message& bc_actor_replicate_network_message::operator=(bc_actor_replicate_network_message&&) noexcept = default;

		inline bool bc_actor_replicate_network_message::need_acknowledgment() const noexcept
		{
			return true;
		}

		inline void bc_actor_replicate_network_message::serialize_message(core::bc_json_key_value& p_params) const
		{
		}

		inline void bc_actor_replicate_network_message::deserialize_message(const core::bc_json_key_value& p_params)
		{
		}
	}
}