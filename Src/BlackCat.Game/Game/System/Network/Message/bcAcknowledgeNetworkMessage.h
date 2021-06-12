// [06/06/2021 MRB]

#pragma once

#include "Game/System/Network/Message/bcNetworkMessage.h"

namespace black_cat
{
	namespace game
	{
		class bc_acknowledge_network_message : public bci_network_message
		{
			BC_NETWORK_MESSAGE(act_ack)

		public:
			bc_acknowledge_network_message();

			bc_acknowledge_network_message(bc_acknowledge_network_message&&) noexcept;

			~bc_acknowledge_network_message() override;

			bc_acknowledge_network_message& operator=(bc_acknowledge_network_message&&) noexcept;

			bc_network_message_id get_acknowledged_message_id() const noexcept;
		
		private:
			void serialize_message(core::bc_json_key_value& p_params) const override;

			void deserialize_message(const core::bc_json_key_value& p_params) override;
		};

		inline bc_acknowledge_network_message::bc_acknowledge_network_message()
			: bci_network_message(message_name())
		{
		}

		inline bc_acknowledge_network_message::bc_acknowledge_network_message(bc_acknowledge_network_message&&) noexcept = default;

		inline bc_acknowledge_network_message::~bc_acknowledge_network_message() = default;

		inline bc_acknowledge_network_message& bc_acknowledge_network_message::operator=(bc_acknowledge_network_message&&) noexcept = default;

		inline bc_network_message_id bc_acknowledge_network_message::get_acknowledged_message_id() const noexcept
		{
			return 0;
		}

		inline void bc_acknowledge_network_message::serialize_message(core::bc_json_key_value& p_params) const
		{
		}

		inline void bc_acknowledge_network_message::deserialize_message(const core::bc_json_key_value& p_params)
		{
		}
	}
}