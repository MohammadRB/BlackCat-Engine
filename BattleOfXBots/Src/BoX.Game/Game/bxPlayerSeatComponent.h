// [09/02/2022 MRB]

#pragma once

#include "Core/Container/bcString.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "BoX.Game/Application/bxDefinitions.h"
#include "BoX.Game/bxExport.h"

namespace box
{
	using namespace black_cat;

	class BX_GAME_DLL bx_player_seat_component : public game::bci_actor_component
	{
		BC_COMPONENT(ply_slt, true, false)

	public:
		bx_player_seat_component(game::bc_actor_id p_actor_index, game::bc_actor_component_id p_index);

		bx_player_seat_component(bx_player_seat_component&&) noexcept = default;

		~bx_player_seat_component() override = default;

		bx_player_seat_component& operator=(bx_player_seat_component&&) noexcept = default;
		
		bx_team get_team() const noexcept;

		void initialize(const game::bc_actor_component_initialize_context& p_context) override;

		void handle_event(const game::bc_actor_component_event_context& p_context) override;

	private:
		bx_team m_team;
	};

	inline bx_team bx_player_seat_component::get_team() const noexcept
	{
		return m_team;
	}
}
