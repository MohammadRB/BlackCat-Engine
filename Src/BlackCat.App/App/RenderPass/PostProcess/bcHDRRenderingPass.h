// [02/10/2023 MRB]

#pragma once

#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/State/bcRenderPassState.h"
#include "Game/System/Render/State/bcRenderState.h"
#include "App/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_hdr_rendering_pass : public game::bci_render_pass
	{
		BC_RENDER_PASS(hdr)

	public:
		bc_hdr_rendering_pass(game::bc_render_pass_variable_t p_hdr_texture, 
			game::bc_render_pass_variable_t p_hdr_texture_read_view, 
			game::bc_render_pass_variable_t p_hdr_texture_render_view,
			game::bc_render_pass_variable_t p_output_texture,
			game::bc_render_pass_variable_t p_output_texture_read_view,
			game::bc_render_pass_variable_t p_output_texture_render_view);

		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_context& p_context) override;

		void initialize_frame(const game::bc_render_pass_render_context& p_context) override;

		void execute(const game::bc_render_pass_render_context& p_context) override;

		void before_reset(const game::bc_render_pass_reset_context& p_context) override;

		void after_reset(const game::bc_render_pass_reset_context& p_context) override;

		void config_changed(const game::bc_render_pass_config_change_context& p_context) override;

		void destroy(game::bc_render_system& p_render_system) override;

	private:
		game::bc_render_pass_variable_t m_hdr_texture_param;
		game::bc_render_pass_variable_t m_hdr_texture_read_view_param;
		game::bc_render_pass_variable_t m_hdr_texture_render_view_param;
		game::bc_render_pass_variable_t m_output_texture_param;
		game::bc_render_pass_variable_t m_output_texture_read_view_param;
		game::bc_render_pass_variable_t m_output_texture_render_view_param;
		bool m_hdr_enabled;

		graphic::bc_sampler_state_ref m_point_sampler;
		graphic::bc_buffer_ref m_cbuffer;
		graphic::bc_texture_config m_luminance_texture_config;
		graphic::bc_texture_config m_adaptive_luminance_texture_config;
		graphic::bc_shader_parameter_link m_luminance_render_target_link;
		graphic::bc_shader_parameter_link m_last_luminance_texture_link;
		graphic::bc_shader_parameter_link m_curr_luminance_texture_link;
		graphic::bc_shader_parameter_link m_adaptive_luminance_render_target_link;
		graphic::bc_shader_parameter_link m_adaptive_luminance_texture_link;
		game::bc_intermediate_texture_guard m_last_luminance_texture;

		graphic::bc_texture2d_ref m_output_texture;
		graphic::bc_resource_view_ref m_output_texture_read_view;
		graphic::bc_render_target_view_ref m_output_texture_render_view;

		graphic::bc_device_pipeline_state_ref m_luminance_device_pipeline_state;
		game::bc_render_pass_state_ptr m_luminance_render_pass_state;
		game::bc_render_state_ptr m_luminance_render_state;

		graphic::bc_device_pipeline_state_ref m_adaptive_luminance_device_pipeline_state;
		game::bc_render_pass_state_ptr m_adaptive_luminance_render_pass_state;
		game::bc_render_state_ptr m_adaptive_luminance_render_state;

		graphic::bc_device_pipeline_state_ref m_tone_mapping_device_pipeline_state;
		game::bc_render_pass_state_ptr m_tone_mapping_render_pass_state;
		game::bc_render_state_ptr m_tone_mapping_render_state;
	};
}