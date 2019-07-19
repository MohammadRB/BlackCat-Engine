// [05/16/2016 MRB]

#pragma once

#include "CorePlatform/Utility/bcNoCopy.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Event/bcEvent.h"
#include "Core/Event/bcEventManager.h"
#include "PlatformImp/Application/bcHumanInterfaceDevice.h"
#include "Graphic/bcEvent.h"
#include "Game/System/Input/bcCamera.h"

namespace black_cat
{
	namespace game
	{
		class bc_input_system : core_platform::bc_no_copy
		{
		public:
			bc_input_system();

			bc_input_system(bc_input_system&&) noexcept;

			~bc_input_system();

			bc_input_system& operator=(bc_input_system&&) noexcept;

			const platform::bc_key_device& get_key_device() const noexcept
			{
				return m_key_device;
			}

			const platform::bc_pointing_device& get_pointing_device() const noexcept
			{
				return m_pointing_device;
			}

			bc_icamera& get_camera()
			{
				return *m_camera.get();
			}

			const bc_icamera& get_camera() const
			{
				return const_cast<bc_input_system&>(*this).get_camera();
			}

			void register_camera(core::bc_unique_ptr< bc_icamera > p_camera);

			void update(core_platform::bc_clock::update_param p_clock_update_param);

		protected:

		private:
			bool _event_handler(core::bc_ievent& p_event);

			platform::bc_key_device m_key_device;
			platform::bc_pointing_device m_pointing_device;

			core::bc_event_listener_handle m_device_reset_listener_handle;

			core::bc_unique_ptr<bc_icamera> m_camera;
		};

		inline bc_input_system::bc_input_system()
			: m_key_device(platform::bc_human_interface_device::create_key_device(0)),
			m_pointing_device(platform::bc_human_interface_device::create_pointing_device(0))
		{
			m_device_reset_listener_handle = core::bc_get_service< core::bc_event_manager >()
				->register_event_listener< graphic::bc_app_event_device_reset >
				(
					core::bc_event_manager::delegate_type(this, &bc_input_system::_event_handler)
				);
		}

		inline bc_input_system::bc_input_system(bc_input_system&&) noexcept = default;

		inline bc_input_system::~bc_input_system() = default;

		inline bc_input_system& bc_input_system::operator=(bc_input_system&&) noexcept = default;

		inline void bc_input_system::register_camera(core::bc_unique_ptr<bc_icamera> p_camera)
		{
			m_camera = std::move(p_camera);
		}

		inline void bc_input_system::update(core_platform::bc_clock::update_param p_clock_update_param)
		{
			m_key_device.update();
			m_pointing_device.update();

			if(m_camera)
			{
				m_camera->update(p_clock_update_param);
			}
		}

		inline bool bc_input_system::_event_handler(core::bc_ievent& p_event)
		{
			auto* l_device_reset_event = core::bc_ievent::event_as<graphic::bc_app_event_device_reset>(p_event);
			if (l_device_reset_event)
			{
				if (!l_device_reset_event->m_before_reset)
				{
					if(m_camera)
					{
						m_camera->set_projection
						(
							l_device_reset_event->m_new_parameters.m_width,
							l_device_reset_event->m_new_parameters.m_height,
							m_camera->get_near_clip(),
							m_camera->get_far_clip()
						);
					}
				}

				return true;
			}

			return false;
		}
	}
}