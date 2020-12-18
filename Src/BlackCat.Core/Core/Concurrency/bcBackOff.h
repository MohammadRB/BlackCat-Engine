//  [10/28/2013 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "Core/Concurrency/bcThreadLocal.h"
#include "Core/CorePCH.h"
#include "Core/Memory/bcAlloc.h"

namespace black_cat
{
	namespace core
	{
		class bc_back_off
		{
		public:
			bc_back_off(bcUINT64 p_min_time = std::chrono::nanoseconds(100).count(),
				bcUINT64 p_max_time = std::chrono::microseconds(100).count() * 1000,
				bcINT32 p_tick_threshold = 5)
				: m_min_time(p_min_time),
				m_max_time(p_max_time),
				m_tick_threshold(p_tick_threshold),
				m_locals()
			{
			}

			~bc_back_off() = default;

			void inc(bcINT32 p_tick_count = 1)
			{
				local_data* l_local = _get();

				l_local->m_tick += p_tick_count;
				while (l_local->m_tick >= m_tick_threshold)
				{
					l_local->m_tick -= m_tick_threshold;
					l_local->m_time = l_local->m_time * 2 + 1;

					if (l_local->m_time > m_max_time)
					{
						l_local->m_time = m_max_time;
						break;
					}
				}
			}

			void dec(bcINT32 p_tick_count = 1)
			{
				local_data* l_local = _get();

				l_local->m_tick -= p_tick_count;
				while (l_local->m_tick <= -m_tick_threshold)
				{
					l_local->m_tick += m_tick_threshold;
					l_local->m_time = l_local->m_time / 2;

					if (l_local->m_time < m_min_time)
					{
						l_local->m_time = m_min_time;
						break;
					}
				}
			}

			bcUINT64 get_current_wait_time()
			{
				local_data* lLocal = _get();
				return lLocal->m_time;
			}

			void reset()
			{
				local_data* l_local = _get();

				l_local->m_tick = 0;
				l_local->m_time = m_min_time;
			}

			void wait()
			{
				local_data* lLocal = _get();
				auto start = std::chrono::steady_clock::now();

				while (true)
				{
					if (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - start).count() > lLocal->m_time)
						break;
				}
			}

		private:
			struct local_data
			{
				bcUINT64 m_time;
				bcINT32 m_tick;

				local_data() : m_time(0), m_tick(0) { }
				~local_data() = default;
			};

			local_data* _get() const
			{
				return m_locals.get();
			}

			bcUINT64 m_min_time;
			bcUINT64 m_max_time;
			bcINT32 m_tick_threshold;
			bc_thread_local<local_data, bc_alloc_type::unknown> m_locals;
		};
	}
}