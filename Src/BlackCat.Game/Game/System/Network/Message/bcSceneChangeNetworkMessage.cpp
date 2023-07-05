// [10/08/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Network/Message/bcSceneChangeNetworkMessage.h"
#include "Game/bcJsonParse.h"

namespace black_cat::game
{
	bc_scene_change_network_message::bc_scene_change_network_message()
		: bci_network_message(message_name())
	{
	}

	bc_scene_change_network_message::bc_scene_change_network_message(core::bc_string p_scene_name)
		: bci_network_message(message_name()),
		  m_scene_name(std::move(p_scene_name))
	{
	}
		
	bc_scene_change_network_message::bc_scene_change_network_message(bc_scene_change_network_message&&) noexcept = default;

	bc_scene_change_network_message::~bc_scene_change_network_message() = default;

	bc_scene_change_network_message& bc_scene_change_network_message::operator=(bc_scene_change_network_message&&) noexcept = default;

	void bc_scene_change_network_message::execute(const bc_network_message_client_context& p_context) noexcept
	{
		const auto l_scene_name = core::bc_to_estring_frame(m_scene_name);
		p_context.m_visitor.load_scene(l_scene_name.c_str());
	}

	void bc_scene_change_network_message::serialize_message(const bc_network_message_serialization_context& p_context)
	{
		json_parse::bc_write(p_context.m_params, "scn", m_scene_name);
	}

	void bc_scene_change_network_message::deserialize_message(const bc_network_message_deserialization_context& p_context)
	{
		json_parse::bc_load(p_context.m_params, "scn", m_scene_name);
	}
}
