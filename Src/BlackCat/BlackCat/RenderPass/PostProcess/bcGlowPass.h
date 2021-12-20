// [12/17/2021 MRB]

#pragma once

#include "GraphicImp/Resource/View/bcResourceView.h"
#include "GraphicImp/Resource/Texture/bcTextureConfig.h"
#include "GraphicImp/Resource/State/bcSamplerState.h"
#include "Core/Messaging/bcMessageHandle.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/State/bcRenderPassState.h"
#include "Game/System/Render/State/bcRenderState.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
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

		void destroy(game::bc_render_system& p_render_system) override;

		bool get_enable() const noexcept;

		void set_enable(bool p_value) noexcept;

		void update_parameters(bcFLOAT p_glow_threshold, bcFLOAT p_glow_intensity);

	private:
		void _update_parameters(game::bc_default_render_thread& p_render_thread);

		bool m_enabled = true;
		bool m_parameters_changed = false;
		game::bc_render_pass_variable_t m_render_target_texture;
		game::bc_render_pass_variable_t m_render_target_view;

		core::bc_event_listener_handle m_config_change_handle;
		bcFLOAT m_glow_threshold = 0;
		bcFLOAT m_glow_intensity = 0;

		graphic::bc_texture_config m_intermediate_texture_config;
		graphic::bc_sampler_state_ref m_point_sampler;
		graphic::bc_sampler_state_ref m_linear_sampler;
		graphic::bc_buffer_ref m_glow_params_buffer;
		graphic::bc_buffer_ref m_blur_params_buffer;
		graphic::bc_resource_view_ref m_render_target_resource_view;
		graphic::bc_shader_parameter_link m_intermediate_texture1_link;
		graphic::bc_shader_parameter_link m_intermediate_texture2_link;

		graphic::bc_device_pipeline_state_ref m_glow_extract_device_pipeline_state;
		game::bc_render_pass_state_ptr m_glow_extract_render_pass_state;
		game::bc_render_state_ptr m_glow_extract_render_state;
		graphic::bc_device_pipeline_state_ref m_blur_device_pipeline_state;
		game::bc_render_pass_state_ptr m_blur_render_pass_state;
		game::bc_render_state_ptr m_blur_render_state;
		graphic::bc_device_pipeline_state_ref m_glow_apply_device_pipeline_state;
		game::bc_render_pass_state_ptr m_glow_apply_render_pass_state;
		game::bc_render_state_ptr m_glow_apply_render_state;
	};

	inline void bc_glow_pass::set_enable(bool p_value) noexcept
	{
		m_enabled = p_value;
	}

	inline bool bc_glow_pass::get_enable() const noexcept
	{
		return m_enabled;
	}
}