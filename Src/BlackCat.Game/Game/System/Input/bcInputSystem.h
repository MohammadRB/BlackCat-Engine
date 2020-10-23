// [05/16/2016 MRB]

#pragma once

#include "CorePlatform/Utility/bcNoCopy.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Event/bcEventManager.h"
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

			bc_icamera& get_camera()
			{
				return *m_camera;
			}

			const bc_icamera& get_camera() const
			{
				return const_cast<bc_input_system&>(*this).get_camera();
			}

			void register_camera(core::bc_unique_ptr< bc_icamera > p_camera);

			void update(core_platform::bc_clock::update_param p_clock_update_param);

		private:
			bool _event_handler(core::bc_ievent& p_event);

			platform::bc_key_device m_key_device;
			platform::bc_pointing_device m_pointing_device;
			
			core::bc_unique_ptr<bc_icamera> m_camera;
			
			core::bc_event_listener_handle m_window_resize_event_handle;
		};
	}
}