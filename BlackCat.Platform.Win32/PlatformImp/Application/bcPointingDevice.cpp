// [04/15/2016 MRB]

#include "PlatformImp/PlatformImpPCH.h"

#include "CorePlatformImp/bcUtility.h"
#include "Platform/bcEvent.h"
#include "PlatformImp/bcExport.h"
#include "PlatformImp/Application/bcPointingDevice.h"

namespace black_cat
{
	namespace platform
	{
		void _check_pointing_device_index(bcBYTE p_device_index)
		{
			if (p_device_index != 0)
				throw std::runtime_error("Invalid device index");
		}

		template< >
		BC_PLATFORMIMP_DLL
		bc_platform_pointing_device<core_platform::g_api_win32>::bc_platform_pointing_device(bcUBYTE p_device_index)
			: m_pack(),
			m_device_index(p_device_index),
			m_event_manager(core::bc_service_manager::get().get_service< core::bc_event_manager >())
		{
			_check_pointing_device_index(m_device_index);
		}

		template< >
		BC_PLATFORMIMP_DLL
		bc_platform_pointing_device<core_platform::g_api_win32>::bc_platform_pointing_device(const bc_platform_pointing_device& p_other) noexcept
			: m_pack(p_other.m_pack),
			m_device_index(p_other.m_device_index),
			m_event_manager(p_other.m_event_manager)
		{
		}

		template< >
		BC_PLATFORMIMP_DLL
		bc_platform_pointing_device<core_platform::g_api_win32>::~bc_platform_pointing_device()
		{
		}

		template< >
		BC_PLATFORMIMP_DLL
		bc_platform_pointing_device<core_platform::g_api_win32>& bc_platform_pointing_device<core_platform::g_api_win32>::operator=(const bc_platform_pointing_device& p_other) noexcept
		{
			m_pack = p_other.m_pack;
			m_device_index = p_other.m_device_index;
			m_event_manager = p_other.m_event_manager;

			return *this;
		}

		template< >
		BC_PLATFORMIMP_DLL
		void bc_platform_pointing_device<core_platform::g_api_win32>::update()
		{
			POINT l_point;
			core_platform::win_call(GetCursorPos(&l_point) != 0);

			bcINT16 l_x = m_pack.m_state.m_x;
			bcINT16 l_y = m_pack.m_state.m_y;

			m_pack.m_state.m_dx = l_point.x - m_pack.m_state.m_x;
			m_pack.m_state.m_dy = l_point.y - m_pack.m_state.m_y;
			m_pack.m_state.m_x = l_point.x;
			m_pack.m_state.m_y = l_point.y;

			if (l_x != m_pack.m_state.m_x || l_y != m_pack.m_state.m_y)
			{
				m_event_manager->process_event(platform::bc_app_event_pointing(m_pack.m_state));
			}
		}

		template< >
		BC_PLATFORMIMP_DLL
		bc_pointing_device_state bc_platform_pointing_device<core_platform::g_api_win32>::get_state() const noexcept
		{
			return m_pack.m_state;
		}
	}
}