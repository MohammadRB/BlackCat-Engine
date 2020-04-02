// [10/04/2019 MRB]

#pragma once

#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Resource/State/bcSamplerState.h"
#include "Game/System/Render/Pass/bcRenderPassState.h"
#include "BlackCat/RenderPass/ShadowMap/bcBaseCascadedShadowMapPass.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_vegetable_cascaded_shadow_map_pass final : public bc_base_cascaded_shadow_map_pass
	{
		BC_RENDER_PASS(veg_csm)
		
	public:
		bc_vegetable_cascaded_shadow_map_pass(const bc_base_cascaded_shadow_map_pass& p_other_instance);

		bc_vegetable_cascaded_shadow_map_pass(constant::bc_render_pass_variable_t p_output_depth_buffers,
			bcSIZE p_shadow_map_size,
			const std::initializer_list< std::tuple< bcSIZE, bcUBYTE > >& p_cascade_sizes);

	private:
		void initialize_pass(game::bc_render_system& p_render_system) override;
		
		void execute_pass(const bc_cascaded_shadow_map_pass_render_param& p_param) override;
		
		void destroy_pass(game::bc_render_system& p_render_system) override;
		
		core::bc_vector<game::bc_render_pass_state_ptr> create_render_pass_states(game::bc_render_system& p_render_system, 
			graphic::bc_texture2d p_depth,
			const core::bc_vector<graphic::bc_depth_stencil_view_ptr>& p_depth_views) override;
		
		graphic::bc_device_pipeline_state_ptr m_leaf_pipeline_state;
		graphic::bc_device_pipeline_state_ptr m_trunk_pipeline_state;
		graphic::bc_sampler_state_ptr m_sampler_state;
	};
}