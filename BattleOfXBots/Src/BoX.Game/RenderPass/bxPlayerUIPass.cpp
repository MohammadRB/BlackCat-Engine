// [12/25/2021 MRB]

#include <iomanip>
#include "Core/Math/bcCurve.h"
#include "Core/Container/bcStringStream.h"
#include "Game/System/Input/bcFileSystem.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/bcGameSystem.h"
#include "BoX.Game/RenderPass/bxPlayerUIPass.h"

namespace box
{
	bx_player_ui_pass::bx_player_ui_pass(game::bc_render_pass_variable_t p_back_buffer_texture, game::bc_render_pass_variable_t p_back_buffer_view, core::bc_estring_view p_font_path) noexcept
		: m_player_service(core::bc_get_service<bx_player_service>()),
		m_back_buffer_texture_parameter(p_back_buffer_texture),
		m_back_buffer_view_parameter(p_back_buffer_view),
		m_font_path(p_font_path)
	{
	}

	void bx_player_ui_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
		auto& l_device = p_render_system.get_device();
		auto& l_device_swap_buffer = p_render_system.get_device_swap_buffer();
		auto& l_game_system = *core::bc_get_service<game::bc_game_system>();

		m_sprite_batch = core::bc_make_unique<graphic::bc_sprite_batch>
		(
			p_render_system.get_device().create_sprite_batch()
		);
		m_sprite_font = core::bc_make_unique<graphic::bc_sprite_font>
		(
			p_render_system.get_device().create_sprite_font(l_game_system.get_file_system().get_content_path(m_font_path.data()))
		);

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

	void bx_player_ui_pass::update(const game::bc_render_pass_update_context& p_context)
	{
	}

	void bx_player_ui_pass::initialize_frame(const game::bc_render_pass_render_context& p_context)
	{
	}

	void bx_player_ui_pass::execute(const game::bc_render_pass_render_context& p_context)
	{
		m_sprite_batch->begin(p_context.m_render_system.get_device(), p_context.m_render_system.get_device_swap_buffer(), m_back_buffer_view);

		switch (m_player_service->get_state())
		{
		case bx_player_state::ghost:
			break;
		case bx_player_state::team_select:
			_draw_team_select_ui();
			break;
		case bx_player_state::playing:
			_draw_player_info_ui();
			break;
		}

		_draw_info_messages();
		_draw_error_messages();
		_draw_kill_messages();
		_draw_time();

		m_sprite_batch->end(p_context.m_render_system.get_device());
	}

	void bx_player_ui_pass::before_reset(const game::bc_render_pass_reset_context& p_context)
	{
	}

	void bx_player_ui_pass::after_reset(const game::bc_render_pass_reset_context& p_context)
	{
		m_back_buffer_texture = *get_shared_resource<graphic::bc_texture2d>(m_back_buffer_texture_parameter);
		m_back_buffer_view = *get_shared_resource<graphic::bc_render_target_view>(m_back_buffer_view_parameter);

		m_screen_center = { static_cast<bcINT32>(m_back_buffer_texture.get_width()) / 2, static_cast<bcINT32>(m_back_buffer_texture.get_height()) / 2 };
		m_screen_size = { static_cast<bcINT32>(m_back_buffer_texture.get_width()), static_cast<bcINT32>(m_back_buffer_texture.get_height()) };
		m_char_bound = m_sprite_font->measure_string(L"T");
		m_team_select_bound = m_sprite_font->measure_string(m_team_select_text);
	}

	void bx_player_ui_pass::destroy(game::bc_render_system& p_render_system)
	{
		m_sprite_batch.reset();
		m_sprite_font.reset();
	}

	void bx_player_ui_pass::_draw_team_select_ui()
	{
		const auto l_position = m_screen_center + core::bc_vector2i(-static_cast<bcINT32>(m_team_select_bound.x) / 2, 0);

		m_sprite_font->draw_string
		(
			*m_sprite_batch,
			m_team_select_text,
			l_position,
			{ 1,1,1 }
		);
	}

	void bx_player_ui_pass::_draw_player_info_ui()
	{
		constexpr bcINT32 l_left_offset = 20;
		constexpr bcINT32 l_bottom_offset = 100;
		constexpr bcFLOAT l_scale = 1.3f;
		const bcINT32 l_top_offset = static_cast<bcINT32>(m_back_buffer_texture.get_height()) - l_bottom_offset;
		const bcINT32 l_line_offset = static_cast<bcINT32>(m_char_bound.y) + 5;
		const core::bc_curve3f l_color_curve({ { 1.f, .3f, .3f }, { 1.f, 1.f, 1.f } });

		const auto l_heath = m_player_service->get_health();
		const auto l_weapon_heat = m_player_service->get_weapon_heat();
		const auto l_grenade_load = m_player_service->get_grenade_load();
		const auto l_smoke_load = m_player_service->get_smoke_load();

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
	}

	void bx_player_ui_pass::_draw_time()
	{
		constexpr auto l_top_offset = 20;
		const auto l_position = core::bc_vector2i(m_screen_center.x - m_char_bound.x * 2, l_top_offset);
		const auto l_time = m_player_service->get_game_time();
		const auto l_time_str = (core::bc_wstring_stream_frame() << std::setfill(L'0') << std::setw(2) << l_time / 60 << ":" << std::setw(2) << l_time % 60).str();

		m_sprite_font->draw_string
		(
			*m_sprite_batch,
			l_time_str.c_str(),
			l_position,
			{ 1,1,1 }
		);
	}

	void bx_player_ui_pass::_draw_info_messages()
	{
		const auto l_info_messages = m_player_service->get_info_messages();
		const auto l_position = m_screen_size + core::bc_vector2i(-10, -100);

		_draw_messages(l_position, bx_text_align::left, { 1.f, 1.f, 1.f }, core::bc_make_cspan(l_info_messages));
	}

	void bx_player_ui_pass::_draw_error_messages()
	{
		const auto l_error_messages = m_player_service->get_error_messages();
		const auto l_position = m_screen_center + core::bc_vector2i(0, 50);
		
		_draw_messages(l_position, bx_text_align::center, m_red_color, core::bc_make_cspan(l_error_messages));
	}

	void bx_player_ui_pass::_draw_kill_messages()
	{
		const auto l_kill_messages = m_player_service->get_kill_messages();
		const auto l_position = core::bc_vector2i(m_screen_size.x, 0) + core::bc_vector2i(-10, 10);
		const auto l_scale = 1.1f;
		auto l_top_offset = l_position.y;

		for (auto& l_kill : l_kill_messages)
		{
			const auto l_killer_name = core::bc_to_wstring_frame(l_kill.m_killer_name);
			const auto l_killed_name = core::bc_to_wstring_frame(l_kill.m_killed_name);
			const auto l_killer_bound = m_sprite_font->measure_string(l_killer_name.c_str()) * l_scale;
			const auto l_killed_bound = m_sprite_font->measure_string(l_killed_name.c_str()) * l_scale;
			const auto l_kill_bound = m_sprite_font->measure_string(L"killed");
			auto l_left_offset = l_position.x - static_cast<bcINT32>(l_killer_bound.x + l_killed_bound.x + l_kill_bound.x);

			m_sprite_font->draw_string
			(
				*m_sprite_batch,
				l_killer_name.c_str(),
				{ l_left_offset, l_top_offset },
				l_kill.m_killer_team == bx_team::red ? m_red_color : m_blue_color,
				l_scale
			);

			l_left_offset += l_killer_bound.x + 2;

			m_sprite_font->draw_string
			(
				*m_sprite_batch,
				L"killed",
				{ l_left_offset, l_top_offset }
			);

			l_left_offset += l_kill_bound.x + 2;

			m_sprite_font->draw_string
			(
				*m_sprite_batch,
				l_killed_name.c_str(),
				{ l_left_offset, l_top_offset },
				l_kill.m_killed_team == bx_team::red ? m_red_color : m_blue_color,
				l_scale
			);

			l_top_offset += static_cast<bcINT32>(l_killer_bound.y) + 5;
		}
	}

	void bx_player_ui_pass::_draw_messages(const core::bc_vector2i& p_position, bx_text_align p_align, const core::bc_vector3f& p_color, core::bc_const_span<core::bc_wstring> p_messages)
	{
		auto l_top_offset = 0U;

		for(auto& l_msg : p_messages)
		{
			const auto l_msg_bound = m_sprite_font->measure_string(l_msg.c_str());
			auto l_position = p_position + core::bc_vector2i(0, l_top_offset);

			switch (p_align)
			{
			case bx_text_align::left:
				l_position.x -= static_cast<bcINT32>(l_msg_bound.x);
				l_position.y -= static_cast<bcINT32>(l_msg_bound.y) / 2;
				break;
			case bx_text_align::center:
				l_position.x -= static_cast<bcINT32>(l_msg_bound.x) / 2;
				l_position.y -= static_cast<bcINT32>(l_msg_bound.y) / 2;
				break;
			case bx_text_align::right:
				l_position.y -= static_cast<bcINT32>(l_msg_bound.y) / 2;
				break;
			}

			m_sprite_font->draw_string
			(
				*m_sprite_batch,
				l_msg.c_str(),
				l_position,
				p_color
			);

			l_top_offset += l_msg_bound.y + 5;
		}
	}
}
