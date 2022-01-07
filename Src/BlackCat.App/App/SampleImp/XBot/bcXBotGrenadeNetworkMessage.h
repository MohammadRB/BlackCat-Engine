// [11/06/2021 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix3f.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/System/Network/Message/bcNetworkMessage.h"
#include "App/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_xbot_start_grenade_throw_network_message : public game::bci_network_message
	{
		BC_NETWORK_MESSAGE(xbg_sgt)

	public:
		bc_xbot_start_grenade_throw_network_message() noexcept;

		bc_xbot_start_grenade_throw_network_message(const game::bc_actor& p_actor, const bcCHAR* p_grenade_name) noexcept;

		bc_xbot_start_grenade_throw_network_message(bc_xbot_start_grenade_throw_network_message&&) noexcept;

		~bc_xbot_start_grenade_throw_network_message() override;

		bc_xbot_start_grenade_throw_network_message& operator=(bc_xbot_start_grenade_throw_network_message&&) noexcept;

		bool need_acknowledgment() const noexcept override;

		void execute(const game::bc_network_message_server_context& p_context) noexcept override;

		void execute(const game::bc_network_message_client_context& p_context) noexcept override;

	private:
		void serialize_message(const game::bc_network_message_serialization_context& p_context) override;

		void deserialize_message(const game::bc_network_message_deserialization_context& p_context) override;

		game::bc_actor_network_id m_actor_net_id;
		core::bc_string m_grenade_entity_name;
		game::bc_actor m_actor;
		bool m_is_self_replicate;
	};

	class BC_DLL bc_xbot_grenade_throw_network_message : public game::bci_network_message
	{
		BC_NETWORK_MESSAGE(xbg_gt)

	public:
		bc_xbot_grenade_throw_network_message() noexcept;

		bc_xbot_grenade_throw_network_message(const bcCHAR* p_grenade_name, const core::bc_matrix4f& p_transform, const core::bc_vector3f& p_direction) noexcept;

		bc_xbot_grenade_throw_network_message(bc_xbot_grenade_throw_network_message&&) noexcept;

		~bc_xbot_grenade_throw_network_message() override;

		bc_xbot_grenade_throw_network_message& operator=(bc_xbot_grenade_throw_network_message&&) noexcept;

		bool need_acknowledgment() const noexcept override;

		void execute(const game::bc_network_message_server_context& p_context) noexcept override;

		void execute(const game::bc_network_message_client_context& p_context) noexcept override;

	private:
		void serialize_message(const game::bc_network_message_serialization_context& p_context) override;

		void deserialize_message(const game::bc_network_message_deserialization_context& p_context) override;

		game::bc_actor _create_actor(game::bc_network_rtt p_ping) const;
		
		core::bc_string m_grenade_entity_name;
		core::bc_vector3f m_position;
		core::bc_matrix3f m_rotation;
		core::bc_vector3f m_direction;

		game::bci_network_message_deserialization_visitor* m_visitor;
	};
}