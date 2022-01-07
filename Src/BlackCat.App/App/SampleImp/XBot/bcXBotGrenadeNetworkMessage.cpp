// [11/06/2021 MRB]

#include "App/AppPCH.h"

#include "Core/Utility/bcLogger.h"
#include "Core/Utility/bcJsonParse.h"
#include "Game/Object/Scene/Component/bcNetworkComponent.h"
#include "Game/Object/Scene/Component/bcRigidDynamicComponent.h"
#include "Game/bcUtility.h"
#include "App/SampleImp/XBot/bcXBotGrenadeNetworkMessage.h"
#include "App/SampleImp/XBot/bcXBotNetworkPlayerActorController.h"

namespace black_cat
{
	bc_xbot_start_grenade_throw_network_message::bc_xbot_start_grenade_throw_network_message() noexcept
		: bci_network_message(message_name()),
		m_actor_net_id(game::bc_actor::invalid_id),
		m_is_self_replicate(false)
	
	{
	}

	bc_xbot_start_grenade_throw_network_message::bc_xbot_start_grenade_throw_network_message(const game::bc_actor& p_actor, const bcCHAR* p_grenade_name) noexcept
		: bci_network_message(message_name()),
		m_grenade_entity_name(p_grenade_name),
		m_is_self_replicate(false)
	{
		const auto* l_actor_network_component = p_actor.get_component<game::bc_network_component>();
		m_actor_net_id = l_actor_network_component->get_network_id();
	}

	bc_xbot_start_grenade_throw_network_message::bc_xbot_start_grenade_throw_network_message(bc_xbot_start_grenade_throw_network_message&&) noexcept = default;

	bc_xbot_start_grenade_throw_network_message::~bc_xbot_start_grenade_throw_network_message() = default;

	bc_xbot_start_grenade_throw_network_message& bc_xbot_start_grenade_throw_network_message::operator=(bc_xbot_start_grenade_throw_network_message&&) noexcept = default;

	bool bc_xbot_start_grenade_throw_network_message::need_acknowledgment() const noexcept
	{
		return false;
	}

	void bc_xbot_start_grenade_throw_network_message::execute(const game::bc_network_message_server_context& p_context) noexcept
	{
		if (m_is_self_replicate || m_actor == nullptr)
		{
			return;
		}

		auto* l_mediate_component = m_actor.get_component<game::bc_mediate_component>();
		auto* l_xbot_controller = static_cast<bc_xbot_network_player_actor_controller*>(l_mediate_component->get_controller());

		l_xbot_controller->start_grenade_throw(m_grenade_entity_name.c_str());
	}

	void bc_xbot_start_grenade_throw_network_message::execute(const game::bc_network_message_client_context& p_context) noexcept
	{
		if (m_is_self_replicate || m_actor == nullptr)
		{
			return;
		}

		auto* l_mediate_component = m_actor.get_component<game::bc_mediate_component>();
		auto* l_xbot_controller = static_cast<bc_xbot_network_player_actor_controller*>(l_mediate_component->get_controller());

		l_xbot_controller->start_grenade_throw(m_grenade_entity_name.c_str());
	}

	void bc_xbot_start_grenade_throw_network_message::serialize_message(const game::bc_network_message_serialization_context& p_context)
	{
		json_parse::bc_write(p_context.m_params, "net_id", core::bc_any(m_actor_net_id));
		json_parse::bc_write(p_context.m_params, "ent", core::bc_any(m_grenade_entity_name));
	}

	void bc_xbot_start_grenade_throw_network_message::deserialize_message(const game::bc_network_message_deserialization_context& p_context)
	{
		const bool l_has_net_id = json_parse::bc_load(p_context.m_params, "net_id", m_actor_net_id);
		if (!l_has_net_id)
		{
			core::bc_log(core::bc_log_type::error, bcL("Failed to deserialize actor network id in xbot grenade throw message"));
			return;
		}

		json_parse::bc_load(p_context.m_params, "ent", m_grenade_entity_name);
		std::tie(m_actor, m_is_self_replicate) = p_context.m_visitor.get_actor(m_actor_net_id);
	}

	bc_xbot_grenade_throw_network_message::bc_xbot_grenade_throw_network_message() noexcept
		: bci_network_message(message_name()),
		m_visitor(nullptr)
	{
	}

	bc_xbot_grenade_throw_network_message::bc_xbot_grenade_throw_network_message(const bcCHAR* p_grenade_name,
		const core::bc_matrix4f& p_transform,
		const core::bc_vector3f& p_direction) noexcept
		: bci_network_message(message_name()),
		m_grenade_entity_name(p_grenade_name),
		m_position(p_transform.get_translation()),
		m_rotation(p_transform.get_rotation()),
		m_direction(p_direction),
		m_visitor(nullptr)

	{
	}

	bc_xbot_grenade_throw_network_message::bc_xbot_grenade_throw_network_message(bc_xbot_grenade_throw_network_message&&) noexcept = default;

	bc_xbot_grenade_throw_network_message::~bc_xbot_grenade_throw_network_message() = default;

	bc_xbot_grenade_throw_network_message& bc_xbot_grenade_throw_network_message::operator=(bc_xbot_grenade_throw_network_message&&) noexcept = default;

	bool bc_xbot_grenade_throw_network_message::need_acknowledgment() const noexcept
	{
		return true;
	}

	void bc_xbot_grenade_throw_network_message::execute(const game::bc_network_message_server_context& p_context) noexcept
	{
		game::bc_network_rtt l_rtt;
		game::bc_network_rtt l_remote_rtt;
		p_context.m_visitor.get_rtt_time(p_context.m_address, &l_rtt, &l_remote_rtt);
		
		// grenade actor will be replicated by its network component
		auto l_actor = _create_actor(l_remote_rtt / 2 / 1000);
	}

	void bc_xbot_grenade_throw_network_message::execute(const game::bc_network_message_client_context& p_context) noexcept
	{
	}

	void bc_xbot_grenade_throw_network_message::serialize_message(const game::bc_network_message_serialization_context& p_context)
	{
		json_parse::bc_write(p_context.m_params, "ent", core::bc_any(m_grenade_entity_name));
		json_parse::bc_write(p_context.m_params, "pos", m_position);
		json_parse::bc_write(p_context.m_params, "rot", m_rotation);
		json_parse::bc_write(p_context.m_params, "dir", m_direction);
	}

	void bc_xbot_grenade_throw_network_message::deserialize_message(const game::bc_network_message_deserialization_context& p_context)
	{
		json_parse::bc_load(p_context.m_params, "ent", m_grenade_entity_name);
		json_parse::bc_load(p_context.m_params, "pos", m_position);
		json_parse::bc_load(p_context.m_params, "rot", m_rotation);
		json_parse::bc_load(p_context.m_params, "dir", m_direction);

		m_visitor = &p_context.m_visitor;
	}

	game::bc_actor bc_xbot_grenade_throw_network_message::_create_actor(game::bc_network_rtt p_ping) const
	{
		auto l_transform = core::bc_matrix4f::identity();
		l_transform.set_translation(m_position + core::bc_vector3f::normalize(m_direction));
		l_transform.set_rotation(m_rotation);
		
		auto l_actor = m_visitor->create_actor(m_grenade_entity_name.c_str(), l_transform);
		l_actor.mark_for_double_update();

		auto* l_network_component = l_actor.get_component<game::bc_network_component>();
		auto* l_rigid_dynamic_component = l_actor.get_component<game::bc_rigid_dynamic_component>();

		if(!l_network_component || !l_rigid_dynamic_component)
		{
			core::bc_log(core::bc_log_type::error, bcL("grenade entity must have network and rigid dynamic component"));
			return l_actor;
		}
		
		auto l_rigid_body = l_rigid_dynamic_component->get_dynamic_body();

		{
			game::bc_rigid_component_lock l_lock(*l_rigid_dynamic_component);

			l_rigid_body.add_force(m_direction);
		}

		return l_actor;
	}
}