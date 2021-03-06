// [06/30/2016 MRB]

#pragma once

#include "Core/Messaging/Event/bcEvent.h"
#include "Graphic/bcRenderApi.h"
#include "Graphic/bcGraphicDefinition.h"

namespace black_cat
{
	namespace graphic
	{
		template<bc_render_api>
		class bc_platform_device;
		using bc_device = bc_platform_device< g_current_render_api >;

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
			bc_app_event_device_reset(bc_device& p_device,
				bc_device_parameters& p_old_parameters,
				bc_device_parameters& p_new_parameters,
				bool p_before_reset)
				: bc_app_render_event(message_name()),
				m_device(&p_device),
				m_old_parameters(p_old_parameters),
				m_new_parameters(p_new_parameters),
				m_before_reset(p_before_reset)
			{
			}

			bc_app_event_device_reset(const bc_app_event_device_reset&) = default;

			~bc_app_event_device_reset() = default;

			bc_app_event_device_reset& operator =(const bc_app_event_device_reset&) = default;

			bc_device* m_device;
			bc_device_parameters m_old_parameters;
			bc_device_parameters m_new_parameters;
			bool m_before_reset;
		};
	}
}