// [11/7/2014 MRB]

#pragma once

#include "coreplatform/coreplatformpch.h"
#include "CorePlatform/bcPlatform.h"
#include "CorePlatform/bcCorePlatformUtility.h"
#include "CorePlatform/Concurrency/bcThreadProvider.h"

namespace black_cat
{
	namespace core_platform
	{
		template< bc_platform TP, typename T >
		class bc_thread_local_provider : private bc_no_copy
		{
		public:
			using type = T;

		public:

		protected:

		private:
		};

		template< class TProvider, typename T >
		class bc_thread_local_proxy : private bc_no_copy
		{
		public:
			using type = T;
			using this_type = bc_thread_local_proxy;
			using provider_type = TProvider;
			typedef void(*cleanup_function)(type*);

		public:
			bc_thread_local_proxy() noexcept(false)
				: m_cleanup_function(&_default_clean_up)
			{
			}

			explicit bc_thread_local_proxy(cleanup_function p_cleanup_function) noexcept(false)
				: m_cleanup_function(p_cleanup_function)
			{
			}

			bc_thread_local_proxy(this_type&& p_other)
			{
				_assign(std::move(p_other));
			}

			~bc_thread_local_proxy() noexcept(true)
			{
				_cleanup();
			};

			this_type& operator =(this_type&& p_other)
			{
				_assign(std::move(p_other));

				return *this;
			}

			// return thread local pointer, or in case of any error return null
			bcInline type* get() const noexcept(true)
			{
				return m_provider.get();
			}

			// set thread local pointer and cleanup previous pointer and return true in case of success otherwise false
			bcInline bool set(type* p_pointer) noexcept(true)
			{
				type* l_pointer = get();
				if (!l_pointer)
					bc_thread::current_thread_on_exit(std::bind(&bc_thread_local_proxy::_cleanup, this));
				else
					m_cleanup_function(l_pointer);

				return m_provider.set(p_pointer);
			}

			bcInline type* operator ->() const noexcept(true)
			{
				//return m_provider.operator->();
				return m_provider.get();
			}

			bcInline type& operator *() const noexcept(true)
			{
				//return m_provider.operator*();
				return *m_provider.get();
			}

			// set thread local pointer to null and return current value
			bcInline type* release() noexcept(true)
			{
				return m_provider.release();
			}

		protected:

		private:
			static void _default_clean_up(type* p_pointer)
			{
				delete p_pointer;
			}

			void _cleanup()
			{
				type* l_pointer = get();
				if (l_pointer) // if resource wasn't released
				{
					m_cleanup_function(l_pointer);
				}
			}

			void _assign(this_type&& p_other)
			{
				m_provider = std::move(p_other.m_provider);
				m_cleanup_function = p_other.m_cleanup_function;
			}

			provider_type m_provider;
			cleanup_function m_cleanup_function;

		};

		template< typename T >
		using bc_thread_local = bc_thread_local_proxy< bc_thread_local_provider< g_current_platform, T >, T >;
	}
}