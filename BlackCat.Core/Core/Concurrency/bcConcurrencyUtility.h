//  [10/28/2013 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomicProvider.h"
#include "CorePlatformImp/Concurrency/bcConcurrencyUtilityProvider.h"
#include "Core/CorePCH.h"
#include "Core/bcCoreUtility.h"
#include "Core/Memory/bcAlloc.h"

namespace black_cat
{
	namespace core
	{
		// Same as core_platform::bc_thread_local but provide auto initialization. if type is an integral it will initialize to zero
		template<typename T, bc_alloc_type t_alloc_type = bc_alloc_type::unknown>
		class bc_thread_local
		{
			using type = T;
			using this_type = bc_thread_local;

		public:
			bc_thread_local()
				: m_local(&_default_clean_up)
			{
				static_assert(t_alloc_type != bc_alloc_type::unknown_movale, "movable pointers are not allowed");
			}

			bc_thread_local(this_type&& p_other)
			{
				_assing(std::move(p_other));
			}

			~bc_thread_local()
			{
			}

			this_type& operator =(this_type&& p_other)
			{
				_assign(std::move(p_other));

				return *this;
			}

			bcInline type* get() const noexcept(true)
			{
				type* l_pointer = m_local.get();

				if (!l_pointer)
				{
					l_pointer = _create<T>();
					m_local.set(l_pointer);
				}

				return l_pointer;
			}

			bcInline bool set(type* p_pointer) noexcept(true)
			{
				return m_local.set(p_pointer);
			}

			bcInline type* operator ->() const noexcept(true)
			{
				return get();
			}

			bcInline type& operator *() const noexcept(true)
			{
				return *get();
			}

			bcInline type* release() noexcept(true)
			{
				return m_local.release();
			}
			
		protected:

		private:
			static void _default_clean_up(type* p_pointer)
			{
				bcDelete(p_pointer);
			}

			bcInline void _assing(this_type&& p_other)
			{
				m_local = std::move(m_local);
			}

			template< typename T >
			bcInline typename std::enable_if< std::is_integral< T >::value, T* >::type _create() const
			{
				return bcNew(T(0), t_alloc_type);
			}

			template< typename T >
			bcInline typename std::enable_if< !std::is_integral< T >::value, T* >::type _create() const
			{
				return bcNew(T(), t_alloc_type);
			}

			mutable core_platform::bc_thread_local<type> m_local;
		};

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

			~bc_back_off()
			{
			}

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

		protected:

		private:
			struct local_data
			{
				bcUINT64 m_time;
				bcINT32 m_tick;

				local_data() : m_time(0), m_tick(0) { }
				~local_data() = default;
			};

			bcInline local_data* _get()
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