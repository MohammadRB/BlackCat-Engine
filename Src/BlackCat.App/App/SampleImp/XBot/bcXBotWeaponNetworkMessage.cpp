// [10/20/2021 MRB]

#include "App/AppPCH.h"

#include "Core/Utility/bcLogger.h"
#include "Game/Object/Scene/Component/bcControllerComponent.h"
#include "Game/Object/Scene/Component/bcNetworkComponent.h"
#include "App/SampleImp/XBot/bcXBotWeaponNetworkMessage.h"
#include "App/SampleImp/XBot/bcXBotNetworkPlayerActorController.h"

namespace black_cat
{
	bc_xbot_weapon_attach_network_message::bc_xbot_weapon_attach_network_message() noexcept
		: bci_network_message(message_name()),
		m_actor_net_id(game::g_invalid_actor_network_id),
		m_is_self_replicate(false)
	{
	}

	bc_xbot_weapon_attach_network_message::bc_xbot_weapon_attach_network_message(const game::bc_actor& p_actor, const game::bc_actor& p_weapon_actor) noexcept
		: bci_network_message(message_name()),
		m_is_self_replicate(false)
	{
		const auto* l_parent_network_component = p_actor.get_component<game::bc_network_component>();
		m_actor_net_id = l_parent_network_component->get_network_id();

		const auto* l_weapon_mediate_component = p_weapon_actor.get_component<game::bc_mediate_component>();
		m_weapon_entity_name = l_weapon_mediate_component->get_entity_name();
	}

	bc_xbot_weapon_attach_network_message::bc_xbot_weapon_attach_network_message(bc_xbot_weapon_attach_network_message&&) noexcept = default;

	bc_xbot_weapon_attach_network_message::~bc_xbot_weapon_attach_network_message() = default;

	bc_xbot_weapon_attach_network_message& bc_xbot_weapon_attach_network_message::operator=(bc_xbot_weapon_attach_network_message&&) noexcept = default;

	bool bc_xbot_weapon_attach_network_message::need_acknowledgment() const noexcept
	{
		return true;
	}

	void bc_xbot_weapon_attach_network_message::execute(const game::bc_network_message_server_context& p_context) noexcept
	{
		if(m_is_self_replicate || m_actor == nullptr)
		{
			return;
		}

		const auto* l_controller_component = m_actor.get_component<game::bc_controller_component>();
		auto* l_xbot_controller = static_cast<bc_xbot_network_player_actor_controller*>(l_controller_component->get_controller());

		l_xbot_controller->attach_weapon(m_weapon_entity_name.c_str());
	}

	void bc_xbot_weapon_attach_network_message::execute(const game::bc_network_message_client_context& p_context) noexcept
	{
		if (m_is_self_replicate || m_actor == nullptr)
		{
			return;
		}

		const auto* l_mediate_component = m_actor.get_component<game::bc_controller_component>();
		auto* l_xbot_controller = static_cast<bc_xbot_network_player_actor_controller*>(l_mediate_component->get_controller());

		l_xbot_controller->attach_weapon(m_weapon_entity_name.c_str());
	}

	void bc_xbot_weapon_attach_network_message::serialize_message(const game::bc_network_message_serialization_context& p_context)
	{
		p_context.m_params.add_or_update("nid", core::bc_any(m_actor_net_id));
		p_context.m_params.add_or_update("w_ent", core::bc_any(m_weapon_entity_name));
	}

	void bc_xbot_weapon_attach_network_message::deserialize_message(const game::bc_network_message_deserialization_context& p_context)
	{
		const auto* l_net_id = p_context.m_params.find("nid")->second.as<game::bc_actor_network_id>();
		if (!l_net_id)
		{
			core::bc_log(core::bc_log_type::error, bcL("Failed to deserialize actor network id in xbot weapon attach message"));
			return;
		}

		m_actor_net_id = *l_net_id;
		m_weapon_entity_name = p_context.m_params.find("w_ent")->second.as_throw<core::bc_string>();
		std::tie(m_actor, m_is_self_replicate) = p_context.m_visitor.get_actor(m_actor_net_id);
	}

	bc_xbot_weapon_detach_network_message::bc_xbot_weapon_detach_network_message() noexcept
		: bci_network_message(message_name()),
		m_actor_net_id(0),
		m_is_self_replicate(false)
	{
	}

	bc_xbot_weapon_detach_network_message::bc_xbot_weapon_detach_network_message(const game::bc_actor& p_actor) noexcept
		: bci_network_message(message_name()),
		m_is_self_replicate(false)
	{
		const auto* l_parent_network_component = p_actor.get_component<game::bc_network_component>();
		m_actor_net_id = l_parent_network_component->get_network_id();
	}

	bc_xbot_weapon_detach_network_message::bc_xbot_weapon_detach_network_message(bc_xbot_weapon_detach_network_message&&) noexcept = default;

	bc_xbot_weapon_detach_network_message::~bc_xbot_weapon_detach_network_message() = default;

	bc_xbot_weapon_detach_network_message& bc_xbot_weapon_detach_network_message::operator=(bc_xbot_weapon_detach_network_message&&) noexcept = default;

	bool bc_xbot_weapon_detach_network_message::need_acknowledgment() const noexcept
	{
		return true;
	}

	void bc_xbot_weapon_detach_network_message::execute(const game::bc_network_message_server_context& p_context) noexcept
	{
		if (m_is_self_replicate || m_actor == nullptr)
		{
			return;
		}

		const auto* l_controller_component = m_actor.get_component<game::bc_controller_component>();
		auto* l_xbot_controller = static_cast<bc_xbot_network_player_actor_controller*>(l_controller_component->get_controller());

		l_xbot_controller->detach_weapon();
	}

	void bc_xbot_weapon_detach_network_message::execute(const game::bc_network_message_client_context& p_context) noexcept
	{
		if (m_is_self_replicate || m_actor == nullptr)
		{
			return;
		}

		const auto* l_controller_component = m_actor.get_component<game::bc_controller_component>();
		auto* l_xbot_controller = static_cast<bc_xbot_network_player_actor_controller*>(l_controller_component->get_controller());

		l_xbot_controller->detach_weapon();
	}

	void bc_xbot_weapon_detach_network_message::serialize_message(const game::bc_network_message_serialization_context& p_context)
	{
		p_context.m_params.add_or_update("nid", core::bc_any(m_actor_net_id));
	}

	void bc_xbot_weapon_detach_network_message::deserialize_message(const game::bc_network_message_deserialization_context& p_context)
	{
		const auto* l_net_id = p_context.m_params.find("nid")->second.as<game::bc_actor_network_id>();
		if (!l_net_id)
		{
			core::bc_log(core::bc_log_type::error, bcL("Failed to deserialize actor network id in xbot weapon detach message"));
			return;
		}

		m_actor_net_id = *l_net_id;
		std::tie(m_actor, m_is_self_replicate) = p_context.m_visitor.get_actor(m_actor_net_id);
	}

	bc_xbot_weapon_drop_network_message::bc_xbot_weapon_drop_network_message() noexcept
		: bci_network_message(message_name()),
		m_actor_net_id(0),
		m_is_self_replicate(false)
	{
	}

	bc_xbot_weapon_drop_network_message::bc_xbot_weapon_drop_network_message(const game::bc_actor& p_actor) noexcept
		: bci_network_message(message_name()),
		m_is_self_replicate(false)
	{
		const auto* l_parent_network_component = p_actor.get_component<game::bc_network_component>();
		m_actor_net_id = l_parent_network_component->get_network_id();
	}

	bc_xbot_weapon_drop_network_message::bc_xbot_weapon_drop_network_message(bc_xbot_weapon_drop_network_message&&) noexcept = default;

	bc_xbot_weapon_drop_network_message::~bc_xbot_weapon_drop_network_message() = default;

	bc_xbot_weapon_drop_network_message& bc_xbot_weapon_drop_network_message::operator=(bc_xbot_weapon_drop_network_message&&) noexcept = default;

	bool bc_xbot_weapon_drop_network_message::need_acknowledgment() const noexcept
	{
		return true;
	}

	void bc_xbot_weapon_drop_network_message::execute(const game::bc_network_message_server_context& p_context) noexcept
	{
		if(m_is_self_replicate || m_actor == nullptr)
		{
			return;
		}

		const auto* l_controller_component = m_actor.get_component<game::bc_controller_component>();
		auto* l_xbot_controller = static_cast<bc_xbot_network_player_actor_controller*>(l_controller_component->get_controller());

		l_xbot_controller->drop_weapon();
	}

	void bc_xbot_weapon_drop_network_message::execute(const game::bc_network_message_client_context& p_context) noexcept
	{
		if (m_is_self_replicate || m_actor == nullptr)
		{
			return;
		}

		const auto* l_mediate_component = m_actor.get_component<game::bc_controller_component>();
		auto* l_xbot_controller = static_cast<bc_xbot_network_player_actor_controller*>(l_mediate_component->get_controller());

		l_xbot_controller->drop_weapon();
	}

	void bc_xbot_weapon_drop_network_message::serialize_message(const game::bc_network_message_serialization_context& p_context)
	{
		p_context.m_params.add_or_update("nid", core::bc_any(m_actor_net_id));
	}

	void bc_xbot_weapon_drop_network_message::deserialize_message(const game::bc_network_message_deserialization_context& p_context)
	{
		const auto* l_net_id = p_context.m_params.find("nid")->second.as<game::bc_actor_network_id>();
		if (!l_net_id)
		{
			core::bc_log(core::bc_log_type::error, bcL("Failed to deserialize actor network id in xbot weapon drop message"));
			return;
		}

		m_actor_net_id = *l_net_id;
		std::tie(m_actor, m_is_self_replicate) = p_context.m_visitor.get_actor(m_actor_net_id);
	}

	bc_xbot_weapon_shoot_network_message::bc_xbot_weapon_shoot_network_message() noexcept
		: bci_network_message(message_name()),
		m_actor_net_id(game::g_invalid_actor_network_id),
		m_is_self_replicate(false)
	{
	}

	bc_xbot_weapon_shoot_network_message::bc_xbot_weapon_shoot_network_message(const game::bc_actor& p_actor) noexcept
		: bci_network_message(message_name()),
		m_is_self_replicate(false)
	{
		const auto* l_parent_network_component = p_actor.get_component<game::bc_network_component>();
		m_actor_net_id = l_parent_network_component->get_network_id();
	}

	bc_xbot_weapon_shoot_network_message::bc_xbot_weapon_shoot_network_message(bc_xbot_weapon_shoot_network_message&&) noexcept = default;

	bc_xbot_weapon_shoot_network_message::~bc_xbot_weapon_shoot_network_message() = default;

	bc_xbot_weapon_shoot_network_message& bc_xbot_weapon_shoot_network_message::operator=(bc_xbot_weapon_shoot_network_message&&) noexcept = default;

	bool bc_xbot_weapon_shoot_network_message::need_acknowledgment() const noexcept
	{
		return false;
	}

	void bc_xbot_weapon_shoot_network_message::execute(const game::bc_network_message_server_context& p_context) noexcept
	{
		if (m_is_self_replicate || m_actor == nullptr)
		{
			return;
		}

		const auto* l_mediate_component = m_actor.get_component<game::bc_controller_component>();
		auto* l_xbot_controller = static_cast<bc_xbot_network_player_actor_controller*>(l_mediate_component->get_controller());
		l_xbot_controller->shoot_weapon();
	}

	void bc_xbot_weapon_shoot_network_message::execute(const game::bc_network_message_client_context& p_context) noexcept
	{
		if (m_is_self_replicate || m_actor == nullptr)
		{
			return;
		}

		const auto* l_controller_component = m_actor.get_component<game::bc_controller_component>();
		auto* l_xbot_controller = static_cast<bc_xbot_network_player_actor_controller*>(l_controller_component->get_controller());
		l_xbot_controller->shoot_weapon();
	}

	void bc_xbot_weapon_shoot_network_message::serialize_message(const game::bc_network_message_serialization_context& p_context)
	{
		p_context.m_params.add_or_update("nid", core::bc_any(m_actor_net_id));
	}

	void bc_xbot_weapon_shoot_network_message::deserialize_message(const game::bc_network_message_deserialization_context& p_context)
	{
		const auto* l_net_id = p_context.m_params.find("nid")->second.as<game::bc_actor_network_id>();
		if (!l_net_id)
		{
			core::bc_log(core::bc_log_type::error, bcL("Failed to deserialize actor network id in xbot weapon shoot message"));
			return;
		}

		m_actor_net_id = *l_net_id;
		std::tie(m_actor, m_is_self_replicate) = p_context.m_visitor.get_actor(m_actor_net_id);
	}
}