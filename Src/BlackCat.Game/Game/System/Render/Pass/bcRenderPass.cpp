// [22/01/2016 MRB]

#include "Game/GamePCH.h"
#include "Core/Messaging/Event/bcEventManager.h"
#include "Game/System/Render/Pass/bcRenderPass.h"
#include "Game/System/Render/bcRenderSystem.h"

namespace black_cat::game
{
	bc_render_pass_reset_context bc_render_pass_reset_context::create_default_instance(bc_render_system& p_render_system)
	{
		auto& l_device = p_render_system.get_device();
		auto& l_device_swap_buffer = p_render_system.get_device_swap_buffer();

		return bc_render_pass_reset_context
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
		);
	}

	inline void bci_render_pass::request_device_reset(const graphic::bc_device_parameters& p_new_parameters)
	{
		auto* l_event_manager = core::bc_get_service<core::bc_event_manager>();

		const graphic::bc_device_parameters l_old_parameters
		(
			m_device_swap_buffer->get_back_buffer_width(),
			m_device_swap_buffer->get_back_buffer_height(),
			m_device_swap_buffer->get_back_buffer_format(),
			m_device_swap_buffer->get_back_buffer_texture().get_sample_count()
		);

		// Put device reset event in render event queue
		l_event_manager->queue_event(graphic::bc_app_event_device_reset(l_old_parameters, p_new_parameters), 0);
	}
}
