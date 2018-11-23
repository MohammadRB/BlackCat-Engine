// [11/08/2018 MRB]

#include "Editor/EditorPCH.h"

#include "Editor/Application/bcUICommand.h"

namespace black_cat
{
	namespace editor
	{
		bool bc_iui_command::is_reversible() const
		{
			return false;
		}

		void bc_iui_command::update_ui(update_ui_context& p_context)
		{
		}

		physics::bc_ray bc_iui_command::get_pointer_ray(const update_context& p_context,
			bcUINT16 p_screen_width,
			bcUINT16 p_screen_height,
			bcUINT16 p_point_left,
			bcUINT16 p_point_top) const
		{
			const auto& l_camera = p_context.m_game_system.get_input_system().get_camera();
			const auto l_pointing_ray = l_camera.project_clip_point_to_3d_ray(p_screen_width, p_screen_height, p_point_left, p_point_top);

			return physics::bc_ray(l_camera.get_position(), l_pointing_ray, l_camera.get_far_clip());
		}

		bool bc_iui_command::query_ray_in_scene(const update_context& p_context,
			bcUINT16 p_screen_width,
			bcUINT16 p_screen_height,
			bcUINT16 p_point_left,
			bcUINT16 p_point_top,
			game::bc_query_group p_query_group,
			physics::bc_query_flags p_flags,
			physics::bc_scene_ray_query_buffer& p_result) const
		{
			const auto l_ray = get_pointer_ray(p_context, p_screen_width, p_screen_height, p_point_left, p_point_top);
			const bool l_px_hit_result = p_context.m_game_system.get_scene()->get_px_scene().raycast
			(
				l_ray,
				p_result,
				physics::bc_hit_flag::hit_info,
				p_flags,
				static_cast<physics::bc_query_group>(p_query_group)
			);

			return l_px_hit_result;
		}

		bool bc_iui_command_reversible::is_reversible() const
		{
			return true;
		}
	}
}