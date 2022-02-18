// [02/12/2022 MRB]

#pragma once

#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/System/Network/Message/bcNetworkMessage.h"
#include "BoX.Game/bxExport.h"

namespace box
{
	using namespace black_cat;

	class BX_GAME_DLL bx_player_killed_network_message : public game::bci_network_message
	{
		BC_NETWORK_MESSAGE(ply_kil)

	public:
		bx_player_killed_network_message() noexcept;

		bx_player_killed_network_message(game::bc_network_client_id p_killer_client_id, 
			const game::bc_actor& p_actor, 
			core::bc_string p_body_part_force, 
			const core::bc_vector3f& p_force) noexcept;

		bx_player_killed_network_message(bx_player_killed_network_message&&) noexcept = default;

		~bx_player_killed_network_message() override = default;

		bx_player_killed_network_message& operator=(bx_player_killed_network_message&&) noexcept = default;

		bool need_acknowledgment() const noexcept override;

		void execute(const game::bc_network_message_client_context& p_context) noexcept override;

	private:
		void serialize_message(const game::bc_network_message_serialization_context& p_context) override;

		void deserialize_message(const game::bc_network_message_deserialization_context& p_context) override;

		game::bc_network_client_id m_killer_client_id;
		game::bc_actor_network_id m_actor_net_id;
		core::bc_string m_body_part_force;
		core::bc_vector3f m_force;

		bool m_is_self_replicate;
		game::bc_actor m_actor;
	};
}
