// [10/10/2014 MRB]

#pragma once

#include "CorePlatform/CorePlatformPCH.h"
#include "CorePlatform/bcType.h"
#include "CorePlatform/Utility/bcNoCopy.h"
#include "CorePlatform/Concurrency/bcConcurrencyDef.h"
#include "CorePlatform/bcPlatform.h"

namespace black_cat::platform
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

	template<bc_platform TPlatform, typename T>
	class bc_platform_shared_future;

	template<bc_platform TPlatform, typename T>
	class bc_platform_promise;

	template<bc_platform TPlatform, typename T>
	class bc_platform_future : private bc_no_copy
	{
		using type = T;
		using platform_pack = bc_platform_future_pack<TPlatform, T>;
		using this_type = bc_platform_future<TPlatform, T>;
		using shared_type = bc_platform_shared_future<TPlatform, T>;
		using promise_type = bc_platform_promise<TPlatform, T>;

		friend promise_type;

	public:
		bc_platform_future() noexcept;

		bc_platform_future(this_type&& p_other) noexcept;

		~bc_platform_future() noexcept;

		this_type& operator =(this_type&& p_other) noexcept;

		shared_type share();

		bool valid() const noexcept;

		type get();

		void wait() const noexcept;

		bc_future_status wait_for(const std::chrono::nanoseconds& p_duration) const;

		bc_future_status wait_for(const std::chrono::microseconds& p_duration) const;

		bc_future_status wait_for(const std::chrono::milliseconds& p_duration) const;

		bc_future_status wait_for(const std::chrono::seconds& p_duration) const;

	private:
		platform_pack m_pack;
	};

	template<bc_platform TPlatform, typename T>
	class bc_platform_shared_future
	{
		using type = T;
		using platform_pack = bc_platform_shared_future_pack<TPlatform, T>;
		using this_type = bc_platform_shared_future<TPlatform, T>;
		using future_type = bc_platform_future<TPlatform, T>;

		friend future_type;

	public:
		bc_platform_shared_future() noexcept;

		bc_platform_shared_future(const this_type& p_other) noexcept;

		bc_platform_shared_future(this_type&& p_other) noexcept;

		bc_platform_shared_future(future_type&& p_future);

		~bc_platform_shared_future() noexcept;

		this_type& operator =(const this_type& p_other);

		this_type& operator =(this_type&& p_other) noexcept;

		bool valid() const noexcept;

		type get() const;

		void wait() const noexcept;
			
		bc_future_status wait_for(const std::chrono::nanoseconds& p_duration) const;

		bc_future_status wait_for(const std::chrono::microseconds& p_duration) const;

		bc_future_status wait_for(const std::chrono::milliseconds& p_duration) const;

		bc_future_status wait_for(const std::chrono::seconds& p_duration) const;

	private:
		platform_pack m_pack;
	};

	template<bc_platform TPlatform, typename T>
	class bc_platform_promise : private bc_no_copy
	{
		using type = T;
		using platform_pack = bc_platform_promise_pack<TPlatform, T>;
		using this_type = bc_platform_promise<TPlatform, T>;
		using future_type = bc_platform_future<TPlatform, T>;

	public:
		bc_platform_promise() noexcept;

		// TODO: Check here
		/*template<typename Allocator>
			bcPromiseBase(std::allocator_arg_t, Allocator const&);*/

		bc_platform_promise(this_type&& p_other) noexcept;

		~bc_platform_promise();

		this_type& operator =(this_type&& p_other) noexcept;

		void swap(this_type& p_other) noexcept;

		future_type get_future();

		void set_value(const type& p_value);

		void set_value(type&& p_value);

		void set_exception(std::exception_ptr p_exception);

	private:
		platform_pack m_pack;
	};

	template<bc_platform TPlatform>
	class bc_platform_promise<TPlatform, void> : private bc_no_copy
	{
		using type = void;
		using platform_pack = bc_platform_promise_pack<TPlatform, void>;
		using this_type = bc_platform_promise<TPlatform, void>;
		using future_type = bc_platform_future<TPlatform, void>;

	public:
		bc_platform_promise() noexcept;

		// TODO: Check here
		/*template<typename Allocator>
			bc_platform_promise(std::allocator_arg_t, Allocator const&);*/

		bc_platform_promise(this_type&& p_other) noexcept;

		~bc_platform_promise();

		this_type& operator =(this_type&& p_other) noexcept;

		void swap(this_type& p_other) noexcept;

		future_type get_future();

		void set_value();

		void set_exception(std::exception_ptr p_exception);

	private:
		platform_pack m_pack;
	};

	template<typename T>
	using bc_future = bc_platform_future<g_current_platform, T>;
	template<typename T>
	using bc_shared_future = bc_platform_shared_future<g_current_platform, T>;
	template<typename T>
	using bc_promise = bc_platform_promise<g_current_platform, T>;
}
