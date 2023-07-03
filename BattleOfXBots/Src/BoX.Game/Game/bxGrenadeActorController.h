// [05/11/2021 MRB]

#pragma once

#include "Game/Object/Scene/ActorComponent/bcActorNetworkController.h"
#include "App/SampleImp/ActorController/bcRigidDynamicNetworkActorController.h"
#include "BoX.Game/bxExport.h"

namespace box
{
	using namespace black_cat;

	class BX_GAME_DLL bx_grenade_actor_controller : public bc_rigid_dynamic_network_actor_controller
	{
	private:
		void initialize(const game::bc_actor_component_initialize_context& p_context) override;
		
		void update_origin_instance(const game::bc_actor_component_update_content& p_context) override;
		
		void update_replicated_instance(const game::bc_actor_component_update_content& p_context) override;

		void removed_from_scene(const game::bc_actor_component_event_context& p_context, game::bc_scene& p_scene) override;

		void _explode(game::bc_actor& p_actor);

		const bcCHAR* m_explosion_entity_name{ nullptr };
		game::bc_network_client_id m_player_id{ game::bc_network_client::invalid_id };
		bcFLOAT m_lifetime{ 0 };
		bool m_exploded{ false };
	};
}