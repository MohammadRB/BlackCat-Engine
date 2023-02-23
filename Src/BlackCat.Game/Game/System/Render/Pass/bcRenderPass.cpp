// [01/22/2016 MRB]

#include "Game/GamePCH.h"
#include "Core/Messaging/Event/bcEventManager.h"
#include "Game/System/Render/Pass/bcRenderPass.h"

namespace black_cat::game
{
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
		l_event_manager->queue_event(graphic::bc_app_event_device_reset
		(
			*m_device,
			*m_device_swap_buffer,
			l_old_parameters,
			p_new_parameters
		), 0);
	}
}
