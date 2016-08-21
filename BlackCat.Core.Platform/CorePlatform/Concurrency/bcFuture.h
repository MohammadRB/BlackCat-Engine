// [10/10/2014 MRB]

#pragma once

#include "CorePlatform/CorePlatformPCH.h"
#include "CorePlatform/bcType.h"
#include "CorePlatform/Utility/bcNoCopy.h"
#include "CorePlatform/Concurrency/bcConcurrencyDef.h"

namespace black_cat
{
	namespace core_platform
	{
		template<bc_platform TPlatform, typename T>
		struct bc_platform_future_pack
		{
		};

		template<bc_platform TPlatform, typename T>
		struct bc_platform_shared_future_pack
		{
		};

		template<bc_platform TPlatform, typename T>
		struct bc_platform_promise_pack
		{
		};

		template< bc_platform TPlatform, typename T >
		class bc_platform_shared_future;

		template< bc_platform TPlatform, typename T >
		class bc_platform_promise;

		template< bc_platform TPlatform, typename T >
		class bc_platform_future : private bc_no_copy
		{
			using type = T;
			using platform_pack = bc_platform_future_pack< TPlatform, T >;
			using this_type = bc_platform_future< TPlatform, T >;
			using shared_type = bc_platform_shared_future< TPlatform, T >;
			using promise_type = bc_platform_promise< TPlatform, T >;

			friend  class promise_type;

		public:
			bc_platform_future() noexcept(true);

			bc_platform_future(this_type&& p_other) noexcept(true);

			~bc_platform_future() noexcept(true);

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
		class bc_platform_shared_future
		{
			using type = T;
			using platform_pack = bc_platform_shared_future_pack< TPlatform, T >;
			using this_type = bc_platform_shared_future< TPlatform, T >;
			using future_type = bc_platform_future< TPlatform, T >;

			friend class future_type;

		public:
			bc_platform_shared_future() noexcept(true);

			bc_platform_shared_future(const this_type& p_other) noexcept(true);

			bc_platform_shared_future(this_type&& p_other) noexcept(true);

			bc_platform_shared_future(future_type&& p_future);

			~bc_platform_shared_future() noexcept(true);

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
		class bc_platform_promise : private bc_no_copy
		{
			using type = T;
			using platform_pack = bc_platform_promise_pack<TPlatform, T>;
			using this_type = bc_platform_promise< TPlatform, T >;
			using future_type = bc_platform_future< TPlatform, T >;

		public:
			bc_platform_promise() noexcept(true);

			// TODO: Check here
			/*template<typename Allocator>
			bcPromiseBase(std::allocator_arg_t, Allocator const&);*/

			bc_platform_promise(this_type&& p_other) noexcept(true);

			~bc_platform_promise();

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

		template< bc_platform TPlatform >
		class bc_platform_promise< TPlatform, void > : private bc_no_copy
		{
			using type = void;
			using platform_pack = bc_platform_promise_pack<TPlatform, void>;
			using this_type = bc_platform_promise< TPlatform, void >;
			using future_type = bc_platform_future< TPlatform, void >;

		public:
			bc_platform_promise() noexcept(true);

			// TODO: Check here
			/*template<typename Allocator>
			bcPromiseBase(std::allocator_arg_t, Allocator const&);*/

			bc_platform_promise(this_type&& p_other) noexcept(true);

			~bc_platform_promise();

			this_type& operator =(this_type&& p_other);

			bcInline void swap(this_type& p_other) noexcept(true);

			bcInline future_type get_future();

			bcInline void set_value();

			bcInline void set_exception(std::exception_ptr p_exception);

		protected:

		private:
			platform_pack m_pack;
		};

		template< typename T >
		using bc_future = bc_platform_future< g_current_platform, T >;
		template< typename T >
		using bc_shared_future = bc_platform_shared_future< g_current_platform, T >;
		template< typename T >
		using bc_promise = bc_platform_promise< g_current_platform, T >;
	}
}