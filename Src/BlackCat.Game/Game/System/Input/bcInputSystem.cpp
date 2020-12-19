// [05/16/2016 MRB]

#include "Game/GamePCH.h"

#include "Platform/bcEvent.h"
#include "Game/System/Input/bcInputSystem.h"

namespace black_cat
{
	namespace game
	{
		bc_input_system::bc_input_system()
			: m_key_device(platform::bc_human_interface_device::create_key_device(0)),
			m_pointing_device(platform::bc_human_interface_device::create_pointing_device(0))
		{
			m_window_resize_event_handle = core::bc_get_service< core::bc_event_manager >()
					->register_event_listener< platform::bc_app_event_window_resize >
					(
						core::bc_event_manager::delegate_type(*this, &bc_input_system::_event_handler)
					);
		}

		bc_input_system::bc_input_system(bc_input_system&&) noexcept = default;

		bc_input_system::~bc_input_system() = default;

		bc_input_system& bc_input_system::operator=(bc_input_system&&) noexcept = default;

		void bc_input_system::register_camera(core::bc_unique_ptr<bc_icamera> p_camera)
		{
			m_camera = std::move(p_camera);
		}

		void bc_input_system::update(core_platform::bc_clock::update_param p_clock_update_param)
		{
			m_key_device.update();
			m_pointing_device.update();

			if (m_camera)
			{
				m_camera->update(p_clock_update_param);
			}
		}

		bool bc_input_system::_event_handler(core::bc_ievent & p_event)
		{
			auto* l_window_resize_event = core::bc_imessage::as<platform::bc_app_event_window_resize>(p_event);
			if (l_window_resize_event)
			{
				if (m_camera)
				{
					m_camera->set_projection
					(
						l_window_resize_event->width(),
						l_window_resize_event->height(),
						m_camera->get_near_clip(),
						m_camera->get_far_clip()
					);
				}

				return true;
			}

			return false;
		}
	}
}