// [25/12/2021 MRB]

#pragma once

#include "Core/Container/bcSpan.h"
#include "Core/Math/bcVector2f.h"
#include "Core/Math/bcVector3f.h"
#include "GraphicImp/Font/bcSpriteBatch.h"
#include "GraphicImp/Font/bcSpriteFont.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "BoX.Game/Application/bxPlayerService.h"
#include "BoX.Game/Application/bxGameState.h"
#include "BoX.Game/bxExport.h"

namespace box
{
	using namespace black_cat;

	enum class bx_text_align
	{
		left, center, right
	};

	class BX_GAME_DLL bx_player_ui_pass : public game::bci_render_pass
	{
		BC_RENDER_PASS(ply_ui)

	public:
		bx_player_ui_pass(game::bc_render_pass_variable_t p_render_target_texture, 
			game::bc_render_pass_variable_t p_render_target_view, 
			core::bc_estring_view p_font_path) noexcept;

		bx_player_ui_pass(bx_player_ui_pass&&) noexcept = default;

		~bx_player_ui_pass() override = default;

		bx_player_ui_pass& operator=(bx_player_ui_pass&&) noexcept = default;

		void initialize_resources(game::bc_render_system& p_render_system) override;

		void update(const game::bc_render_pass_update_context& p_context) override;

		void initialize_frame(const game::bc_render_pass_render_context& p_context) override;

		void execute(const game::bc_render_pass_render_context& p_context) override;

		void before_reset(const game::bc_render_pass_reset_context& p_context) override;

		void after_reset(const game::bc_render_pass_reset_context& p_context) override;

		void destroy(game::bc_render_system& p_render_system) override;

	private:
		void _draw_team_select_ui();

		void _draw_player_info_ui();

		void _draw_time();

		void _draw_info_messages();

		void _draw_error_messages();

		void _draw_kill_messages();

		void _draw_scores();

		void _draw_messages(const core::bc_vector2i& p_position, bx_text_align p_align, const core::bc_vector3f& p_color, core::bc_const_span<core::bc_wstring> p_messages);

		game::bc_render_pass_variable_t m_render_target_texture_param;
		game::bc_render_pass_variable_t m_render_target_view_param;
		const core::bc_vector3f m_red_color{ 1.f, .65f, .65f };
		const core::bc_vector3f m_blue_color{ .65f, .65f, 1.f };
		const bcWCHAR* m_team_select_text = L"Select Your Team. Press R to join Red team or B to join Blue team.";

		bx_player_service* m_player_service;
		core::bc_estring_view m_font_path;
		graphic::bc_texture2d m_render_target_texture;
		graphic::bc_render_target_view m_render_target_view;

		core::bc_unique_ptr<graphic::bc_sprite_batch> m_sprite_batch;
		core::bc_unique_ptr<graphic::bc_sprite_font> m_sprite_font;
		core::bc_vector2i m_screen_center;
		core::bc_vector2i m_screen_size;
		core::bc_vector2f m_char_bound;
		core::bc_vector2f m_team_select_bound;

		graphic::bc_texture2d_content_ptr m_score_background_texture;
		graphic::bc_resource_view_ref m_score_background_view;
	};
}
