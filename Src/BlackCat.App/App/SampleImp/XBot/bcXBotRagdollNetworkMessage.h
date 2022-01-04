// [01/03/2022 MRB]

#pragma once

#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/System/Network/Message/bcNetworkMessage.h"
#include "App/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_xbot_ragdoll_activation_network_message : public game::bci_network_message
	{
		BC_NETWORK_MESSAGE(xbr_act)

	public:
		bc_xbot_ragdoll_activation_network_message() noexcept;

		bc_xbot_ragdoll_activation_network_message(core::bc_string p_body_part_force, const core::bc_vector3f& p_force) noexcept;

		bc_xbot_ragdoll_activation_network_message(bc_xbot_ragdoll_activation_network_message&&) noexcept = default;

		~bc_xbot_ragdoll_activation_network_message() override = default;

		bc_xbot_ragdoll_activation_network_message& operator=(bc_xbot_ragdoll_activation_network_message&&) noexcept = default;

		bool need_acknowledgment() const noexcept override;

		void execute(const game::bc_network_message_client_context& p_context) noexcept override;

		void execute(const game::bc_network_message_server_context& p_context) noexcept override;

	private:
		void serialize_message(const game::bc_network_message_serialization_context& p_context) override;

		void deserialize_message(const game::bc_network_message_deserialization_context& p_context) override;

		bool m_is_self_replicate;
		game::bc_actor m_actor;

		core::bc_string m_body_part_force;
		core::bc_vector3f m_force;
	};
}
