// [04/23/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/bcSceneCheckPoint.h"
#include "Game/Object/Scene/Component/bcCheckPointComponent.h"

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
			
			for(const auto& l_actor : m_scene->get_scene_graph())
			{
				const auto* l_component = l_actor.get_component<bc_checkpoint_component>();
				if(l_component)
				{
					l_result.push_back(l_actor);
				}
			}

			return l_result;
		}

		void bc_scene_check_point::remove_dynamic_actors() const
		{
			for (auto& l_actor : m_scene->get_scene_graph())
			{
				const auto* l_component = l_actor.get_component<bc_checkpoint_component>();
				if (l_component)
				{
					m_scene->remove_actor(l_actor);
				}
			}
		}
	}	
}