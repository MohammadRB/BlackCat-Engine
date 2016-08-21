// [10/11/2014 MRB]

#pragma once

#include "CorePlatform/CorePlatformPCH.h"
#include "CorePlatform/bcType.h"
#include "CorePlatform/Utility/bcNoCopy.h"
#include "CorePlatform/Concurrency/bcConcurrencyDef.h"
#include "CorePlatform/Concurrency/bcAtomic.h"
#include "CorePlatform/Memory/bcMemAlloc.h"
#include <stack>
#include <map>
#include <atomic>

namespace black_cat
{
	namespace core_platform
	{
		template< bc_platform TP >
		struct bc_platform_thread_pack
		{
		};

		//class bc_per_thread_data : private bc_no_copy
		//{
		//public:
		//	using this_type = bc_per_thread_data;

		//public:
		//	bc_per_thread_data()
		//		: m_exiter()
		//	{
		//	};

		//	bc_per_thread_data(this_type&& p_other)
		//		: m_exiter(std::move(p_other.m_exiter))
		//	{
		//	}

		//	~bc_per_thread_data() = default;

		//	this_type& operator =(this_type&& p_other)
		//	{
		//		m_exiter = std::move(p_other.m_exiter);

		//		return *this;
		//	}

		//	void add_exit_func(std::function< void() > p_func)
		//	{
		//		m_exiter.add(p_func);
		//	}

		//protected:

		//private:
		//	class _thread_exiter : private bc_no_copy
		//	{
		//	public:
		//		using this_type = _thread_exiter;

		//	public:
		//		_thread_exiter() = default;

		//		_thread_exiter(this_type&& p_other)
		//			: m_exit_funcs(std::move(p_other.m_exit_funcs))
		//		{
		//		}

		//		~_thread_exiter()
		//		{
		//			while (!m_exit_funcs.empty())
		//			{
		//				m_exit_funcs.top()();
		//				m_exit_funcs.pop();
		//			}
		//		}

		//		this_type& operator =(this_type&& p_other)
		//		{
		//			m_exit_funcs = std::move(p_other.m_exit_funcs);

		//			return *this;
		//		}

		//		void add(std::function< void() > p_func)
		//		{
		//			m_exit_funcs.push(std::move(p_func));
		//		}

		//	protected:

		//	private:
		//		std::stack< std::function< void() > > m_exit_funcs;
		//	};

		//	_thread_exiter m_exiter;
		//};

		template< bc_platform TP >
		class bc_platform_thread : private bc_no_copy
		{
		public:
			using this_type = bc_platform_thread< TP >;
			using id = bcUINT32;
			using platform_pack = bc_platform_thread_pack<TP>;

		public:
			bc_platform_thread() noexcept(true);

			bc_platform_thread(this_type&& p_other) noexcept(true);

			template< typename Callable, typename ...Args >
			explicit bc_platform_thread(Callable&& pFunc, Args&&... pArgs);

			~bc_platform_thread()
			{
				//this_type::s_static_data.remove_me();
			}

			this_type& operator =(this_type&& p_other) noexcept(true);

			bcInline void swap(this_type& p_other) noexcept(true);

			bcInline void join();

			bcInline void detach();

			bcInline bool joinable() const noexcept(true);

			bcInline id get_id() const noexcept(true);

			bcInline static id current_thread_id() noexcept(true);

			bcInline static void current_thread_sleep_for(bcUINT64 p_nano);

			// make the processor available to other logical processors in a hyper threading enabled processor so that the other 
			// logical processors can make progress
			bcInline static void current_thread_yield() noexcept(true);

			// reschedule the execution of current thread, allowing other threads to run
			bcInline static void current_thread_yield_switch() noexcept(true);

			bcInline static void current_thread_on_exit(std::function< void() > p_func)
			{
				// http://stackoverflow.com/questions/20112221/invoking-a-function-automatically-on-stdthread-exit-in-c11
				class thread_exiter : private bc_no_copy
				{
				public:
					thread_exiter() = default;

					~thread_exiter()
					{
						while (!m_exit_funcs.empty())
						{
							m_exit_funcs.top()();
							m_exit_funcs.pop();
						}
					}

					void add(std::function< void() > p_func)
					{
						m_exit_funcs.push(std::move(p_func));
					}

				protected:

				private:
					std::stack< std::function< void() > > m_exit_funcs;
				};

				thread_local static thread_exiter l_exiter;
				l_exiter.add(std::move(p_func));

				//this_type::s_static_data.me().add_exit_func(p_func);
			}

			// Call this function when main thread is about exit, to remove it's per-thread-data
			static void on_main_thread_exit()
			{
				//this_type::s_static_data.remove_me();
			}

		protected:

		private:
			//class _static_data
			//{
			//private:
			//	using map_type = std::map< id, bc_per_thread_data >;

			//public:
			//	_static_data() : m_flag(), m_threads()
			//	{
			//		m_flag.clear(std::memory_order::memory_order_relaxed);
			//	}

			//	~_static_data() {}

			//	void remove_me()
			//	{
			//		lock();

			//		m_threads.erase(this_type::current_thread_id());

			//		unlock();
			//	}

			//	bc_per_thread_data& me()
			//	{
			//		id l_thread_id = this_type::current_thread_id();

			//		lock_shared();

			//		auto l_iterator = m_threads.find(l_thread_id);
			//		
			//		unlock_shared();

			//		if (l_iterator == m_threads.end())
			//		{
			//			lock();

			//			l_iterator = m_threads.insert(map_type::value_type(l_thread_id, bc_per_thread_data())).first;

			//			unlock();
			//		}

			//		return *&l_iterator->second;
			//	}

			//protected:

			//private:
			//	bcInline void lock()
			//	{
			//		while (m_flag.test_and_set(std::memory_order::memory_order_acquire));
			//	}

			//	bcInline void lock_shared()
			//	{
			//		lock();
			//	}

			//	bcInline void unlock()
			//	{
			//		m_flag.clear(std::memory_order::memory_order_release);
			//	}

			//	bcInline void unlock_shared()
			//	{
			//		unlock();
			//	}

			//	std::atomic_flag m_flag; // TODO
			//	map_type m_threads;
			//};

			//static _static_data s_static_data;
			platform_pack m_pack;
		};

		/*template< bc_platform TP >
		typename bc_platform_thread< TP >::_static_data bc_platform_thread< TP >::s_static_data;*/

		using bc_thread = bc_platform_thread< g_current_platform >;
	}
}