// [11/24/2014 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/bcCoreUtility.h"
#include "Core/Utility/bcLazy.h"
#include "Core/Utility/bcDelegate.h"
#include "Core/Memory/bcMemory.h"
#include "Core/Memory/bcMemBlock.h"

namespace black_cat
{
	namespace core
	{
		template< typename TMemory >
		class bc_memory_extender : public bc_memory, 
			bc_initializable<bc_delegate< TMemory*() >&&, bc_delegate< void(TMemory*) >&&>,
			private core_platform::bc_no_copy
		{
		public:
			using this_type = bc_memory_extender;
			using memory_type = TMemory;
			using initializer_type = bc_delegate< memory_type*() >;
			using cleanup_type = bc_delegate< void(memory_type*) >;
			using is_movable_type = typename memory_type::is_movable_type;

		private:
			class _bucket
			{
			public:
				memory_type* m_my;
				bc_concurrent_lazy< _bucket > m_next;

				_bucket(initializer_type& p_initializer, cleanup_type& p_cleanup)
					: m_initializer(p_initializer),
					m_cleanup(p_cleanup),
					m_my(p_initializer()),
					m_next(bc_delegate<_bucket*()>(this, &_bucket::_initializer),
						bc_delegate<void(_bucket*)>(this, &_bucket::_cleanup))
				{
				}

				_bucket(_bucket&& p_other)
					: m_initializer(p_other.m_initializer),
					m_cleanup(p_other.m_cleanup),
					m_my(std::move(p_other.m_my)),
					m_next(std::move(p_other.m_next))
				{
				}

				~_bucket()
				{
					m_cleanup(m_my);
				}

				_bucket& operator =(_bucket&& p_other)
				{
					m_initializer = p_other.m_initializer;
					m_cleanup = p_other.m_cleanup;
					m_my = std::move(p_other.m_my);
					m_next = std::move(p_other.m_next);

					return *this;
				}

			protected:

			private:
				_bucket* _initializer()
				{
					// Because we have override global new and delete operators, we must avoid using new operator to prevent
					// circular calls between memory management and this class
					_bucket* l_result = reinterpret_cast<_bucket*>(core_platform::bc_mem_alloc(sizeof(_bucket)));
					new (l_result)_bucket(m_initializer, m_cleanup);

					return l_result;
				}

				void _cleanup(_bucket* p_pointer)
				{
					p_pointer->~_bucket();
					core_platform::bc_mem_free(p_pointer);
				}

				initializer_type& m_initializer;
				cleanup_type& m_cleanup;
			};

		public:
			bc_memory_extender()
				: m_initializer(),
				m_cleanup(),
				m_first(bc_delegate< _bucket*() >(this, &this_type::_bucket_initializer), bc_delegate< void(_bucket*) >(this, &this_type::_bucket_cleanup))
			{
			}

			bc_memory_extender(this_type&& p_other) noexcept(true)
			{
				_assign(std::move(p_other));
			}

			~bc_memory_extender()
			{
				if (m_initialized)
					destroy();
			}

			this_type& operator =(this_type&& p_other) noexcept(true)
			{
				_assign(std::move(p_other));
				return *this;
			}

			void initialize(initializer_type&& p_initializer, cleanup_type&& p_cleanup) override
			{
				m_initializer = std::move(p_initializer);
				m_cleanup = std::move(p_cleanup);
				
				m_initialized = true;
			}

			void destroy() noexcept(true) override
			{
				bc_concurrent_lazy< _bucket >* l_current_bucket = &m_first;

				while (l_current_bucket->is_set())
				{
					(*l_current_bucket)->m_my->destroy();

					l_current_bucket = &((*l_current_bucket)->m_next);
				}

				m_initialized = false;
			}

			bcInline void* alloc(bc_memblock* p_memblock) noexcept(true) override
			{
				void* l_result = nullptr;
				bc_concurrent_lazy< _bucket >* l_current_bucket = &m_first;

				while (l_result == nullptr)
				{
					l_result = (*l_current_bucket)->m_my->alloc(p_memblock);

					if (!l_result)
						l_current_bucket = &((*l_current_bucket)->m_next);
				}

				return l_result;
			}

			bcInline void free(void* p_pointer, bc_memblock* p_memblock) noexcept(true) override
			{
				bc_concurrent_lazy< _bucket >* l_current_bucket = &m_first;

				while (l_current_bucket->is_set())
				{
					if ((*l_current_bucket)->m_my->contain_pointer(p_pointer))
					{
						(*l_current_bucket)->m_my->free(p_pointer, p_memblock);
						break;
					}

					l_current_bucket = &((*l_current_bucket)->m_next);
				}
			}

			bcInline bool contain_pointer(void* p_pointer) const noexcept(true) override
			{
				bool l_result = false;
				bc_concurrent_lazy< _bucket >* l_current_bucket = const_cast<bc_concurrent_lazy< _bucket >*>(&m_first);

				while (l_current_bucket->is_set())
				{
					if ((*l_current_bucket)->m_my->contain_pointer(p_pointer))
					{
						l_result = true;
						break;
					}

					l_current_bucket = &((*l_current_bucket)->m_next);
				}

				return l_result;
			}

			bcInline void clear() noexcept(true) override
			{
				bc_concurrent_lazy< _bucket >* l_current_bucket = &m_first;

				while (l_current_bucket->is_set())
				{
					(*l_current_bucket)->m_my->clear();

					l_current_bucket = &((*l_current_bucket)->m_next);
				}
			}

			const bc_memory_tracer& tracer()
			{
				bc_concurrent_lazy< _bucket >* l_current_bucket = &m_first;
				bcSIZE l_total_size = 0;
				bcSIZE l_used_size = 0;
				bcSIZE l_alloc_count = 0;
				bcSIZE l_overhead_size = 0;
				bcSIZE l_max_used_size = 0;
				bcSIZE l_rejected_alloc_size = 0;
				bcSIZE l_rejected_alloc_count = 0;

				while (l_current_bucket->is_set())
				{
					const bc_memory_tracer& l_current_bucket_tracer = (*l_current_bucket)->m_my->tracer();

					l_total_size += l_current_bucket_tracer.total_size();
					l_used_size += l_current_bucket_tracer.used_size();
					l_alloc_count += l_current_bucket_tracer.alloc_count();
					l_overhead_size += l_current_bucket_tracer.overhead_size();
					l_max_used_size += l_current_bucket_tracer.max_used_size();
					l_rejected_alloc_size += l_current_bucket_tracer.rejected_alloc_size();
					l_rejected_alloc_count += l_current_bucket_tracer.rejected_alloc_count();

					l_current_bucket = &((*l_current_bucket)->m_next);
				}

				m_tracer.total_size(l_total_size);
				m_tracer.used_size(l_used_size);
				m_tracer.alloc_count(l_alloc_count);
				m_tracer.overhead_size(l_overhead_size);
				m_tracer.max_used_size(l_max_used_size);
				m_tracer.rejected_alloc_size(l_rejected_alloc_size);
				m_tracer.rejected_alloc_count(l_rejected_alloc_count);

				return m_tracer;
			}

		protected:

		private:
			_bucket* _bucket_initializer()
			{
				_bucket* l_result = reinterpret_cast<_bucket*>(core_platform::bc_mem_alloc(sizeof(_bucket)));
				new (l_result)_bucket(m_initializer, m_cleanup);

				return l_result;
			}

			void _bucket_cleanup(_bucket* p_pointer)
			{
				p_pointer->~_bucket();
				core_platform::bc_mem_free(p_pointer);
			}

			bcInline void _assign(this_type&& p_other)
			{
				m_initializer = std::move(p_other.m_initializer);
				m_cleanup = std::move(p_other.m_cleanup);
				m_first = std::move(p_other.m_first);
			}

			initializer_type m_initializer;
			cleanup_type m_cleanup;
			bc_concurrent_lazy< _bucket > m_first;
		};
	}
}