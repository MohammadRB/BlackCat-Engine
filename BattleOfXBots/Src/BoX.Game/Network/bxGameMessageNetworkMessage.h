// [28/04/2022 MRB]

#pragma once

#include "Core/Container/bcString.h"
#include "Game/System/Network/Message/bcNetworkMessage.h"
#include "BoX.Game/Application/bxServerNetworkMessageVisitor.h"
#include "BoX.Game/Application/bxClientNetworkMessageVisitor.h"

namespace box
{
	class bx_game_message_network_message : public game::bci_network_message
	{
		BC_NETWORK_MESSAGE(gme_msg)

	public:
		bx_game_message_network_message();

		explicit bx_game_message_network_message(core::bc_string p_str);

		bx_game_message_network_message(bx_game_message_network_message&&) noexcept;

		~bx_game_message_network_message() override;

		bx_game_message_network_message& operator=(bx_game_message_network_message&&) noexcept;

		bool is_in_game_message() const noexcept override;

		void execute(const game::bc_network_message_client_context& p_context) noexcept override;

		void execute(const game::bc_network_message_server_context& p_context) noexcept override;

	private:
		void serialize_message(const game::bc_network_message_serialization_context& p_context) override;

		void deserialize_message(const game::bc_network_message_deserialization_context& p_context) override;

		core::bc_string m_str;
	};

	inline bx_game_message_network_message::bx_game_message_network_message()
		: bci_network_message(message_name())
	{
	}

	inline bx_game_message_network_message::bx_game_message_network_message(core::bc_string p_str)
		: bci_network_message(message_name()),
		m_str(std::move(p_str))
	{
	}

	inline bx_game_message_network_message::bx_game_message_network_message(bx_game_message_network_message&&) noexcept = default;

	inline bx_game_message_network_message::~bx_game_message_network_message() = default;

	inline bx_game_message_network_message& bx_game_message_network_message::operator=(bx_game_message_network_message&&) noexcept = default;

	inline bool bx_game_message_network_message::is_in_game_message() const noexcept
	{
		return false;
	}

	inline void bx_game_message_network_message::execute(const game::bc_network_message_client_context& p_context) noexcept
	{
		auto& l_visitor = static_cast<bx_client_network_message_visitor&>(p_context.m_injected_visitor);
		l_visitor.message_received(m_str);
	}

	inline void bx_game_message_network_message::execute(const game::bc_network_message_server_context& p_context) noexcept
	{
		auto& l_visitor = static_cast<bx_server_network_message_visitor&>(p_context.m_injected_visitor);
		l_visitor.message_received(p_context.m_client.m_id, m_str);
	}

	inline void bx_game_message_network_message::serialize_message(const game::bc_network_message_serialization_context& p_context)
	{
		p_context.m_params.add_or_update("str", core::bc_any(m_str));
	}

	inline void bx_game_message_network_message::deserialize_message(const game::bc_network_message_deserialization_context& p_context)
	{
		if (const auto* l_str = p_context.m_params.find("str")->second.as<core::bc_string>())
		{
			m_str = *l_str;
		}
	}
}