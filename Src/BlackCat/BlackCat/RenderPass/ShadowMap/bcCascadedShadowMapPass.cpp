// [06/07/2019 MRB]

#include "BlackCat/BlackCatPCH.h"
#include "BlackCat/RenderPass/ShadowMap/bcCascadedShadowMapPass.h"

namespace black_cat
{
	bc_cascaded_shadow_map_pass::bc_cascaded_shadow_map_pass(std::initializer_list<bcSIZE> p_cascade_sizes)
		: m_cascade_sizes(p_cascade_sizes)
	{
	}

	void bc_cascaded_shadow_map_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
	}

	void bc_cascaded_shadow_map_pass::update(const game::bc_render_pass_update_param& p_param)
	{
	}

	void bc_cascaded_shadow_map_pass::initialize_frame(const game::bc_render_pass_render_param& p_param)
	{
		auto& l_camera = p_param.m_camera;

		game::bc_icamera::extend l_camera_frustum_corners;
		l_camera.get_extend_points(l_camera_frustum_corners);

		core::bc_vector_frame<bcFLOAT> l_cascade_break_points;
		l_cascade_break_points.push_back(l_camera.get_near_clip());
		l_cascade_break_points.assign(std::begin(m_cascade_sizes), std::end(m_cascade_sizes));

		const auto l_lower_left_ray = l_cascade_break_points[4] - l_cascade_break_points[0];
		const auto l_upper_left_ray = l_cascade_break_points[5] - l_cascade_break_points[1];
		const auto l_upper_right_ray = l_cascade_break_points[6] - l_cascade_break_points[2];
		const auto l_lower_right_ray = l_cascade_break_points[7] - l_cascade_break_points[3];
		const auto l_forward_ray = l_camera.get_forward();

		for (bcUINT32 l_cascade_index = 0; l_cascade_index < l_cascade_break_points.size() - 1; ++l_cascade_index)
		{
			const auto l_min_z = l_cascade_break_points[l_cascade_index];
			const auto l_max_z = l_cascade_break_points[l_cascade_index + 1];

			auto l_min_z_ray = l_forward_ray * (l_min_z - l_camera.get_near_clip());
			auto l_max_z_ray = l_forward_ray * (l_max_z - l_camera.get_near_clip());
		}
	}

	void bc_cascaded_shadow_map_pass::execute(const game::bc_render_pass_render_param& p_param)
	{
	}

	void bc_cascaded_shadow_map_pass::before_reset(const game::bc_render_pass_reset_param& p_param)
	{
	}

	void bc_cascaded_shadow_map_pass::after_reset(const game::bc_render_pass_reset_param& p_param)
	{
	}

	void bc_cascaded_shadow_map_pass::destroy(game::bc_render_system& p_render_system)
	{
	}
}