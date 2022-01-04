// [01/03/2022 MRB]

#include "App/AppPCH.h"

#include "Core/Utility/bcJsonParse.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "App/SampleImp/XBot/bcXBotRagdollNetworkMessage.h"
#include "App/SampleImp/XBot/bcXBotNetworkPlayerActorController.h"

namespace black_cat
{
	bc_xbot_ragdoll_activation_network_message::bc_xbot_ragdoll_activation_network_message() noexcept:
		bci_network_message(message_name()),
		m_is_self_replicate(false)
	{
	}

	bc_xbot_ragdoll_activation_network_message::bc_xbot_ragdoll_activation_network_message(core::bc_string p_body_part_force, const core::bc_vector3f& p_force) noexcept
		: bci_network_message(message_name()),
		m_is_self_replicate(false),
		m_body_part_force(std::move(p_body_part_force)),
		m_force(p_force)
	{
	}

	bool bc_xbot_ragdoll_activation_network_message::need_acknowledgment() const noexcept
	{
		return true;
	}

	void bc_xbot_ragdoll_activation_network_message::execute(const game::bc_network_message_client_context& p_context) noexcept
	{
		if (m_is_self_replicate || m_actor == nullptr)
		{
			return;
		}

		const auto* l_mediate_component = m_actor.get_component<game::bc_mediate_component>();
		auto* l_xbot_controller = static_cast<bc_xbot_network_player_actor_controller*>(l_mediate_component->get_controller());

		l_xbot_controller->enable_ragdoll(std::move(m_body_part_force), m_force);
	}

	void bc_xbot_ragdoll_activation_network_message::execute(const game::bc_network_message_server_context& p_context) noexcept
	{
		if (m_is_self_replicate || m_actor == nullptr)
		{
			return;
		}

		const auto* l_mediate_component = m_actor.get_component<game::bc_mediate_component>();
		auto* l_xbot_controller = static_cast<bc_xbot_network_player_actor_controller*>(l_mediate_component->get_controller());

		l_xbot_controller->enable_ragdoll(std::move(m_body_part_force), m_force);
	}

	void bc_xbot_ragdoll_activation_network_message::serialize_message(const game::bc_network_message_serialization_context& p_context)
	{
		json_parse::bc_write(p_context.m_params, "frc", m_force);
		json_parse::bc_write(p_context.m_params, "prt", core::bc_any(m_body_part_force));
	}

	void bc_xbot_ragdoll_activation_network_message::deserialize_message(const game::bc_network_message_deserialization_context& p_context)
	{
		const auto l_actor_net_id = json_parse::bc_load(p_context.m_params, "net_id").second.as_throw<game::bc_actor_network_id>();
		m_body_part_force = json_parse::bc_load(p_context.m_params, "prt").second.as_throw<core::bc_string>();
		json_parse::bc_load(p_context.m_params, "frc", m_force);

		std::tie(m_actor, m_is_self_replicate) = p_context.m_visitor.get_actor(l_actor_net_id);
	}
}
