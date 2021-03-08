// [10/11/2014 MRB]

#pragma once

#include "CorePlatform/CorePlatformPCH.h"
#include "CorePlatform/Utility/bcNoCopy.h"
#include "CorePlatform/bcType.h"
#include "CorePlatform/bcPlatform.h"

#include <stack>

namespace black_cat
{
	namespace core_platform
	{
		enum class bc_thread_priority
		{
			lowest,
			below_normal,
			normal,
			above_normal,
			highest
		};
		
		template< bc_platform TP >
		struct bc_platform_thread_pack
		{
		};

		template< bc_platform TP >
		class bc_platform_thread : private bc_no_copy
		{
		public:
			using this_type = bc_platform_thread< TP >;
			using id = bcSIZE;
			using platform_pack = bc_platform_thread_pack<TP>;

		public:
			bc_platform_thread() noexcept;

			bc_platform_thread(this_type&& p_other) noexcept;

			template< typename Callable, typename ...Args >
			explicit bc_platform_thread(Callable&& pFunc, Args&&... pArgs);

			~bc_platform_thread();

			this_type& operator =(this_type&& p_other) noexcept;

			void swap(this_type& p_other) noexcept;

			void join();

			void detach();

			bool joinable() const noexcept;

			id get_id() const noexcept;

			const bcWCHAR* get_name() const noexcept;
			
			void set_name(const bcWCHAR* p_name) noexcept;

			bc_thread_priority get_priority() const noexcept;

			void set_priority(bc_thread_priority p_priority) noexcept;
			
			static id current_thread_id() noexcept;

			static void current_thread_sleep_for(const std::chrono::nanoseconds& p_duration);

			static void current_thread_sleep_for(const std::chrono::microseconds& p_duration);

			static void current_thread_sleep_for(const std::chrono::milliseconds& p_duration);

			static void current_thread_sleep_for(const std::chrono::seconds& p_duration);

			/**
			 * \brief make the processor available to other logical processors in a hyper threading enabled processor 
			 * so that the other logical processors can make progress
			 */
			static void current_thread_yield() noexcept;

			/**
			 * \brief reschedule the execution of current thread, allowing other threads to run
			 */
			static void current_thread_yield_switch() noexcept;

			static void current_thread_on_exit(std::function< void() > p_func)
			{
				// http://stackoverflow.com/questions/20112221/invoking-a-function-automatically-on-stdthread-exit-in-c11
				class thread_exit : private bc_no_copy
				{
				public:
					thread_exit() = default;

					~thread_exit()
					{
						while (!m_exit_functions.empty())
						{
							m_exit_functions.top()();
							m_exit_functions.pop();
						}
					}

					void add(std::function< void() > p_func)
					{
						m_exit_functions.push(std::move(p_func));
					}

				private:
					std::stack< std::function< void() > > m_exit_functions;
				};

				thread_local static thread_exit l_exit;
				l_exit.add(std::move(p_func));
			}
			
		private:
			platform_pack m_pack;
		};

		using bc_thread = bc_platform_thread< g_current_platform >;
	}
}