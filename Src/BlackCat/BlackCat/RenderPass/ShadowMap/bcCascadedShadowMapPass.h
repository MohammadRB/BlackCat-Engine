// [06/07/2019 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Input/bcCameraFrustum.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_cascaded_shadow_map_pass : game::bc_irender_pass
	{
	public:
		bc_cascaded_shadow_map_pass(std::initializer_list<bcSIZE> p_cascade_sizes);

		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_param& p_param) override;

		void initialize_frame(const game::bc_render_pass_render_param& p_param) override;

		void execute(const game::bc_render_pass_render_param& p_param) override;

		void before_reset(const game::bc_render_pass_reset_param& p_param) override;

		void after_reset(const game::bc_render_pass_reset_param& p_param) override;

		void destroy(game::bc_render_system& p_render_system) override;

	private:
		core::bc_vector_program<bcSIZE> m_cascade_sizes;
		core::bc_vector_frame<game::bc_camera_frustum> m_cascade_frustums;
	};
}