// [11/8/2014 MRB]

#pragma once

#include "CorePlatformImp/CorePlatformImpPCH.h"
#include "CorePlatform/bcPlatform.h"
#include "CorePlatform/Concurrency/bcConcurrencyUtility.h"
#include "CorePlatform/bcCorePlatformException.h"

namespace black_cat
{
	namespace core_platform
	{
		template<>
		struct bc_platform_thread_local_pack<bc_platform::win32>
		{
			DWORD m_tls_index;
		};

		template< bc_platform TPlatform, typename T >
		void bc_platform_thread_local<TPlatform, T>::_platform_initialize()
		{
			if ((m_pack.m_tls_index = TlsAlloc()) == TLS_OUT_OF_INDEXES)
			{
				bcAssert(false);
				throw bc_thread_resource_exception("TLS allocation failed");
			}
		};

		template< bc_platform TPlatform, typename T >
		void bc_platform_thread_local<TPlatform, T>::_platform_cleanup()
		{
			if (m_pack.m_tls_index != TLS_OUT_OF_INDEXES)
				TlsFree(m_pack.m_tls_index);
		};

		template< bc_platform TPlatform, typename T >
		void bc_platform_thread_local<TPlatform, T>::_platform_assign(this_type&& p_other)
		{
			m_pack.m_tls_index = p_other.m_pack.m_tls_index;
		};

		template< bc_platform TPlatform, typename T >
		bool bc_platform_thread_local<TPlatform, T>::_platform_set(type* p_pointer)
		{
			if (TlsSetValue(m_pack.m_tls_index, p_pointer))
			{
				return true;
			}

			return false;
		};

		template< bc_platform TPlatform, typename T >
		typename bc_platform_thread_local<TPlatform, T>::type* bc_platform_thread_local<TPlatform, T>::_platform_get() const
		{
			type* l_pointer = static_cast< type* >(TlsGetValue(m_pack.m_tls_index));

			return l_pointer;
		};
	}
}