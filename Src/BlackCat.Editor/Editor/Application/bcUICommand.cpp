// [11/08/2018 MRB]

#include "Editor/EditorPCH.h"

#include "Core/Utility/bcEnumOperand.h"
#include "Game/Object/Mesh/bcMeshUtility.h"
#include "Game/Object/Animation/Job/bcMeshColliderSkinningAnimationJob.h"
#include "Editor/Application/bcUICommand.h"

namespace black_cat
{
	namespace editor
	{
		bc_iui_command::state_ptr bc_iui_command::create_state(state_context& p_context) const
		{
			return nullptr;
		}

		void bc_iui_command::update_ui(update_ui_context& p_context)
		{
		}

		physics::bc_ray bc_iui_command::get_pointer_ray(const update_context& p_context,
			bcUINT16 p_point_left,
			bcUINT16 p_point_top) const
		{
			const auto& l_camera = *p_context.m_game_system.get_input_system().get_camera();
			const auto l_pointing_ray = l_camera.project_clip_point_to_3d_ray(p_point_left, p_point_top);

			return physics::bc_ray(l_camera.get_position(), l_pointing_ray, l_camera.get_far_clip());
		}

		bool bc_iui_command::query_ray_in_scene(const update_context& p_context,
			bcUINT16 p_point_left,
			bcUINT16 p_point_top,
			game::bc_actor_group p_query_group,
			physics::bc_query_flags p_flags,
			physics::bc_scene_ray_query_buffer& p_result) const
		{
			const auto l_ray = get_pointer_ray(p_context, p_point_left, p_point_top);
			auto l_query_filter = physics::bc_scene_query_post_filter_callback
			(
				[&p_context, &l_ray, this](const physics::bc_scene_query_post_filter_data& p_filter_data)
				{
					return game::bc_mesh_utility::skinned_mesh_ray_hit_test
					(
						p_context.m_game_system.get_physics_system(),
						l_ray,
						p_filter_data
					).first;
				}
			);

			auto& l_px_scene = p_context.m_game_system.get_scene()->get_px_scene();

			{
				physics::bc_scene_shared_lock l_lock(&l_px_scene);

				const bool l_px_hit_result = l_px_scene.raycast
				(
					l_ray,
					p_result,
					physics::bc_hit_flag::hit_info,
					p_flags,
					static_cast<physics::bc_query_group>(p_query_group),
					&l_query_filter
				);
				return l_px_hit_result;
			}
		}
	}
}