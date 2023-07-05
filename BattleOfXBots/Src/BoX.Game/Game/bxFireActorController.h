// [19/12/2020 MRB]

#pragma once

#include "Game/Object/Scene/ActorComponent/bcActorController.h"
#include "BoX.Game/bxExport.h"

namespace box
{
	using namespace black_cat;

	class BX_GAME_DLL bx_fire_actor_controller : public game::bci_actor_controller
	{
	private:
		void initialize(const game::bc_actor_component_initialize_context& p_context) override;

		void update(const game::bc_actor_component_update_content& p_context) override;

		bcFLOAT m_light_intensity = 0;
		bcFLOAT m_light_flare_intensity = 0;
	};
}