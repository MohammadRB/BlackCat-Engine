// [17/12/2021 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "GraphicImp/Resource/Texture/bcTextureConfig.h"
#include "GraphicImp/Resource/State/bcSamplerState.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/State/bcRenderPassState.h"
#include "Game/System/Render/State/bcRenderState.h"
#include "App/bcExport.h"

namespace black_cat
{
	struct _bc_blur_step_data
	{
		bcUINT32 m_size;
		graphic::bc_shader_parameter_link m_intermediate_texture_link;
		graphic::bc_shader_parameter_link m_result_texture_link;

		graphic::bc_device_pipeline_state_ref m_hor_blur_device_pipeline_state;
		game::bc_render_pass_state_ptr m_hor_blur_render_pass_state;
		game::bc_render_state_ptr m_hor_blur_render_state;
		graphic::bc_device_pipeline_state_ref m_ver_blur_device_pipeline_state;
		game::bc_render_pass_state_ptr m_ver_blur_render_pass_state;
		game::bc_render_state_ptr m_ver_blur_render_state;
	};

	class BC_DLL bc_glow_pass : public game::bci_render_pass
	{
		BC_RENDER_PASS(glow)

	public:
		bc_glow_pass(game::bc_render_pass_variable_t p_render_target_texture, game::bc_render_pass_variable_t p_render_target_view);

		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_context& p_context) override;

		void initialize_frame(const game::bc_render_pass_render_context& p_context) override;

		void execute(const game::bc_render_pass_render_context& p_context) override;

		void before_reset(const game::bc_render_pass_reset_context& p_context) override;

		void after_reset(const game::bc_render_pass_reset_context& p_context) override;

		void config_changed(const game::bc_render_pass_config_change_context& p_context) override;

		void destroy(game::bc_render_system& p_render_system) override;
		
	private:
		_bc_blur_step_data _create_blur_step_data(const game::bc_render_pass_reset_context& p_context, 
			graphic::bc_shader_parameter_link p_glow_texture, 
			bcUINT32 p_step_size, 
			bcUINT32 p_kernel_size);

		void _update_parameters(game::bc_default_render_thread& p_render_thread);

		game::bc_render_pass_variable_t m_render_target_texture_param;
		game::bc_render_pass_variable_t m_render_target_view_param;
		bcUINT32 m_back_buffer_width;
		bcUINT32 m_back_buffer_height;
		bcFLOAT m_glow_threshold;
		bcFLOAT m_glow_intensity;
		bool m_parameters_changed;
		
		graphic::bc_sampler_state_ref m_point_sampler;
		graphic::bc_sampler_state_ref m_linear_sampler;
		graphic::bc_buffer_ref m_glow_params_buffer;
		graphic::bc_buffer_ref m_blur_params_buffer;
		graphic::bc_resource_view_ref m_render_target_resource_view;
		graphic::bc_shader_parameter_link m_glow_texture_link;
		graphic::bc_shader_parameter_link m_upscale_texture_view_link;
		graphic::bc_shader_parameter_link m_upscale_render_view_link;
		graphic::bc_shader_parameter_link m_apply_texture_view_link;
		graphic::bc_texture_config m_glow_intermediate_texture_config;
		graphic::bc_texture_config m_upscale_intermediate_texture_config;

		graphic::bc_device_pipeline_state_ref m_glow_extract_device_pipeline_state;
		game::bc_render_pass_state_ptr m_glow_extract_render_pass_state;
		game::bc_render_state_ptr m_glow_extract_render_state;

		graphic::bc_device_pipeline_state_ref m_upscale_device_pipeline_state;
		game::bc_render_pass_state_ptr m_upscale_render_pass_state;
		game::bc_render_state_ptr m_upscale_render_state;
		
		graphic::bc_device_pipeline_state_ref m_glow_apply_device_pipeline_state;
		game::bc_render_pass_state_ptr m_glow_apply_render_pass_state;
		game::bc_render_state_ptr m_glow_apply_render_state;

		core::bc_vector<_bc_blur_step_data> m_blur_steps;
	};
}