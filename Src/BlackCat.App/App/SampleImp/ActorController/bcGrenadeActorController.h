// [11/05/2021 MRB]

#pragma once

#include "Game/Object/Scene/ActorComponent/bcActorNetworkController.h"
#include "App/SampleImp/ActorController/bcRigidDynamicNetworkActorController.h"
#include "App/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_grenade_actor_controller : public bc_rigid_dynamic_network_actor_controller
	{
	private:
		void initialize(const game::bc_actor_component_initialize_context& p_context) override;
		
		void update_origin_instance(const game::bc_actor_component_update_content& p_context) override;
		
		void update_replicated_instance(const game::bc_actor_component_update_content& p_context) override;

		void handle_event(const game::bc_actor_component_event_context& p_context) override;

		const bcCHAR* m_explosion_entity_name{ nullptr };
		bcFLOAT m_lifetime{ 0 };
	};
}