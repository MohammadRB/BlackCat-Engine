// [10/10/2014 MRB]

#pragma once

#include "CorePlatform/CorePlatformPCH.h"
#include "CorePlatform/bcType.h"
#include "CorePlatform/bcCorePlatformUtility.h"
#include "CorePlatform/Concurrency/bcConcurrencyDef.h"

namespace black_cat
{
	namespace core_platform
	{
		template<bc_platform TPlatform, typename T>
		struct bc_future_pack
		{
		};

		template<bc_platform TPlatform, typename T>
		struct bc_shared_future_pack
		{
		};

		template<bc_platform TPlatform, typename T>
		struct bc_promise_pack
		{
		};

		template< bc_platform TPlatform, typename T >
		class bc_shared_future_proxy;

		template< bc_platform TPlatform, typename T >
		class bc_promise_proxy;

		template< bc_platform TPlatform, typename T >
		class bc_future_proxy : private bc_no_copy
		{
			using type = T;
			using platform_pack = bc_future_pack< TPlatform, T >;
			using this_type = bc_future_proxy< TPlatform, T >;
			using shared_type = bc_shared_future_proxy< TPlatform, T >;
			using promise_type = bc_promise_proxy< TPlatform, T >;

			friend class promise_type;

		public:
			bc_future_proxy() noexcept(true);

			bc_future_proxy(this_type&& p_other) noexcept(true);

			~bc_future_proxy() noexcept(true);

			this_type& operator =(this_type&& p_other) noexcept(true);

			bcInline shared_type share();

			bcInline bool valid() const noexcept(true);

			bcInline type get();

			bcInline void wait() const noexcept(true);

			bcInline bc_future_status wait_for(const bcUINT64 p_nano) const;

		protected:

		private:
			platform_pack m_pack;
		};

		template< bc_platform TPlatform, typename T >
		class bc_shared_future_proxy
		{
			using type = T;
			using platform_pack = bc_shared_future_pack< TPlatform, T >;
			using this_type = bc_shared_future_proxy< TPlatform, T >;
			using future_type = bc_future_proxy< TPlatform, T >;

			friend class future_type;

		public:
			bc_shared_future_proxy() noexcept(true);

			bc_shared_future_proxy(const this_type& p_other) noexcept(true);

			bc_shared_future_proxy(this_type&& p_other) noexcept(true);

			bc_shared_future_proxy(future_type&& p_future);

			~bc_shared_future_proxy() noexcept(true);

			this_type& operator =(const this_type& p_other);

			this_type& operator =(this_type&& p_other);

			bcInline bool valid() const noexcept(true);

			bcInline type get() const;

			bcInline void wait() const noexcept(true);

			bcInline bc_future_status wait_for(const bcUINT64 p_nano) const;

		protected:

		private:
			platform_pack m_pack;
		};

		template< bc_platform TPlatform, typename T >
		class bc_promise_proxy : private bc_no_copy
		{
			using type = T;
			using platform_pack = bc_promise_pack<TPlatform, T>;
			using this_type = bc_promise_proxy< TPlatform, T >;
			using future_type = bc_future_proxy< TPlatform, T >;

		public:
			bc_promise_proxy() noexcept(true);

			// TODO: Check here
			/*template<typename Allocator>
			bcPromiseBase(std::allocator_arg_t, Allocator const&);*/

			bc_promise_proxy(this_type&& p_other) noexcept(true);

			~bc_promise_proxy();

			this_type& operator =(this_type&& p_other);

			bcInline void swap(this_type& p_other) noexcept(true);

			bcInline future_type get_future();

			bcInline void set_value(const type& p_value);

			bcInline void set_value(type&& p_value);

			bcInline void set_exception(std::exception_ptr p_exception);

		protected:

		private:
			platform_pack m_pack;
		};

		template< typename T >
		using bc_future = bc_future_proxy< g_current_platform, T >;
		template< typename T >
		using bc_shared_future = bc_shared_future_proxy< g_current_platform, T >;
		template< typename T >
		using bc_promise = bc_promise_proxy< g_current_platform, T >;
	}
}