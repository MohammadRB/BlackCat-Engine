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

	void bc_text_draw_pass::update(const game::bc_render_pass_update_param& p_param)
	{
	}

	void bc_text_draw_pass::initialize_frame(const game::bc_render_pass_render_param& p_param)
	{
	}

	void bc_text_draw_pass::execute(const game::bc_render_pass_render_param& p_param)
	{
		auto& l_counter_value_manager = *core::bc_get_service<core::bc_counter_value_manager>();
		
		const auto l_fps_counter = l_counter_value_manager.find("fps");
		const auto l_update_time = l_counter_value_manager.find("update_time");
		const auto l_render_time = l_counter_value_manager.find("render_time");

		core::bc_vector_frame<graphic::bc_device_text> l_texts;
		const auto l_text_bound = m_text_renderer->measure_text(L"Test");
		const auto l_text_position_calculator = [&](bcUINT32 i)
		{
			return core::bc_vector2i(10, 10 + i * (l_text_bound.y + 2));
		};
		
		if(l_fps_counter != std::end(l_counter_value_manager))
		{
			const graphic::bc_device_text l_text(L"FPS: " + l_fps_counter->second, l_text_position_calculator(0));
			l_texts.push_back(l_text);
		}
		if (l_update_time != std::end(l_counter_value_manager))
		{
			const graphic::bc_device_text l_text(L"update: " + l_update_time->second, l_text_position_calculator(1));
			l_texts.push_back(l_text);
		}
		if (l_render_time != std::end(l_counter_value_manager))
		{
			const graphic::bc_device_text l_text(L"render: " + l_render_time->second, l_text_position_calculator(2));
			l_texts.push_back(l_text);
		}

		m_text_renderer->draw_texts(p_param.m_render_system.get_device(), m_back_buffer_view, l_texts.data(), l_texts.size());
	}

	void bc_text_draw_pass::before_reset(const game::bc_render_pass_reset_param& p_param)
	{
	}

	void bc_text_draw_pass::after_reset(const game::bc_render_pass_reset_param& p_param)
	{
		m_back_buffer_view = *get_shared_resource<graphic::bc_render_target_view>(m_back_buffer_view_parameter);
	}

	void bc_text_draw_pass::destroy(game::bc_render_system& p_render_system)
	{
		m_text_renderer.reset();
	}
}