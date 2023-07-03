// [23/04/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/bcSceneCheckPoint.h"
#include "Game/Object/Scene/Component/bcCheckPointComponent.h"

namespace black_cat
{
	namespace game
	{
		bc_scene_checkpoint::bc_scene_checkpoint(bc_scene& p_scene)
			: m_scene(&p_scene)
		{
		}

		bc_scene_checkpoint::bc_scene_checkpoint(bc_scene_checkpoint&&) noexcept = default;

		bc_scene_checkpoint::~bc_scene_checkpoint() = default;

		bc_scene_checkpoint& bc_scene_checkpoint::operator=(bc_scene_checkpoint&&) noexcept = default;

		core::bc_vector_frame<bc_actor> bc_scene_checkpoint::export_dynamic_actors() const
		{
			core::bc_vector_frame<bc_actor> l_result;
			l_result.reserve(256);
			
			for(const auto& l_actor : m_scene->get_scene_graph())
			{
				if(l_actor.has_component<bc_checkpoint_component>())
				{
					l_result.push_back(l_actor);
				}
			}

			return l_result;
		}

		void bc_scene_checkpoint::remove_dynamic_actors() const
		{
			for (auto& l_actor : m_scene->get_scene_graph())
			{
				if (l_actor.has_component<bc_checkpoint_component>())
				{
					m_scene->remove_actor(l_actor);
				}
			}
		}

		core::bc_path bc_scene_checkpoint::get_checkpoint_path(const bc_scene& p_scene, core::bc_estring_view p_checkpoint_name)
		{
			auto l_scene_path = core::bc_path(p_scene.get_path());
			const auto l_new_file_name = l_scene_path.get_filename_without_extension() + bcL(".") + p_checkpoint_name.data() + l_scene_path.get_file_extension();
			l_scene_path.set_filename(l_new_file_name.c_str());

			return l_scene_path;
		}
	}	
}