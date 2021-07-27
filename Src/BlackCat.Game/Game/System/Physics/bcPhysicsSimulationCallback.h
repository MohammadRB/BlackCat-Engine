// [03/23/2021 MRB]

#pragma once

#include "Physics/Fundation/bcPhysicsSimulationCallback.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_physics_simulation_callback : public physics::bci_physics_simulation_callback
		{
		public:
			void on_joint_break(physics::bc_joint* p_joints, bcUINT32 p_count) override;
			
			void on_wake(physics::bc_actor* p_actors, bcUINT32 p_count) override;
			
			void on_sleep(physics::bc_actor* p_actors, bcUINT32 p_count) override;
			
			void on_trigger(physics::bc_trigger_pair* p_pairs, bcUINT32 p_count) override;
			
			void on_contact(const physics::bc_contant_pair& p_pair, const physics::bc_contact_shape_pair* p_shape_pairs, bcUINT32 p_count) override;
		};
	}	
}