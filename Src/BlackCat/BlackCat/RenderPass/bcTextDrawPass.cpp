// [11/09/2020 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/Container/bcVector.h"
#include "Core/Utility/bcCounterValueManager.h"
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
		m_text_renderer = core::bc_make_unique<graphic::bc_device_text_renderer>
		(
			p_render_system.get_device().create_text_renderer()
		);
	}

	void bc_text_draw_pass::update(const game::bc_render_pass_update_context& p_param)
	{
	}

	void bc_text_draw_pass::initialize_frame(const game::bc_render_pass_render_context& p_param)
	{
	}

	void bc_text_draw_pass::execute(const game::bc_render_pass_render_context& p_param)
	{
		auto& l_counter_value_manager = *core::bc_get_service<core::bc_counter_value_manager>();

		core::bc_vector_frame<graphic::bc_device_text> l_texts;
		const auto l_text_bound = m_text_renderer->measure_text(L"Test");
		const auto l_text_position_calculator = [&](bcUINT32 i)
		{
			return core::bc_vector2i(10, 10 + i * (l_text_bound.y + 2));
		};

		for(bcUINT32 i = 0; i < m_counter_values.size(); ++i)
		{
			const auto* const l_counter = m_counter_values[i];
			const auto l_counter_value = l_counter_value_manager.find(l_counter);
			if (l_counter_value == std::end(l_counter_value_manager))
			{
				continue;
			}

			const graphic::bc_device_text l_text(core::bc_to_wstring(l_counter) + L": " + l_counter_value->second, l_text_position_calculator(i));
			l_texts.push_back(l_text);
		}
		
		m_text_renderer->draw_texts(p_param.m_render_system.get_device(), m_back_buffer_view, l_texts.data(), l_texts.size());
	}

	void bc_text_draw_pass::before_reset(const game::bc_render_pass_reset_context& p_param)
	{
	}

	void bc_text_draw_pass::after_reset(const game::bc_render_pass_reset_context& p_param)
	{
		m_back_buffer_view = *get_shared_resource<graphic::bc_render_target_view>(m_back_buffer_view_parameter);
	}

	void bc_text_draw_pass::destroy(game::bc_render_system& p_render_system)
	{
		m_text_renderer.reset();
	}
}