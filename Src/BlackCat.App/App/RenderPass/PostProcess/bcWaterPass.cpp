// [11/12/2023 MRB]

#include "App/AppPCH.h"
#include "App/RenderPass/PostProcess/bcWaterPass.h"

namespace black_cat
{
	bc_water_pass::bc_water_pass(game::bc_render_pass_variable_t p_render_target_texture,
		game::bc_render_pass_variable_t p_render_target_view)
		: m_render_target_texture(p_render_target_texture),
		m_render_target_view(p_render_target_view)
	{
	}

	void bc_water_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
	}

	void bc_water_pass::update(const game::bc_render_pass_update_context& p_context)
	{
	}

	void bc_water_pass::initialize_frame(const game::bc_render_pass_render_context& p_context)
	{
	}

	void bc_water_pass::execute(const game::bc_render_pass_render_context& p_context)
	{
	}

	void bc_water_pass::before_reset(const game::bc_render_pass_reset_context& p_context)
	{
	}

	void bc_water_pass::after_reset(const game::bc_render_pass_reset_context& p_context)
	{
	}

	void bc_water_pass::destroy(game::bc_render_system& p_render_system)
	{
	}
}
