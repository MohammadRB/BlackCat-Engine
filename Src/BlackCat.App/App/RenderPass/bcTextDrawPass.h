// [11/09/2020 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Core/Math/bcVector2f.h"
#include "Core/bcConstant.h"
#include "GraphicImp/Font/bcSpriteBatch.h"
#include "GraphicImp/Font/bcSpriteFont.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "App/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_text_draw_pass : public game::bci_render_pass
	{
		BC_RENDER_PASS(txt_drw)

	public:
		bc_text_draw_pass(game::bc_render_pass_variable_t p_back_buffer_view_parameter, core::bc_estring_view p_font_path);
		
		void initialize_resources(game::bc_render_system& p_render_system) override;
		
		void update(const game::bc_render_pass_update_context& p_context) override;
		
		void initialize_frame(const game::bc_render_pass_render_context& p_context) override;
		
		void execute(const game::bc_render_pass_render_context& p_context) override;
		
		void before_reset(const game::bc_render_pass_reset_context& p_context) override;
		
		void after_reset(const game::bc_render_pass_reset_context& p_context) override;
		
		void destroy(game::bc_render_system& p_render_system) override;

	private:
		game::bc_render_pass_variable_t m_back_buffer_view_parameter;
		core::bc_estring_view m_font_path;

		graphic::bc_render_target_view m_back_buffer_view;
		core::bc_unique_ptr<graphic::bc_sprite_batch> m_sprite_batch;
		core::bc_unique_ptr<graphic::bc_sprite_font> m_sprite_font;
		core::bc_vector2f m_text_bound;
	};
}