// [06/27/2021 MRB]

#pragma once

#include "Core/Container/bcString.h"
#include "Game/System/Network/Message/bcNetworkMessage.h"

namespace black_cat
{
	namespace game
	{
		class bc_string_network_message : public bci_network_message
		{
			BC_NETWORK_MESSAGE(str)

		public:
			bc_string_network_message();

			explicit bc_string_network_message(core::bc_string p_str);

			bc_string_network_message(bc_string_network_message&&) noexcept;

			~bc_string_network_message() override;

			bc_string_network_message& operator=(bc_string_network_message&&) noexcept;

		private:
			void serialize_message(core::bc_json_key_value& p_params) const override;

			void deserialize_message(const core::bc_json_key_value& p_params) override;

			core::bc_string m_str;
		};

		inline bc_string_network_message::bc_string_network_message()
			: bci_network_message(message_name())
		{
		}

		inline bc_string_network_message::bc_string_network_message(core::bc_string p_str)
			: bci_network_message(message_name()),
			m_str(std::move(p_str))
		{
		}

		inline bc_string_network_message::bc_string_network_message(bc_string_network_message&&) noexcept = default;

		inline bc_string_network_message::~bc_string_network_message() = default;

		inline bc_string_network_message& bc_string_network_message::operator=(bc_string_network_message&&) noexcept = default;

		inline void bc_string_network_message::serialize_message(core::bc_json_key_value& p_params) const
		{
			p_params.add(std::make_pair("str", core::bc_any(m_str)));
		}

		inline void bc_string_network_message::deserialize_message(const core::bc_json_key_value& p_params)
		{
			const auto l_str = p_params.find("str")->second.as<core::bc_string>();
			if(l_str)
			{
				m_str = *l_str;
			}
		}
	}
}