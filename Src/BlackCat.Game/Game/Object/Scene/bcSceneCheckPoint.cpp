// [04/23/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/bcSceneCheckPoint.h"
#include "Game/Object/Scene/Component/bcRigidBodyComponent.h"

namespace black_cat
{
	namespace game
	{
		bc_scene_check_point::bc_scene_check_point(bc_scene& p_scene)
			: m_scene(&p_scene)
		{
		}

		bc_scene_check_point::bc_scene_check_point(bc_scene_check_point&&) noexcept = default;

		bc_scene_check_point::~bc_scene_check_point() = default;

		bc_scene_check_point& bc_scene_check_point::operator=(bc_scene_check_point&&) noexcept = default;

		core::bc_vector_frame<bc_actor> bc_scene_check_point::export_dynamic_actors() const
		{
			core::bc_vector_frame<bc_actor> l_result;
			l_result.reserve(256);
			
			for(bc_actor& l_actor : m_scene->get_scene_graph())
			{
				auto* l_rigid_body_component = l_actor.get_component<bc_rigid_body_component>();
				if(l_rigid_body_component && l_rigid_body_component->get_body_type() == physics::bc_actor_type::rigid_dynamic)
				{
					l_result.push_back(l_actor);
				}
			}

			return l_result;
		}

		void bc_scene_check_point::import_dynamic_actors(core::bc_vector_frame<std::pair<bc_actor, core::bc_matrix4f>>& p_actors)
		{
			for (bc_actor& l_actor : m_scene->get_scene_graph())
			{
				auto* l_rigid_body_component = l_actor.get_component<bc_rigid_body_component>();
				if (l_rigid_body_component && l_rigid_body_component->get_body_type() == physics::bc_actor_type::rigid_dynamic)
				{
					m_scene->remove_actor(l_actor);
				}
			}

			for(auto& l_actor : p_actors)
			{
				m_scene->add_actor(std::get<bc_actor>(l_actor), std::get<core::bc_matrix4f>(l_actor));
			}
		}
	}	
}