// [01/03/2022 MRB]

#pragma once

#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "Core/Messaging/Query/bcQueryResult.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/State/bcSamplerState.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/State/bcRenderPassState.h"
#include "Game/System/Render/State/bcRenderState.h"
#include "Game/Query/bcSceneQuery.h"
#include "BoX.Game/bxExport.h"

namespace box
{
	using namespace black_cat;

	struct _bx_bullet
	{
		core::bc_vector3f m_position;
		core::bc_vector3f m_direction;
	};

	class BX_GAME_DLL bx_bullet_trail_pass : public game::bci_render_pass
	{
		BC_RENDER_PASS(blt_trl)

	public:
		bx_bullet_trail_pass(game::bc_render_pass_variable_t p_render_target_texture, 
			game::bc_render_pass_variable_t p_render_target_view, 
			core::bc_estring_view p_texture_path);

		bx_bullet_trail_pass(bx_bullet_trail_pass&&) noexcept = default;

		~bx_bullet_trail_pass() override = default;

		bx_bullet_trail_pass& operator=(bx_bullet_trail_pass&&) noexcept = default;

		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_context& p_context) override;

		void initialize_frame(const game::bc_render_pass_render_context& p_context) override;

		void execute(const game::bc_render_pass_render_context& p_context) override;

		void before_reset(const game::bc_render_pass_reset_context& p_context) override;

		void after_reset(const game::bc_render_pass_reset_context& p_context) override;

		void config_changed(const game::bc_render_pass_config_change_context& p_context) override;

		void destroy(game::bc_render_system& p_render_system) override;

	private:
		void _update_bullets_buffer(const game::bc_render_pass_render_context& p_context);

		void _update_parameters_buffer(const game::bc_render_pass_render_context& p_context);

		game::bc_render_pass_variable_t m_render_target_texture;
		game::bc_render_pass_variable_t m_render_target_view;
		core::bc_estring_view m_texture_path;
		bool m_hdr_enabled;
		bcFLOAT m_hdr_intensity;
		bool m_parameters_changed;

		graphic::bc_sampler_state_ref m_linear_sampler;
		graphic::bc_texture2d_content_ptr m_texture;
		graphic::bc_resource_view_ref m_texture_view;
		graphic::bc_buffer_ref m_cbuffer;
		graphic::bc_buffer_ref m_bullets_buffer;
		graphic::bc_resource_view_ref m_bullets_buffer_view;
		graphic::bc_shader_parameter_link  m_bullets_parameter_link;
		graphic::bc_device_pipeline_state_ref m_device_pipeline_state;
		game::bc_render_pass_state_ptr m_render_pass_state;

		core::bc_query_result_t<game::bc_scene_query> m_query_result;
		core::bc_vector<_bx_bullet> m_bullets;
	};
}
