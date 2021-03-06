// [05/16/2016 MRB]

#pragma once

#include "CorePlatform/Utility/bcNoCopy.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Concurrency/bcMutexTest.h"
#include "Core/Container/bcVector.h"
#include "Core/Messaging/Event/bcEventListenerHandle.h"
#include "PlatformImp/Application/bcHumanInterfaceDevice.h"
#include "Game/System/Input/bcCamera.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_input_system : core_platform::bc_no_copy
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

			/**
			 * \brief Get last added camera as active camera
			 * \return 
			 */
			bci_camera* get_camera() noexcept
			{
				if(!m_editor_mode)
				{
					return m_cameras.back().get();
				}

				return m_editor_camera;
			}

			/**
			 * \brief Get last added camera as active camera
			 * \return
			 */
			const bci_camera* get_camera() const
			{
				return const_cast<bc_input_system&>(*this).get_camera();
			}

			/**
			 * \brief Add camera to camera stack
			 * \param p_camera 
			 * \return 
			 */
			bci_camera* add_camera(core::bc_unique_ptr<bci_camera> p_camera);
			
			/**
			 * \brief Add camera to camera stack and use it in editor mode 
			 * \param p_camera 
			 * \return 
			 */
			bci_camera* add_editor_camera(core::bc_unique_ptr<bci_camera> p_camera);

			void remove_camera(const bci_camera* p_camera);
			
			void update(core_platform::bc_clock::update_param p_clock_update_param);

		private:
			bool _event_handler(core::bci_event& p_event);

			core::bc_event_listener_handle m_window_resize_event_handle;
			core::bc_event_listener_handle m_editor_mode_event_handle;
			platform::bc_key_device m_key_device;
			platform::bc_pointing_device m_pointing_device;

			core::bc_mutex_test m_cameras_mutex;
			core::bc_vector<core::bc_unique_ptr<bci_camera>> m_cameras;

			bci_camera* m_editor_camera;
			bool m_editor_mode;
		};
	}
}