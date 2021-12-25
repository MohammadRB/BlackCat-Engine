// [12/25/2021 MRB]

#pragma once

#include "Game/System/Render/Pass/bcRenderPass.h"

namespace box
{
	using namespace black_cat;

	class bc_player_ui_pass : public game::bci_render_pass
	{
		BC_RENDER_PASS(ply_ui)

	public:
		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_context& p_context) override;

		void initialize_frame(const game::bc_render_pass_render_context& p_context) override;

		void execute(const game::bc_render_pass_render_context& p_context) override;

		void before_reset(const game::bc_render_pass_reset_context& p_context) override;

		void after_reset(const game::bc_render_pass_reset_context& p_context) override;

		void destroy(game::bc_render_system& p_render_system) override;

	private:
	};
}