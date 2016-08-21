// [06/30/2016 MRB]

#pragma once

#include "Core/Event/bcEvent.h"
#include "Graphic/Device/bcDevice.h"

namespace black_cat
{
	namespace graphic
	{
		class bc_app_event_device_reset : public core::bc_app_event
		{
		public:
			explicit bc_app_event_device_reset(bc_device* p_device,
				bc_device_parameters& p_old_parameters,
				bc_device_parameters& p_new_parameters,
				bool p_before_reset)
				: bc_app_event(event_name()),
				m_device(p_device),
				m_old_parameters(p_old_parameters),
				m_new_parameters(p_new_parameters),
				m_before_reset(p_before_reset)

			{
			}

			bc_app_event_device_reset(const bc_app_event_device_reset&) = default;

			~bc_app_event_device_reset() = default;

			bc_app_event_device_reset& operator =(const bc_app_event_device_reset&) = default;

			static const bcCHAR* event_name()
			{
				return core::g_evt_device_reset;
			}

			bc_device* m_device;
			bc_device_parameters& m_old_parameters;
			bc_device_parameters& m_new_parameters;
			bool m_before_reset;
		protected:

		private:
		};
	}
}