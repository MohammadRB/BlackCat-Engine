// [11/09/2020 MRB]

#include "App/AppPCH.h"

#include "Core/Container/bcVector.h"
#include "Core/Utility/bcCounterValueManager.h"
#include "Game/System/Input/bcFileSystem.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/bcGameSystem.h"
#include "App/RenderPass/bcCounterValueDrawPass.h"

namespace black_cat
{
	bc_counter_value_draw_pass::bc_counter_value_draw_pass(game::bc_render_pass_variable_t p_back_buffer_view_parameter, core::bc_estring_view p_font_path)
		: m_back_buffer_view_parameter(p_back_buffer_view_parameter),
		m_font_path(p_font_path),
		m_counter_values_read_index(0),
		m_counter_values_update_interval(1000)
	{
	}

	void bc_counter_value_draw_pass::initialize_resources(game::bc_render_system& p_render_system)
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
		m_text_bound = m_sprite_font->measure_string(L"Test");

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

	void bc_counter_value_draw_pass::update(const game::bc_render_pass_update_context& p_context)
	{
		m_counter_values_update_interval += p_context.m_clock.m_elapsed;

		if(m_counter_values_update_interval >= 1000)
		{
			const auto& l_json_counter_values = bc_get_global_config().get_counter_values();
			const auto l_write_index = (m_counter_values_read_index + 1) % 2;
			auto& l_counter_values = m_counter_values[l_write_index];
			l_counter_values.clear();

			std::transform
			(
				std::begin(l_json_counter_values),
				std::end(l_json_counter_values),
				std::back_inserter(l_counter_values),
				[](const core::bc_json_value<core::bc_string>& l_json_entry)
				{
					return l_json_entry.get();
				}
			);

			m_counter_values_update_interval = 0;
			m_counter_values_read_index = l_write_index;
		}
	}

	void bc_counter_value_draw_pass::initialize_frame(const game::bc_render_pass_render_context& p_context)
	{
	}

	void bc_counter_value_draw_pass::execute(const game::bc_render_pass_render_context& p_context)
	{
		auto& l_counter_value_manager = *core::bc_get_service<core::bc_counter_value_manager>();
		const auto l_text_position_calculator = [this](bcUINT32 p_row)
		{
			return core::bc_vector2i(10, 10 + p_row * (m_text_bound.y + 2));
		};

		bcUINT32 l_counter_ite = 0;
		std::mbstate_t l_to_wstring_state{0, 0, 0};
		bcWCHAR l_to_wstring_buffer[300];
		core::bc_wstring_frame l_string_buffer;
		l_string_buffer.reserve(300);

		m_sprite_batch->begin(p_context.m_render_system.get_device(), p_context.m_render_system.get_device_swap_buffer(), m_back_buffer_view);

		const auto& l_counter_values = m_counter_values[m_counter_values_read_index];
		for(const auto& l_counter_name : l_counter_values)
		{
			const auto l_counter_value_ite = l_counter_value_manager.find(l_counter_name.c_str());
			const auto* l_counter_value = L"-";

			if (l_counter_value_ite != std::end(l_counter_value_manager))
			{
				l_counter_value = l_counter_value_ite->second.c_str();
			}

			const auto* l_counter_name_ptr = l_counter_name.c_str();
			std::mbsrtowcs(&l_to_wstring_buffer[0], &l_counter_name_ptr, l_counter_name.size(), &l_to_wstring_state);
			
			l_string_buffer.append(l_to_wstring_buffer, l_counter_name.size());
			l_string_buffer.append(L": ");
			l_string_buffer.append(l_counter_value);

			m_sprite_font->draw_string(*m_sprite_batch, l_string_buffer.c_str(), l_text_position_calculator(l_counter_ite));

			++l_counter_ite;
			l_string_buffer.clear();
			std::memset(&l_to_wstring_state, 0, sizeof(std::mbstate_t));
		}

		m_sprite_batch->end(p_context.m_render_system.get_device());
	}

	void bc_counter_value_draw_pass::before_reset(const game::bc_render_pass_reset_context& p_context)
	{
	}

	void bc_counter_value_draw_pass::after_reset(const game::bc_render_pass_reset_context& p_context)
	{
		m_back_buffer_view = *get_shared_resource<graphic::bc_render_target_view>(m_back_buffer_view_parameter);
	}

	void bc_counter_value_draw_pass::destroy(game::bc_render_system& p_render_system)
	{
		m_sprite_batch.reset();
		m_sprite_font.reset();
	}
}
