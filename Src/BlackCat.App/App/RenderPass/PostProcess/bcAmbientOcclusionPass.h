// [03/03/2023 MRB]

#pragma once

#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/State/bcRenderPassState.h"
#include "Game/System/Render/State/bcRenderState.h"
#include "App/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_ambient_occlusion_pass : public game::bci_render_pass
	{
		BC_RENDER_PASS(amb_occ)

	public:
		bc_ambient_occlusion_pass(game::bc_render_pass_variable_t p_render_target_texture, game::bc_render_pass_variable_t p_render_target_view) noexcept;

		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_context& p_context) override;

		void initialize_frame(const game::bc_render_pass_render_context& p_context) override;

		void execute(const game::bc_render_pass_render_context& p_context) override;

		void before_reset(const game::bc_render_pass_reset_context& p_context) override;

		void after_reset(const game::bc_render_pass_reset_context& p_context) override;

		void destroy(game::bc_render_system& p_render_system) override;

	private:
		void _update_parameters(game::bc_default_render_thread& p_render_thread);

		game::bc_render_pass_variable_t m_render_target_texture;
		game::bc_render_pass_variable_t m_render_target_view;
		bool m_update_parameters;
		bcUINT32 m_num_samples;
		bcFLOAT m_radius;
		bcFLOAT m_intensity;
		bcFLOAT m_bias;
		bcFLOAT m_cotangent_fov;

		graphic::bc_texture_config m_intermediate_texture_config;
		graphic::bc_shader_parameter_link m_ao_texture_link;

		graphic::bc_sampler_state_ref m_point_sampler;
		graphic::bc_sampler_state_ref m_linear_sampler;
		graphic::bc_buffer_ref m_params_buffer;

		graphic::bc_device_pipeline_state_ref m_ao_device_pipeline_state;
		game::bc_render_pass_state_ptr m_ao_render_pass_state;
		game::bc_render_state_ptr m_ao_render_state;
	};
}
