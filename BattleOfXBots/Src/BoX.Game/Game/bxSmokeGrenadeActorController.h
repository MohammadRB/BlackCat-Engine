// [06/11/2021 MRB]

#pragma once

#include "Game/Object/Scene/ActorComponent/bcActorNetworkController.h"
#include "App/SampleImp/ActorController/bcRigidDynamicNetworkActorController.h"
#include "BoX.Game/bxExport.h"

namespace box
{
	using namespace black_cat;

	class BX_GAME_DLL bx_smoke_grenade_actor_controller : public bc_rigid_dynamic_network_actor_controller
	{
	private:
		void initialize(const game::bc_actor_component_initialize_context& p_context) override;

		void update_origin_instance(const game::bc_actor_component_update_content& p_context) override;

		void update_replicated_instance(const game::bc_actor_component_update_content& p_context) override;
		
		bcFLOAT m_lifetime{ 0 };
		bcFLOAT m_smoke_time{ 0 };
		core::bc_vector3f m_color;
		bool m_particles_added{ false };
	};
}