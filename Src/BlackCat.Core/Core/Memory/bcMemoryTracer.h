// [10/23/2014 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "Core/CorePCH.h"

namespace black_cat
{
	namespace core
	{
		class bc_memory_tracer
		{
		public:
			bc_memory_tracer()
#ifdef BC_MEMORY_TRACING
				: m_total_size(0U),
				m_alloc_count(0U),
				m_used_size(0U),
				m_overhead_size(0U),
				m_max_used_size(0U),
				m_rejected_alloc_count(0U),
				m_rejected_alloc_size(0U)
#endif
			{
			}

			bc_memory_tracer(const bc_memory_tracer& p_other)
#ifdef BC_MEMORY_TRACING
				: m_total_size(p_other.total_size()),
				m_alloc_count(p_other.alloc_count()),
				m_used_size(p_other.used_size()),
				m_overhead_size(p_other.overhead_size()),
				m_max_used_size(p_other.max_used_size()),
				m_rejected_alloc_count(p_other.rejected_alloc_count()),
				m_rejected_alloc_size(p_other.rejected_alloc_size())
#endif
			{
			}

			~bc_memory_tracer() = default;

			bc_memory_tracer& operator=(const bc_memory_tracer& p_other)
			{
#ifdef BC_MEMORY_TRACING
				total_size(p_other.total_size());
				used_size(p_other.used_size());
				overhead_size(p_other.overhead_size());
				max_used_size(p_other.max_used_size());
				alloc_count(p_other.alloc_count());
				rejected_alloc_size(p_other.rejected_alloc_size());
				rejected_alloc_count(p_other.rejected_alloc_count());
#endif

				return *this;
			}

			void initialize(bcSIZE p_total_size)
			{
#ifdef BC_MEMORY_TRACING
				m_total_size.store(p_total_size, core_platform::bc_memory_order::seqcst);
#endif
			}

			bcSIZE total_size() const
			{
#ifdef BC_MEMORY_TRACING
				return m_total_size.load(core_platform::bc_memory_order::relaxed);
#else
				return 0;
#endif
			}

			void total_size(bcSIZE p_val)
			{
#ifdef BC_MEMORY_TRACING
				m_total_size.store(p_val, core_platform::bc_memory_order::relaxed);
#endif
			}

			bcSIZE used_size() const
			{
#ifdef BC_MEMORY_TRACING
				return m_used_size.load(core_platform::bc_memory_order::relaxed);
#else
				return 0;
#endif
			}

			void used_size(bcSIZE p_val)
			{
#ifdef BC_MEMORY_TRACING
				m_used_size.store(p_val, core_platform::bc_memory_order::relaxed);
#endif 
			}

			bcSIZE overhead_size() const
			{
#ifdef BC_MEMORY_TRACING
				return m_overhead_size.load(core_platform::bc_memory_order::relaxed);
#else
				return 0;
#endif
			}

			void overhead_size(bcSIZE p_val)
			{
#ifdef BC_MEMORY_TRACING
				m_overhead_size.store(p_val, core_platform::bc_memory_order::relaxed);
#endif
			}

			bcSIZE max_used_size() const
			{
#ifdef BC_MEMORY_TRACING
				return m_max_used_size.load(core_platform::bc_memory_order::relaxed);
#else
				return 0;
#endif
			}

			void max_used_size(bcSIZE p_val)
			{
#ifdef BC_MEMORY_TRACING
				m_max_used_size.store(p_val, core_platform::bc_memory_order::relaxed);
#endif 
			}

			bcSIZE alloc_count() const
			{
#ifdef BC_MEMORY_TRACING
				return m_alloc_count.load(core_platform::bc_memory_order::relaxed);
#else
				return 0;
#endif
			}

			void alloc_count(bcSIZE p_val)
			{
#ifdef BC_MEMORY_TRACING
				m_alloc_count.store(p_val, core_platform::bc_memory_order::relaxed);
#endif
			}

			bcSIZE rejected_alloc_size() const
			{
#ifdef BC_MEMORY_TRACING
				return m_rejected_alloc_size.load(core_platform::bc_memory_order::relaxed);
#else
				return 0;
#endif
			}

			void rejected_alloc_size(bcSIZE p_val)
			{
#ifdef BC_MEMORY_TRACING
				m_rejected_alloc_size.store(p_val, core_platform::bc_memory_order::relaxed);
#endif 
			}

			bcSIZE rejected_alloc_count() const
			{
#ifdef BC_MEMORY_TRACING
				return m_rejected_alloc_count.load(core_platform::bc_memory_order::relaxed);
#else
				return 0;
#endif
			}

			void rejected_alloc_count(bcSIZE p_val)
			{
#ifdef BC_MEMORY_TRACING
				m_rejected_alloc_count.store(p_val, core_platform::bc_memory_order::relaxed);
#endif 
			}

			void accept_alloc(bcSIZE p_size)
			{
#ifdef BC_MEMORY_TRACING
				m_used_size.fetch_add(p_size, core_platform::bc_memory_order::relaxed);

				while (true)
				{
					bcSIZE l_local_max_used_size = m_max_used_size.load(core_platform::bc_memory_order::relaxed);
					bcSIZE l_local_used_size = m_used_size.load(core_platform::bc_memory_order::relaxed);
					bcSIZE l_new_max_used_size = std::max<bcUINT32>(l_local_used_size, l_local_max_used_size);

					if (m_max_used_size.compare_exchange_strong(
						&l_local_max_used_size,
						l_new_max_used_size,
						core_platform::bc_memory_order::relaxed,
						core_platform::bc_memory_order::relaxed))
					{
						break;
					}
				}

				m_alloc_count.fetch_add(1U, core_platform::bc_memory_order::relaxed);
#endif
			}

			void accept_free(bcSIZE p_size)
			{
#ifdef BC_MEMORY_TRACING
				m_used_size.fetch_sub(p_size, core_platform::bc_memory_order::relaxed);
				m_alloc_count.fetch_sub(1U, core_platform::bc_memory_order::relaxed);
#endif
			}

			void accept_overhead(bcSIZE p_size)
			{
#ifdef BC_MEMORY_TRACING
				m_overhead_size.fetch_add(p_size, core_platform::bc_memory_order::relaxed);
#endif
			}

			void accept_free_overhead(bcSIZE p_size)
			{
#ifdef BC_MEMORY_TRACING
				m_overhead_size.fetch_sub(p_size, core_platform::bc_memory_order::relaxed);
#endif
			}

			void reject_alloc(bcSIZE p_size)
			{
#ifdef BC_MEMORY_TRACING
				m_rejected_alloc_size.fetch_add(p_size, core_platform::bc_memory_order::relaxed);
				m_rejected_alloc_count.fetch_add(1U, core_platform::bc_memory_order::relaxed);
#endif
			}

			void accept_clear()
			{
#ifdef BC_MEMORY_TRACING
				m_used_size.store(0U, core_platform::bc_memory_order::relaxed);
				m_overhead_size.store(0U, core_platform::bc_memory_order::relaxed);
				m_alloc_count.store(0U, core_platform::bc_memory_order::relaxed);
#endif
			}

		protected:
#ifdef BC_MEMORY_TRACING
			core_platform::bc_atomic<bcSIZE> m_total_size;
			core_platform::bc_atomic<bcSIZE> m_alloc_count;
			core_platform::bc_atomic<bcSIZE> m_used_size;
			core_platform::bc_atomic<bcSIZE> m_overhead_size;
			core_platform::bc_atomic<bcSIZE> m_max_used_size;
			core_platform::bc_atomic<bcSIZE> m_rejected_alloc_count;
			core_platform::bc_atomic<bcSIZE> m_rejected_alloc_size;
#endif	
		};
	}
}