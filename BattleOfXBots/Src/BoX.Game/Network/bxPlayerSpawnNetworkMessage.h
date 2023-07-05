// [11/02/2022 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Game/System/Network/Message/bcNetworkMessage.h"
#include "BoX.Game/Application/bxDefinitions.h"
#include "BoX.Game/bxExport.h"

namespace box
{
	using namespace black_cat;

	class BX_GAME_DLL bx_player_spawn_network_message : public game::bci_network_message
	{
		BC_NETWORK_MESSAGE(ply_spw)

	public:
		bx_player_spawn_network_message() noexcept;

		bx_player_spawn_network_message(const core::bc_vector3f& p_position, bx_team p_team) noexcept;

		bx_player_spawn_network_message(bx_player_spawn_network_message&&) noexcept = default;

		~bx_player_spawn_network_message() override = default;

		bx_player_spawn_network_message& operator=(bx_player_spawn_network_message&&) noexcept = default;

	private:
		bool need_acknowledgment() const noexcept override;

		void execute(const game::bc_network_message_client_context& p_context) noexcept override;

		void serialize_message(const game::bc_network_message_serialization_context& p_context) override;

		void deserialize_message(const game::bc_network_message_deserialization_context& p_context) override;

		core::bc_vector3f m_position;
		bx_team m_team;
	};
}
