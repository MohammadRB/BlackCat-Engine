// [04/15/2016 MRB]

#include "PlatformImp/PlatformImpPCH.h"

#include "CorePlatformImp/bcUtility.h"
#include "Platform/bcException.h"
#include "Platform/bcEvent.h"
#include "PlatformImp/bcExport.h"
#include "PlatformImp/Application/bcKeyDevice.h"

namespace black_cat
{
	namespace platform
	{
		void _check_key_device_index(bcBYTE p_device_index)
		{
			if (p_device_index != 0)
			{
				throw bc_runtime_exception("Invalid device index");
			}
		}

		template< >
		BC_PLATFORMIMP_DLL
		bc_platform_key_device<core_platform::g_api_win32>::bc_platform_key_device(bcUBYTE p_device_index)
			: m_device_index(p_device_index),
			m_pack(),
			m_event_manager(core::bc_get_service<core::bc_event_manager>())
		{
			_check_key_device_index(m_device_index);

			std::memset(&m_pack.m_state, static_cast<bcINT>(bc_key_state::up), sizeof(m_pack.m_state));
		}

		template< >
		BC_PLATFORMIMP_DLL
		bc_platform_key_device<core_platform::g_api_win32>::bc_platform_key_device(const bc_platform_key_device& p_other) noexcept
			: m_device_index(p_other.m_device_index),
			m_pack(p_other.m_pack),
			m_event_manager(p_other.m_event_manager)
		{
		}

		template< >
		BC_PLATFORMIMP_DLL
		bc_platform_key_device<core_platform::g_api_win32>::~bc_platform_key_device()
		{
		}

		template< >
		BC_PLATFORMIMP_DLL
		bc_platform_key_device<core_platform::g_api_win32>& bc_platform_key_device<core_platform::g_api_win32>::operator=(const bc_platform_key_device& p_other) noexcept
		{
			m_pack = p_other.m_pack;
			m_device_index = p_other.m_device_index;
			m_event_manager = p_other.m_event_manager;

			return *this;
		}

		template< >
		BC_PLATFORMIMP_DLL
		void bc_platform_key_device<core_platform::g_api_win32>::update()
		{
			bcUBYTE l_key_states[256];

			core_platform::win_call(GetKeyboardState(l_key_states) != 0);

			for (bcUINT i = 0; i < 256; ++i)
			{
				bc_key_state l_key_state = bc_key_state::up;
				bc_key_state l_prev_state = m_pack.m_state[i];
				bool l_pressed = (l_key_states[i] & 0x80) > 0;

				if(l_pressed)
				{
					if (l_prev_state == bc_key_state::up)
					{
						l_key_state = bc_key_state::pressing;
					}
					else if (l_prev_state == bc_key_state::pressing || l_prev_state == bc_key_state::pressed)
					{
						l_key_state = bc_key_state::pressed;
					}
				}
				else
				{
					if (l_prev_state == bc_key_state::pressing || l_prev_state == bc_key_state::pressed)
					{
						l_key_state = bc_key_state::releasing;
					}
					else if(l_prev_state == bc_key_state::releasing)
					{
						l_key_state = bc_key_state::up;
					}
				}

				m_pack.m_state[i] = l_key_state;

				if (l_key_state != bc_key_state::up)
				{
					bc_app_event_key l_key_event(static_cast<bc_key>(i), l_key_state);

					m_event_manager->process_event(l_key_event);
				}
			}
		}

		template< >
		BC_PLATFORMIMP_DLL
		bc_key_state bc_platform_key_device<core_platform::g_api_win32>::get_key_state(bc_key p_key) const noexcept
		{
			return m_pack.m_state[static_cast< bcINT >(p_key)];
		}
	}
}