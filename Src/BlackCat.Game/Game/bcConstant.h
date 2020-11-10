// [06/28/2019 MRB]

#pragma once

#include "Core/bcConstant.h"

namespace black_cat
{
	namespace constant
	{
		BC_RENDER_PASS_VARIABLE(depth_stencil_texture);
		BC_RENDER_PASS_VARIABLE(depth_stencil_view);
		BC_RENDER_PASS_VARIABLE(back_buffer_view);
		BC_RENDER_PASS_VARIABLE(render_target_texture_1);
		BC_RENDER_PASS_VARIABLE(render_target_view_1);
		BC_RENDER_PASS_VARIABLE(render_target_texture_2);
		BC_RENDER_PASS_VARIABLE(render_target_view_2);
		BC_RENDER_PASS_VARIABLE(render_target_texture_3);
		BC_RENDER_PASS_VARIABLE(render_target_view_3);
		BC_RENDER_PASS_VARIABLE(render_target_texture_4);
		BC_RENDER_PASS_VARIABLE(render_target_view_4);

		BC_PARAMETER(shader_function, "function");
		BC_PARAMETER(mesh, "mesh");
		BC_PARAMETER(sub_mesh, "sub_mesh");
		BC_PARAMETER(child_actor, "childs");
		BC_PARAMETER(heightmap, "heightmap");
		BC_PARAMETER(texture_config, "config");
	}
}