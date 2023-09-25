// [30/06/2016 MRB]

#pragma once

#include "Core/Messaging/Event/bcEvent.h"
#include "Graphic/bcGraphicDefinition.h"

namespace black_cat::graphic
{
	struct bc_device_parameters
	{
	public:
		bc_device_parameters(bcUINT p_width,
			bcUINT p_height,
			bc_format p_format,
			bc_texture_ms_config p_multi_sample)
			: m_width(p_width),
			m_height(p_height),
			m_format(p_format),
			m_multi_sample(p_multi_sample)
		{
		}

		bcUINT m_width;
		bcUINT m_height;
		bc_format m_format;
		bc_texture_ms_config m_multi_sample;
	};
		
	class bc_app_event_device_reset : public core::bc_app_render_event
	{
		BC_EVENT(dvc_rst)

	public:
		bc_app_event_device_reset(const bc_device_parameters& p_old_parameters,
			const bc_device_parameters& p_new_parameters)
			: bc_app_render_event(message_name()),
			  m_old_parameters(p_old_parameters),
			  m_new_parameters(p_new_parameters)
		{
		}

		bc_app_event_device_reset(const bc_app_event_device_reset&) = default;

		~bc_app_event_device_reset() override = default;

		bc_app_event_device_reset& operator =(const bc_app_event_device_reset&) = default;

		bc_device_parameters m_old_parameters;
		bc_device_parameters m_new_parameters;
	};
}
