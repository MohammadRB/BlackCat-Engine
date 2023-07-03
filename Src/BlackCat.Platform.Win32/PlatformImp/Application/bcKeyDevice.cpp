// [15/04/2016 MRB]

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
		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_key_device<g_api_win32>::bc_platform_key_device(bcUBYTE p_device_index)
			: m_device_index(p_device_index),
			m_pack()
		{
			if (m_device_index != 0)
			{
				throw bc_runtime_exception("Invalid device index");
			}

			std::memset(&m_pack.m_state, static_cast<bcINT>(bc_key_state::up), sizeof(m_pack.m_state));
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_key_device<g_api_win32>::bc_platform_key_device(const bc_platform_key_device& p_other) noexcept
			: m_device_index(p_other.m_device_index),
			m_pack(p_other.m_pack)
		{
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_key_device<g_api_win32>::~bc_platform_key_device()
		{
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_platform_key_device<g_api_win32>& bc_platform_key_device<g_api_win32>::operator=(const bc_platform_key_device& p_other) noexcept
		{
			m_device_index = p_other.m_device_index;
			m_pack = p_other.m_pack;

			return *this;
		}

		template<>
		BC_PLATFORMIMP_DLL
		bc_key_state bc_platform_key_device<g_api_win32>::get_key_state(bc_key p_key) const noexcept
		{
			return m_pack.m_state[static_cast<bcINT>(p_key)];
		}

		template<>
		BC_PLATFORMIMP_DLL
		core::bc_vector_frame<std::pair<bc_key, bc_key_state>> bc_platform_key_device<g_api_win32>::update()
		{
			bcUBYTE l_key_states[256];
			core::bc_vector_frame<std::pair<bc_key, bc_key_state>> l_keys;
			l_keys.reserve(16);

			win_call(GetKeyboardState(l_key_states) != 0);

			for (bcUINT i = 0; i < 256; ++i)
			{
				auto l_key_state = bc_key_state::up;
				const bc_key_state l_prev_state = m_pack.m_state[i];
				const bool l_pressed = (l_key_states[i] & 0x80) > 0;

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
					l_keys.push_back(std::make_pair(static_cast<bc_key>(i), l_key_state));
				}
			}

			return l_keys;
		}
	}
}