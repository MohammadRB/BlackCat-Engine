// [12/25/2021 MRB]

#include "Core/Math/bcCurve.h"
#include "Core/Container/bcArray.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "BoX/RenderPass/bxPlayerUIPass.h"

namespace box
{
	bc_player_ui_pass::bc_player_ui_pass(game::bc_render_pass_variable_t p_back_buffer_texture, game::bc_render_pass_variable_t p_back_buffer_view, core::bc_estring_view p_font_path) noexcept
		: m_ui_service(core::bc_get_service<bx_player_service>()),
		m_back_buffer_texture_parameter(p_back_buffer_texture),
		m_back_buffer_view_parameter(p_back_buffer_view),
		m_font_path(p_font_path)
	{
	}

	void bc_player_ui_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
		auto& l_device = p_render_system.get_device();
		auto& l_device_swap_buffer = p_render_system.get_device_swap_buffer();

		m_sprite_batch = core::bc_make_unique<graphic::bc_sprite_batch>
		(
			p_render_system.get_device().create_sprite_batch()
		);
		m_sprite_font = core::bc_make_unique<graphic::bc_sprite_font>
		(
			p_render_system.get_device().create_sprite_font(m_font_path.data())
		);
		m_text_bound = m_sprite_font->measure_string(L"T");

		after_reset(game::bc_render_pass_reset_context
		(
			p_render_system,
			l_device,
			l_device_swap_buffer,
			graphic::bc_device_parameters
			(
				0,
				0,
				graphic::bc_format::unknown,
				graphic::bc_texture_ms_config(1, 0)
			),
			graphic::bc_device_parameters
			(
				l_device_swap_buffer.get_back_buffer_width(),
				l_device_swap_buffer.get_back_buffer_height(),
				l_device_swap_buffer.get_back_buffer_format(),
				l_device_swap_buffer.get_back_buffer_texture().get_sample_count()
			)
		));
	}

	void bc_player_ui_pass::update(const game::bc_render_pass_update_context& p_context)
	{
	}

	void bc_player_ui_pass::initialize_frame(const game::bc_render_pass_render_context& p_context)
	{
	}

	void bc_player_ui_pass::execute(const game::bc_render_pass_render_context& p_context)
	{
		constexpr bcINT32 l_left_offset = 20;
		constexpr bcINT32 l_bottom_offset = 100;
		constexpr bcFLOAT l_scale = 1.3f;
		const bcINT32 l_top_offset = m_back_buffer_texture.get_height() - l_bottom_offset;
		const bcINT32 l_line_offset = m_text_bound.y + 5.f;
		const core::bc_curve3f l_color_curve({ { 1.f, .3f, .3f }, { 1.f, 1.f, 1.f } });

		const auto l_heath = m_ui_service->get_health();
		const auto l_weapon_heat = m_ui_service->get_weapon_heat();
		const auto l_grenade_load = m_ui_service->get_grenade_load();
		const auto l_smoke_load = m_ui_service->get_smoke_load();

		m_sprite_batch->begin(p_context.m_render_system.get_device(), p_context.m_render_system.get_device_swap_buffer(), m_back_buffer_view);

		m_sprite_font->draw_string
		(
			*m_sprite_batch, 
			(L"Health: " + core::bc_to_wstring(l_heath)).c_str(), 
			{ l_left_offset, l_top_offset }, 
			l_color_curve.sample(l_heath / 100.f), 
			l_scale
		);
		m_sprite_font->draw_string
		(
			*m_sprite_batch,
			(L"Weapon Heat: " + core::bc_to_wstring(l_weapon_heat)).c_str(),
			{ l_left_offset, l_top_offset + l_line_offset * 1 },
			l_color_curve.sample(1 - (l_weapon_heat / 100.f)),
			l_scale
		);
		m_sprite_font->draw_string
		(
			*m_sprite_batch,
			(L"Grenade Load: " + core::bc_to_wstring(l_grenade_load)).c_str(),
			{ l_left_offset, l_top_offset + l_line_offset * 2 },
			l_color_curve.sample(l_grenade_load / 100.f),
			l_scale
		);
		m_sprite_font->draw_string
		(
			*m_sprite_batch,
			(L"Smoke Load: " + core::bc_to_wstring(l_smoke_load)).c_str(),
			{ l_left_offset, l_top_offset + l_line_offset * 3 },
			l_color_curve.sample(l_smoke_load / 100.f),
			l_scale
		);

		m_sprite_batch->end(p_context.m_render_system.get_device());
	}

	void bc_player_ui_pass::before_reset(const game::bc_render_pass_reset_context& p_context)
	{
	}

	void bc_player_ui_pass::after_reset(const game::bc_render_pass_reset_context& p_context)
	{
		m_back_buffer_texture = *get_shared_resource<graphic::bc_texture2d>(m_back_buffer_texture_parameter);
		m_back_buffer_view = *get_shared_resource<graphic::bc_render_target_view>(m_back_buffer_view_parameter);
	}

	void bc_player_ui_pass::destroy(game::bc_render_system& p_render_system)
	{
		m_sprite_batch.reset();
		m_sprite_font.reset();
	}
}