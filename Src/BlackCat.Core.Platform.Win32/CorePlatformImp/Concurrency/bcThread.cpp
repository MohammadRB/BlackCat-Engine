// [05/15/2022 MRB]

#include "CorePlatformImp/CorePlatformImpPCH.h"
#include "CorePlatformImp/Concurrency/bcThread.h"
#include "CorePlatformImp/bcExport.h"
#include "CorePlatform/bcException.h"

namespace black_cat
{
	namespace platform
	{
		template<>
		BC_COREPLATFORMIMP_DLL const bcWCHAR* bc_platform_thread<bc_platform::win32>::get_name() const
		{
			using GetThreadDescription = HRESULT(WINAPI*)(HANDLE, PWSTR*);

			const HMODULE l_kernel_dll = GetModuleHandleA("KernelBase.dll");
			if (!l_kernel_dll)
			{
				throw bc_platform_exception(0, "Failed to load KernelBase.dll");
			}

			const auto l_thread_description_func = reinterpret_cast<GetThreadDescription>(GetProcAddress(l_kernel_dll, "GetThreadDescription"));
			if (!l_thread_description_func)
			{
				throw bc_platform_exception(0, "Failed to load GetThreadDescription function");
			}

			PWSTR l_desc;
			win_call(l_thread_description_func(const_cast<std::thread&>(m_pack.m_thread).native_handle(), &l_desc));

			return l_desc;
		}

		template<>
		BC_COREPLATFORMIMP_DLL void bc_platform_thread<bc_platform::win32>::set_name(const bcWCHAR* p_name)
		{
			// Use Runtime Dynamic Linking to for Win 10/Server 2016 build 1607
			using SetThreadDescription = HRESULT(WINAPI*)(HANDLE, PCWSTR);

			const HMODULE l_kernel_dll = GetModuleHandleA("KernelBase.dll");
			if (!l_kernel_dll) 
			{
				throw bc_platform_exception(0, "Failed to load KernelBase.dll");
			}

			const auto l_thread_description_func = reinterpret_cast<SetThreadDescription>(GetProcAddress(l_kernel_dll, "SetThreadDescription"));
			if (!l_thread_description_func) 
			{
				throw bc_platform_exception(0, "Failed to load SetThreadDescription function");
			}

			win_call(l_thread_description_func(m_pack.m_thread.native_handle(), p_name));
		}

		template<>
		BC_COREPLATFORMIMP_DLL bc_thread_priority bc_platform_thread<bc_platform::win32>::get_priority() const noexcept
		{
			auto l_priority = bc_thread_priority::normal;
			const auto l_priority_int = GetThreadPriority(const_cast<std::thread&>(m_pack.m_thread).native_handle());

			switch (l_priority_int)
			{
			case THREAD_PRIORITY_LOWEST:
				l_priority = bc_thread_priority::lowest;
				break;
			case THREAD_PRIORITY_BELOW_NORMAL:
				l_priority = bc_thread_priority::below_normal;
				break;
			case THREAD_PRIORITY_NORMAL:
				l_priority = bc_thread_priority::normal;
				break;
			case THREAD_PRIORITY_ABOVE_NORMAL:
				l_priority = bc_thread_priority::above_normal;
				break;
			case THREAD_PRIORITY_HIGHEST:
				l_priority = bc_thread_priority::highest;
				break;
			default:
				BC_ASSERT(false);
			}

			return l_priority;
		}

		template<>
		BC_COREPLATFORMIMP_DLL void bc_platform_thread<bc_platform::win32>::set_priority(bc_thread_priority p_priority) noexcept
		{
			auto l_priority = THREAD_PRIORITY_NORMAL;

			switch (p_priority)
			{
			case bc_thread_priority::lowest:
				l_priority = THREAD_PRIORITY_LOWEST;
				break;
			case bc_thread_priority::below_normal:
				l_priority = THREAD_PRIORITY_BELOW_NORMAL;
				break;
			case bc_thread_priority::normal:
				l_priority = THREAD_PRIORITY_NORMAL;
				break;
			case bc_thread_priority::above_normal:
				l_priority = THREAD_PRIORITY_ABOVE_NORMAL;
				break;
			case bc_thread_priority::highest:
				l_priority = THREAD_PRIORITY_HIGHEST;
				break;
			default:
				BC_ASSERT(false);
			}

			win_call(static_cast<bool>(SetThreadPriority(m_pack.m_thread.native_handle(), l_priority)));
		}
	}
}
