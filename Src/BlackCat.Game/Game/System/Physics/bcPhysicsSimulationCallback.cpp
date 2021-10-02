// [03/23/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Physics/bcPhysicsSystem.h"
#include "Game/System/Physics/bcPhysicsSimulationCallback.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/Component/Event/bcRigidDynamicSleepActorEvent.h"

namespace black_cat
{
	namespace game
	{
		bc_physics_simulation_callback::bc_physics_simulation_callback(bc_physics_system& p_physics_system)
			: m_physics_system(&p_physics_system)
		{
		}

		void bc_physics_simulation_callback::on_joint_break(physics::bc_joint* p_joints, bcUINT32 p_count)
		{
		}

		void bc_physics_simulation_callback::on_wake(physics::bc_actor* p_actors, bcUINT32 p_count)
		{
			for(auto l_ite = 0U; l_ite < p_count; ++l_ite)
			{
				auto& l_px_actor = p_actors[l_ite];
				auto l_actor = m_physics_system->get_game_actor(l_px_actor);
				l_actor.add_event(bc_rigid_dynamic_sleep_actor_event(false));
			}
		}

		void bc_physics_simulation_callback::on_sleep(physics::bc_actor* p_actors, bcUINT32 p_count)
		{
			for (auto l_ite = 0U; l_ite < p_count; ++l_ite)
			{
				auto& l_px_actor = p_actors[l_ite];
				auto l_actor = m_physics_system->get_game_actor(l_px_actor);
				l_actor.add_event(bc_rigid_dynamic_sleep_actor_event(true));
			}
		}

		void bc_physics_simulation_callback::on_trigger(physics::bc_trigger_pair* p_pairs, bcUINT32 p_count)
		{
		}

		void bc_physics_simulation_callback::on_contact(const physics::bc_contant_pair& p_pair, const physics::bc_contact_shape_pair* p_shape_pairs, bcUINT32 p_count)
		{
		}
	}	
}