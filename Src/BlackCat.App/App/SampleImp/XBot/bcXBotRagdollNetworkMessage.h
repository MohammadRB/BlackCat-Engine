// [03/01/2022 MRB]

#pragma once

#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/System/Network/Message/bcNetworkMessage.h"
#include "App/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_xbot_ragdoll_activation_network_message : public game::bci_network_message
	{
		BC_NETWORK_MESSAGE(xbr_acv)

	public:
		bc_xbot_ragdoll_activation_network_message() noexcept;

		bc_xbot_ragdoll_activation_network_message(const game::bc_actor& p_actor, core::bc_string p_body_part_force, const core::bc_vector3f& p_force) noexcept;

		bc_xbot_ragdoll_activation_network_message(bc_xbot_ragdoll_activation_network_message&&) noexcept = default;

		~bc_xbot_ragdoll_activation_network_message() override = default;

		bc_xbot_ragdoll_activation_network_message& operator=(bc_xbot_ragdoll_activation_network_message&&) noexcept = default;

		bool need_acknowledgment() const noexcept override;

		void execute(const game::bc_network_message_client_context& p_context) noexcept override;

		void execute(const game::bc_network_message_server_context& p_context) noexcept override;

	private:
		void serialize_message(const game::bc_network_message_serialization_context& p_context) override;

		void deserialize_message(const game::bc_network_message_deserialization_context& p_context) override;

		game::bc_actor_network_id m_actor_net_id;
		core::bc_string m_body_part_force;
		core::bc_vector3f m_force;

		bool m_is_self_replicate;
		game::bc_actor m_actor;
	};
}
