// [03/23/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Physics/bcPhysicsSimulationCallback.h"

namespace black_cat
{
	namespace game
	{
		void bc_physics_simulation_callback::on_joint_break(physics::bc_joint* p_joints, bcUINT32 p_count)
		{
		}

		void bc_physics_simulation_callback::on_wake(physics::bc_actor* p_actors, bcUINT32 p_count)
		{
		}

		void bc_physics_simulation_callback::on_sleep(physics::bc_actor* p_actors, bcUINT32 p_count)
		{
		}

		void bc_physics_simulation_callback::on_trigger(physics::bc_trigger_pair* p_pairs, bcUINT32 p_count)
		{
		}

		void bc_physics_simulation_callback::on_contact(const physics::bc_contant_pair& p_pair, const physics::bc_contact_shape_pair* p_shape_pairs, bcUINT32 p_count)
		{
		}
	}	
}