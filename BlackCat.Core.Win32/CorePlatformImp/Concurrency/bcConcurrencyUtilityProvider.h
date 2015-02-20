// [11/8/2014 MRB]

#pragma once

#include "coreplatformimp/corewin32pch.h"
#include "coreplatform/bcplatform.h"
#include "coreplatform/concurrency/bcconcurrencyutilityprovider.h"
#include "CorePlatform/bcCorePlatformException.h"

namespace black_cat
{
	namespace core_platform
	{
		class BC_COREWIN32_DLL_EXP bc_thread_local_counter
		{
		public:
			bc_thread_local_counter() = default;

			~bc_thread_local_counter() = default;

			static bcSIZE get()
			{
				return m_num;
			}

			static bcSIZE inc()
			{
				return ++m_num;
			}

			static bcSIZE dec()
			{
				return --m_num;
			}

		protected:

		private:
			static bcSIZE m_num;
		};

		template< typename T >
		class bc_thread_local_provider< bc_platform::win32, T > : public bc_thread_local_provider< bc_platform::unknown, T >
		{
			using base_type = bc_thread_local_provider< bc_platform::unknown, T >;

		public:
			using type = typename base_type::type;
			using this_type = bc_thread_local_provider;

		public:
			bc_thread_local_provider() noexcept(false)
			{
#ifdef BC_DEBUG
				_inc_thread_local_count();
#endif 

				if ((m_tls_index = TlsAlloc()) == TLS_OUT_OF_INDEXES)
				{
					throw bc_thread_resource_exception("TLS alloc failed");
				}
			};

			bc_thread_local_provider(this_type&& p_other)
			{
				_assign(std::move(p_other));
			}

			~bc_thread_local_provider() noexcept(true)
			{
#ifdef BC_DEBUG
				_dec_thread_local_count();
#endif 
				if (m_tls_index != TLS_OUT_OF_INDEXES)
					TlsFree(m_tls_index);
			};

			this_type& operator =(this_type&& p_other)
			{
				_assign(std::move(p_other));

				return *this;
			}

			bcInline type* get() const noexcept(true)
			{
				type* l_pointer = nullptr;
				l_pointer = static_cast< type* >(TlsGetValue(m_tls_index));

				return l_pointer;
			}

			bcInline bool set(type* p_pointer) noexcept(true)
			{
				if (TlsSetValue(m_tls_index, p_pointer))
				{
					return true;
				}

				return false;
			}

			//bcInline type* operator ->() const noexcept(true)
			//{
			//	return get();
			//}

			//bcInline type& operator *() const noexcept(true)
			//{
			//	return *get();
			//}

			bcInline type* release() noexcept(true)
			{
				type* l_pointer = get();
				set(nullptr);

				return l_pointer;
			}

		protected:

		private:
			void _inc_thread_local_count()
			{
				bcAssert(bc_thread_local_counter::inc() <= TLS_MINIMUM_AVAILABLE);
			}

			void _dec_thread_local_count()
			{
				bc_thread_local_counter::dec();
			}

			void _assign(this_type&& p_other)
			{
				m_tls_index = p_other.m_tls_index;
				p_other.m_tls_index = TLS_OUT_OF_INDEXES;
			}

			DWORD m_tls_index;
		};
	}
}