// [02/14/2022 MRB]

#include "Core/File/bcJsonDocument.h"
#include "Game/bcJsonParse.h"
#include "BoX.Game/Network/bxGameStateNetworkMessage.h"

namespace box
{
	BC_JSON_STRUCTURE(_bx_json_kill_state)
	{
		BC_JSON_VALUE(bcINT32, killer_team);
		BC_JSON_VALUE(core::bc_string, killer_name);
		BC_JSON_VALUE(bcINT32, killed_team);
		BC_JSON_VALUE(core::bc_string, killed_name);
	};

	BC_JSON_STRUCTURE(_bx_json_game_state)
	{
		BC_JSON_VALUE(bcINT32, game_time);
		BC_JSON_ARRAY(core::bc_string, info_messages);
		BC_JSON_ARRAY(_bx_json_kill_state, killing_list);
	};

	bx_game_state_network_message::bx_game_state_network_message() noexcept
		: bci_network_message(message_name()),
		m_state()
	{
	}

	bx_game_state_network_message::bx_game_state_network_message(bx_game_state p_state) noexcept
		: bci_network_message(message_name()),
		m_state(std::move(p_state))
	{
	}

	void bx_game_state_network_message::execute(const game::bc_network_message_client_context& p_context) noexcept
	{
		auto& l_visitor = static_cast<bx_client_network_message_visitor&>(p_context.m_injected_visitor);
		l_visitor.update_game_state(m_state);
	}

	void bx_game_state_network_message::serialize_message(const game::bc_network_message_serialization_context& p_context)
	{
		core::bc_json_document<_bx_json_game_state> l_json{};
		*l_json->m_game_time = m_state.m_game_time;

		for (auto& l_info : m_state.m_info_messages)
		{
			auto& l_entry = l_json->m_info_messages.new_entry();
			*l_entry = std::move(l_info);
		}

		for(auto& l_kill : m_state.m_killing_list)
		{
			auto& l_entry = l_json->m_killing_list.new_entry();
			*l_entry->m_killer_team = static_cast<bcINT32>(l_kill.m_killer_team);
			*l_entry->m_killer_name = l_kill.m_killer_name;
			*l_entry->m_killed_team = static_cast<bcINT32>(l_kill.m_killed_team);
			*l_entry->m_killed_name = l_kill.m_killed_name;
		}

		const auto l_json_str = l_json.write();
		json_parse::bc_write(p_context.m_params, "st", l_json_str);
	}

	void bx_game_state_network_message::deserialize_message(const game::bc_network_message_deserialization_context& p_context)
	{
		core::bc_string l_json_str;
		json_parse::bc_load(p_context.m_params, "st", l_json_str);

		core::bc_json_document<_bx_json_game_state> l_json{};
		l_json.load(l_json_str);

		m_state.m_game_time = *l_json->m_game_time;

		for(auto& l_entry : l_json->m_info_messages)
		{
			m_state.m_info_messages.push_back(std::move(*l_entry));
		}

		for(auto& l_entry : l_json->m_killing_list)
		{
			bx_player_kill_state l_state;
			l_state.m_killer_team = static_cast<bx_team>(*l_entry->m_killer_team);
			l_state.m_killer_name = std::move(*l_entry->m_killer_name);
			l_state.m_killed_team = static_cast<bx_team>(*l_entry->m_killed_team);
			l_state.m_killed_name = std::move(*l_entry->m_killed_name);

			m_state.m_killing_list.push_back(std::move(l_state));
		}
	}
}
