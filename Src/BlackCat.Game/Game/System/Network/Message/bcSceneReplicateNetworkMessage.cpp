// [19/08/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Network/Message/bcSceneReplicateNetworkMessage.h"

namespace black_cat::game
{
	bc_scene_replicate_network_message::bc_scene_replicate_network_message()
		: bci_network_message(message_name())
	{
	}

	bc_scene_replicate_network_message::bc_scene_replicate_network_message(bc_scene_replicate_network_message&&) noexcept = default;

	bc_scene_replicate_network_message::~bc_scene_replicate_network_message() = default;

	bc_scene_replicate_network_message& bc_scene_replicate_network_message::operator=(bc_scene_replicate_network_message&&) noexcept = default;

	void bc_scene_replicate_network_message::execute(const bc_network_message_server_context& p_context) noexcept
	{
		p_context.m_visitor.replicate_scene(p_context.m_client.m_address);
	}

	void bc_scene_replicate_network_message::serialize_message(const bc_network_message_serialization_context& p_context)
	{
	}

	void bc_scene_replicate_network_message::deserialize_message(const bc_network_message_deserialization_context& p_context)
	{
	}
}
