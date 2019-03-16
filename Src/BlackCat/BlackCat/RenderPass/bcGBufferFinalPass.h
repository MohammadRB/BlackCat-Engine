// [03/15/2019 MRB]

#pragma once

#include "BlackCat/bcExport.h"
#include "Game/System/Render/Pass/bcRenderPass.h"

namespace black_cat
{
	class BC_BLACKCAT_DLL bc_gbuffer_final_pass : public game::bc_irender_pass
	{
	public:
		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_param& p_param) override;

		void initialize_frame(const game::bc_render_pass_render_param& p_param) override;

		void execute(const game::bc_render_pass_render_param& p_param) override;

		void before_reset(const game::bc_render_pass_reset_param& p_param) override;

		void after_reset(const game::bc_render_pass_reset_param& p_param) override;

		void destroy(game::bc_render_system& p_render_system) override;

	private:

	};
}