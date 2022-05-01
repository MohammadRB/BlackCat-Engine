// [11/09/2020 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcArray.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcString.h"
#include "Core/Math/bcVector2f.h"
#include "Core/bcConstant.h"
#include "GraphicImp/Font/bcSpriteBatch.h"
#include "GraphicImp/Font/bcSpriteFont.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "App/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_counter_value_draw_pass : public game::bci_render_pass
	{
		BC_RENDER_PASS(txt_drw)

	public:
		bc_counter_value_draw_pass(game::bc_render_pass_variable_t p_render_target_texture_param, 
			game::bc_render_pass_variable_t p_render_target_view_param, 
			core::bc_estring_view p_font_path);
		
		void initialize_resources(game::bc_render_system& p_render_system) override;
		
		void update(const game::bc_render_pass_update_context& p_context) override;
		
		void initialize_frame(const game::bc_render_pass_render_context& p_context) override;
		
		void execute(const game::bc_render_pass_render_context& p_context) override;
		
		void before_reset(const game::bc_render_pass_reset_context& p_context) override;
		
		void after_reset(const game::bc_render_pass_reset_context& p_context) override;
		
		void destroy(game::bc_render_system& p_render_system) override;

	private:
		game::bc_render_pass_variable_t m_render_target_texture_param;
		game::bc_render_pass_variable_t m_render_target_view_param;
		core::bc_estring_view m_font_path;

		core::bc_array<core::bc_vector<core::bc_string>, 2> m_counter_values;
		bcUINT32 m_counter_values_read_index;
		bcFLOAT m_counter_values_update_interval;

		graphic::bc_texture2d m_render_target_texture;
		graphic::bc_render_target_view m_render_target_render_view;
		core::bc_unique_ptr<graphic::bc_sprite_batch> m_sprite_batch;
		core::bc_unique_ptr<graphic::bc_sprite_font> m_sprite_font;
		core::bc_vector2f m_char_bound;
	};
}