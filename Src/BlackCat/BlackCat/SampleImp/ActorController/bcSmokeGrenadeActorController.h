// [11/06/2021 MRB]

#pragma once

#include "Game/Object/Scene/ActorComponent/bcActorNetworkController.h"
#include "BlackCat/SampleImp/ActorController/bcRigidDynamicNetworkActorController.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_smoke_grenade_actor_controller : public bc_rigid_dynamic_network_actor_controller
	{
	public:
		void initialize(const game::bc_actor_component_initialize_context& p_context) override;

		void update_origin_instance(const game::bc_actor_component_update_content& p_context) override;

		void update_replicated_instance(const game::bc_actor_component_update_content& p_context) override;

		void handle_event(const game::bc_actor_component_event_context& p_context) override;

	private:
		bcFLOAT m_lifetime{ 0 };
		bcFLOAT m_smoke_time{ 0 };
		core::bc_vector3f m_color;

		bool m_particles_added{ false };
		bcUINT32 m_num_particles_per_second{ 0 };
		bcUINT32 m_num_spawned_particles_in_current_second{ 0 };
	};
}