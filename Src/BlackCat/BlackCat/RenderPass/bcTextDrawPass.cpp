// [11/09/2020 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/Container/bcVector.h"
#include "Core/Utility/bcCounterValueManager.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "BlackCat/RenderPass/bcTextDrawPass.h"

namespace black_cat
{
	bc_text_draw_pass::bc_text_draw_pass(constant::bc_render_pass_variable_t p_back_buffer_view_parameter)
		: m_back_buffer_view_parameter(p_back_buffer_view_parameter)
	{
	}

	void bc_text_draw_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
		auto& l_device = p_render_system.get_device();
		auto& l_device_swap_buffer = p_render_system.get_device_swap_buffer();
		
		m_text_renderer = core::bc_make_unique<graphic::bc_device_text_renderer>
		(
			p_render_system.get_device().create_text_renderer()
		);
		m_text_bound = m_text_renderer->measure_text(L"Test");

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

	void bc_text_draw_pass::update(const game::bc_render_pass_update_context& p_context)
	{
	}

	void bc_text_draw_pass::initialize_frame(const game::bc_render_pass_render_context& p_context)
	{
	}

	void bc_text_draw_pass::execute(const game::bc_render_pass_render_context& p_context)
	{
		auto& l_counter_value_manager = *core::bc_get_service<core::bc_counter_value_manager>();
		const auto l_text_position_calculator = [this](bcUINT32 p_row)
		{
			return core::bc_vector2i(10, 10 + p_row * (m_text_bound.y + 2));
		};

		bcUINT32 l_counter_ite = 0;
		const auto& l_counter_values = get_global_config().get_counter_values();
		core::bc_vector_frame<graphic::bc_device_text> l_texts;
		l_texts.reserve(l_counter_values.size());

		std::mbstate_t l_to_wstring_state;
		bcWCHAR l_to_wstring_buffer[300];
		core::bc_wstring_frame l_string_buffer;
		l_string_buffer.reserve(300);
		
		for(const core::bc_json_value<core::bc_string>& l_counter_name : l_counter_values)
		{
			const auto l_counter_value_ite = l_counter_value_manager.find(l_counter_name->c_str());
			const auto* l_counter_value = L"-";

			if (l_counter_value_ite != std::end(l_counter_value_manager))
			{
				l_counter_value = l_counter_value_ite->second.c_str();
			}

			const auto* l_counter_name_ptr = (*l_counter_name).c_str();
			std::mbsrtowcs(&l_to_wstring_buffer[0], &l_counter_name_ptr, l_counter_name->size(), &l_to_wstring_state);
			
			//l_string_buffer.append(core::bc_to_wstring_frame(*l_counter_name));
			l_string_buffer.append(l_to_wstring_buffer, l_counter_name->size());
			l_string_buffer.append(L": ");
			l_string_buffer.append(l_counter_value);

			graphic::bc_device_text l_text
			(
				l_string_buffer.c_str(),
				l_text_position_calculator(l_counter_ite)
			);
			l_texts.push_back(std::move(l_text));

			++l_counter_ite;
			l_string_buffer.clear();
			std::memset(&l_to_wstring_state, 0, sizeof(std::mbstate_t));
		}
		
		m_text_renderer->draw_texts
		(
			p_context.m_render_system.get_device(), 
			p_context.m_render_system.get_device_swap_buffer(), 
			m_back_buffer_view, 
			l_texts.data(), 
			l_texts.size()
		);
	}

	void bc_text_draw_pass::before_reset(const game::bc_render_pass_reset_context& p_context)
	{
	}

	void bc_text_draw_pass::after_reset(const game::bc_render_pass_reset_context& p_context)
	{
		m_back_buffer_view = *get_shared_resource<graphic::bc_render_target_view>(m_back_buffer_view_parameter);
	}

	void bc_text_draw_pass::destroy(game::bc_render_system& p_render_system)
	{
		m_text_renderer.reset();
	}
}