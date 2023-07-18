// [28/06/2019 MRB]

#pragma once

#include "Game/bcConstant.h"

namespace black_cat::constant
{
	BC_RENDER_PASS_VARIABLE(depth_stencil_texture);
	BC_RENDER_PASS_VARIABLE(depth_stencil_render_view);
	BC_RENDER_PASS_VARIABLE(depth_stencil_read_view);
	BC_RENDER_PASS_VARIABLE(back_buffer_texture);
	BC_RENDER_PASS_VARIABLE(back_buffer_render_view);
	BC_RENDER_PASS_VARIABLE(render_target_texture_1);
	BC_RENDER_PASS_VARIABLE(render_target_render_view_1);
	BC_RENDER_PASS_VARIABLE(render_target_read_view_1);
	BC_RENDER_PASS_VARIABLE(render_target_texture_2);
	BC_RENDER_PASS_VARIABLE(render_target_render_view_2);
	BC_RENDER_PASS_VARIABLE(render_target_read_view_2);
	BC_RENDER_PASS_VARIABLE(render_target_texture_3);
	BC_RENDER_PASS_VARIABLE(render_target_render_view_3);
	BC_RENDER_PASS_VARIABLE(render_target_read_view_3);
	BC_RENDER_PASS_VARIABLE(render_target_texture_4);
	BC_RENDER_PASS_VARIABLE(render_target_render_view_4);
	BC_RENDER_PASS_VARIABLE(render_target_read_view_4);
	BC_RENDER_PASS_VARIABLE(direct_light_depth_buffers);
	BC_RENDER_PASS_VARIABLE(ambient_occlusion_texture);
	BC_RENDER_PASS_VARIABLE(ambient_occlusion_read_view);
	BC_RENDER_PASS_VARIABLE(gbuffer_texture);
	BC_RENDER_PASS_VARIABLE(gbuffer_read_view);
	BC_RENDER_PASS_VARIABLE(gbuffer_render_view);
	BC_RENDER_PASS_VARIABLE(hdr_output_texture);
	BC_RENDER_PASS_VARIABLE(hdr_output_read_view);
	BC_RENDER_PASS_VARIABLE(hdr_output_render_view);

	BC_CONFIG_VARIABLE(render_hdr_enabled);
	BC_CONFIG_VARIABLE(render_hdr_reference_intensity);
}