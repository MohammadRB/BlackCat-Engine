// [10/13/2014 MRB]

#pragma once

#include "CorePlatform/CorePlatformPCH.h"
#include "CorePlatform/bcPlatform.h"
#include "CorePlatform/bcCorePlatformUtility.h"

namespace black_cat
{
	namespace core_platform
	{
		template< bc_platform TP >
		class bc_atomic_flag_provider : private bc_no_copy
		{
		public:
			
		protected:

		private:

		};

		template< bc_platform TP, typename T >
		class bc_atomic_provider : private bc_no_copy
		{
		public:

		protected:

		private:

		};

		template< typename TProvider >
		class bc_atomic_flag_proxy : private bc_no_copy
		{
		private:
			using provider_type = TProvider;
		public:
			using this_type = bc_atomic_flag_proxy;

		public:
			bc_atomic_flag_proxy() noexcept(true)
				: m_provider()
			{
			}

			bc_atomic_flag_proxy(bool p_flag) noexcept(true)
				: TProvider(p_flag)
			{
			}

			~bc_atomic_flag_proxy() noexcept(true)
			{
			}

			bcInline bool test_and_set() volatile noexcept(true)
			{
				return m_provider.test_and_set();
			}

			bcInline bool test_and_set() noexcept(true)
			{
				return m_provider.test_and_set();
			}

			bcInline bool test_and_set(bc_memory_order p_order) volatile noexcept(true)
			{
				return m_provider.test_and_set(p_order);
			}

			bcInline bool test_and_set(bc_memory_order p_order) noexcept(true)
			{
				return m_provider.test_and_set(p_order);
			}

			bcInline void clear() volatile noexcept(true)
			{
				m_provider.clear();
			}

			bcInline void clear() noexcept(true)
			{
				m_provider.clear();
			}

			bcInline void clear(bc_memory_order p_order) volatile noexcept(true)
			{
				m_provider.clear(p_order);
			}

			bcInline void clear(bc_memory_order p_order) noexcept(true)
			{
				m_provider.clear(p_order);
			}

		protected:

		private:
			TProvider m_provider;
		};

		template< typename TProvider, typename T >
		class bc_atomic_proxy : public TProvider
		{
		private:
			using provider_type = TProvider;
		public:
			using this_type = bc_atomic_proxy;

		public:
			bc_atomic_proxy() noexcept(true)
			{
			}

			bc_atomic_proxy(T p_value) noexcept(true)
				: provider_type(p_value)
			{
			}

			bc_atomic_proxy(const this_type&) = delete;

			~bc_atomic_proxy() noexcept(true)
			{
			}

			this_type& operator =(const this_type&) = delete;
		};

		using bc_atomic_flag = bc_atomic_flag_proxy< bc_atomic_flag_provider< g_current_platform > >;
		template< typename T >
		using bc_atomic = bc_atomic_proxy< bc_atomic_provider< g_current_platform, T >, T >;
	}
}