// [12/25/2021 MRB]

#pragma once

#include "Core/Math/bcVector2f.h"
#include "GraphicImp/Device/bcDeviceTextRenderer.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "BoX/Application/bxPlayerService.h"

namespace box
{
	using namespace black_cat;

	class bc_player_ui_pass : public game::bci_render_pass
	{
		BC_RENDER_PASS(ply_ui)

	public:
		explicit bc_player_ui_pass(game::bc_render_pass_variable_t p_back_buffer_texture, game::bc_render_pass_variable_t p_back_buffer_view) noexcept;

		bc_player_ui_pass(bc_player_ui_pass&&) noexcept = default;

		~bc_player_ui_pass() override = default;

		bc_player_ui_pass& operator=(bc_player_ui_pass&&) noexcept = default;

		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_context& p_context) override;

		void initialize_frame(const game::bc_render_pass_render_context& p_context) override;

		void execute(const game::bc_render_pass_render_context& p_context) override;

		void before_reset(const game::bc_render_pass_reset_context& p_context) override;

		void after_reset(const game::bc_render_pass_reset_context& p_context) override;

		void destroy(game::bc_render_system& p_render_system) override;

	private:
		bx_player_service* m_ui_service;

		game::bc_render_pass_variable_t m_back_buffer_texture_parameter;
		game::bc_render_pass_variable_t m_back_buffer_view_parameter;
		graphic::bc_texture2d m_back_buffer_texture;
		graphic::bc_render_target_view m_back_buffer_view;

		core::bc_unique_ptr<graphic::bc_device_text_renderer> m_text_renderer;
		core::bc_vector2f m_text_bound;
	};
}