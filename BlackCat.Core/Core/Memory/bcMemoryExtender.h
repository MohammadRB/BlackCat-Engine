// [11/24/2014 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/CorePCH.h"
#include "Core/Utility/bcInitializable.h"
#include "Core/Utility/bcDelegate.hpp"
#include "Core/Memory/bcMemory.h"
#include "Core/Memory/bcMemBlock.h"
#include "Core/Concurrency/bcConcurrency.h"

namespace black_cat
{
	namespace core
	{
		template< class TMemory >
		class _bc_memory_extender_bucket
		{
		public:
			using memory_type = TMemory;
			using initializer_type = bc_delegate< memory_type*() >;
			using cleanup_type = bc_delegate< void(memory_type*) >;

		public:
			_bc_memory_extender_bucket(initializer_type& p_initializer, cleanup_type& p_cleanup)
				: m_memory_initializer(p_initializer),
				m_memory_cleanup(p_cleanup),
				m_bucket_initializer(this, &_bc_memory_extender_bucket::_bucket_initializer),
				m_memory(nullptr),
				m_next(nullptr)
			{
			}

			_bc_memory_extender_bucket(_bc_memory_extender_bucket&& p_other) noexcept
				: m_memory_initializer(p_other.m_memory_initializer),
				m_memory_cleanup(p_other.m_memory_cleanup),
				m_bucket_initializer(std::move(p_other.m_bucket_initializer)),
				m_memory(p_other.m_memory),
				m_next(p_other.m_next)
			{
				p_other.m_memory = nullptr;
				p_other.m_next = nullptr;
			}

			~_bc_memory_extender_bucket()
			{
				m_memory_cleanup(m_memory.load(core_platform::bc_memory_order::relaxed));
				_bucket_cleanup(m_next.load(core_platform::bc_memory_order::relaxed));
			}

			_bc_memory_extender_bucket& operator=(_bc_memory_extender_bucket&& p_other) noexcept
			{
				m_memory_initializer = p_other.m_memory_initializer;
				m_memory_cleanup = p_other.m_memory_cleanup;
				m_bucket_initializer = std::move(p_other.m_bucket_initializer);
				m_memory = p_other.m_memory;
				m_next = p_other.m_next;

				p_other.m_memory = nullptr;
				p_other.m_next = nullptr;

				return *this;
			}

			memory_type* get_memory()
			{
				return bc_concurreny::double_check_lock(m_memory, m_memory_initializer);
			}

			_bc_memory_extender_bucket* get_next()
			{
				return bc_concurreny::double_check_lock(m_next, m_bucket_initializer);
			}

			void destroy()
			{
				auto* l_memory = m_memory.load(core_platform::bc_memory_order::relaxed);
				auto* l_next = m_next.load(core_platform::bc_memory_order::relaxed);
				
				if(l_memory)
				{
					l_memory->destroy();
				}

				if(l_next)
				{
					_bucket_cleanup(l_next);
					m_next.store(nullptr, core_platform::bc_memory_order::relaxed);
				}
			}

		protected:

		private:
			_bc_memory_extender_bucket* _bucket_initializer()
			{
				// Because we have override global new and delete operators, we must avoid using new operator to prevent
				// circular calls between memory management and this class
				_bc_memory_extender_bucket* l_result = reinterpret_cast<_bc_memory_extender_bucket*>(core_platform::bc_mem_alloc(sizeof(_bc_memory_extender_bucket)));
				new (l_result)_bc_memory_extender_bucket(m_memory_initializer, m_memory_cleanup);

				return l_result;
			}

			void _bucket_cleanup(_bc_memory_extender_bucket* p_pointer)
			{
				if(p_pointer)
				{
					p_pointer->~_bc_memory_extender_bucket();
					core_platform::bc_mem_free(p_pointer);
				}
			}

			initializer_type& m_memory_initializer;
			cleanup_type& m_memory_cleanup;
			bc_delegate<_bc_memory_extender_bucket*()> m_bucket_initializer;
			core_platform::bc_atomic< memory_type* > m_memory;
			core_platform::bc_atomic<_bc_memory_extender_bucket* > m_next;
		};

		template< typename TMemory >
		class bc_memory_extender : 
			public bc_memory, 
			public bc_initializable<bc_delegate< TMemory*() >&&, bc_delegate< void(TMemory*) >&&>
		{
		public:
			using this_type = bc_memory_extender;
			using _bucket = _bc_memory_extender_bucket<TMemory>;
			using memory_type = typename _bucket::memory_type;
			using initializer_type = typename _bucket::initializer_type;
			using cleanup_type = typename _bucket::cleanup_type;
			using is_movable_type = typename memory_type::is_movable_type;

		public:
			bc_memory_extender();

			bc_memory_extender(this_type&& p_other) noexcept;

			~bc_memory_extender();

			this_type& operator =(this_type&& p_other) noexcept;

			void* alloc(bc_memblock* p_memblock) noexcept override;

			void free(void* p_pointer, bc_memblock* p_memblock) noexcept override;

			bool contain_pointer(void* p_pointer) const noexcept override;

			void clear() noexcept override;

			const bc_memory_tracer& tracer();

		protected:

		private:
			void _initialize(initializer_type&& p_initializer, cleanup_type&& p_cleanup) override;

			void _destroy() noexcept override;

			void _assign(this_type&& p_other);

			initializer_type m_initializer;
			cleanup_type m_cleanup;
			_bucket m_first;
		};

		template< typename TMemory >
		bc_memory_extender< TMemory >::bc_memory_extender()
			: m_initializer(),
			m_cleanup(),
			m_first(m_initializer, m_cleanup)
		{
		}

		template< typename TMemory >
		bc_memory_extender< TMemory >::bc_memory_extender(this_type&& p_other) noexcept
		{
			_assign(std::move(p_other));
		}

		template< typename TMemory >
		bc_memory_extender< TMemory >::~bc_memory_extender()
		{
			if (m_initialized)
			{
				destroy();
			}
		}

		template< typename TMemory >
		typename bc_memory_extender< TMemory >::this_type& bc_memory_extender< TMemory >::operator=(this_type&& p_other) noexcept
		{
			_assign(std::move(p_other));
			return *this;
		}

		template< typename TMemory >
		void* bc_memory_extender< TMemory >::alloc(bc_memblock* p_memblock) noexcept
		{
			void* l_result = nullptr;
			_bucket* l_current_bucket = &m_first;

			while (l_result == nullptr)
			{
				l_result = l_current_bucket->get_memory()->alloc(p_memblock);

				if (!l_result)
				{
					l_current_bucket = l_current_bucket->get_next();
				}
			}

			return l_result;
		}

		template< typename TMemory >
		void bc_memory_extender< TMemory >::free(void* p_pointer, bc_memblock* p_memblock) noexcept
		{
			_bucket* l_current_bucket = &m_first;

			while (l_current_bucket)
			{
				if (l_current_bucket->get_memory()->contain_pointer(p_pointer))
				{
					l_current_bucket->get_memory()->free(p_pointer, p_memblock);
					break;
				}

				l_current_bucket = l_current_bucket->get_next();
			}
		}

		template< typename TMemory >
		bool bc_memory_extender< TMemory >::contain_pointer(void* p_pointer) const noexcept
		{
			bool l_result = false;
			_bucket* l_current_bucket = const_cast< _bucket* >(&m_first);

			while (l_current_bucket)
			{
				if (l_current_bucket->get_memory()->contain_pointer(p_pointer))
				{
					l_result = true;
					break;
				}

				l_current_bucket = l_current_bucket->get_next();
			}

			return l_result;
		}

		template< typename TMemory >
		void bc_memory_extender< TMemory >::clear() noexcept
		{
			_bucket* l_current_bucket = &m_first;

			while (l_current_bucket)
			{
				l_current_bucket->get_memory()->clear();

				l_current_bucket = l_current_bucket->get_next();
			}
		}

		template< typename TMemory >
		const bc_memory_tracer& bc_memory_extender< TMemory >::tracer()
		{
			_bucket* l_current_bucket = &m_first;
			bcSIZE l_total_size = 0;
			bcSIZE l_used_size = 0;
			bcSIZE l_alloc_count = 0;
			bcSIZE l_overhead_size = 0;
			bcSIZE l_max_used_size = 0;
			bcSIZE l_rejected_alloc_size = 0;
			bcSIZE l_rejected_alloc_count = 0;

			while (l_current_bucket)
			{
				const bc_memory_tracer& l_current_bucket_tracer = l_current_bucket->get_memory()->tracer();

				l_total_size += l_current_bucket_tracer.total_size();
				l_used_size += l_current_bucket_tracer.used_size();
				l_alloc_count += l_current_bucket_tracer.alloc_count();
				l_overhead_size += l_current_bucket_tracer.overhead_size();
				l_max_used_size += l_current_bucket_tracer.max_used_size();
				l_rejected_alloc_size += l_current_bucket_tracer.rejected_alloc_size();
				l_rejected_alloc_count += l_current_bucket_tracer.rejected_alloc_count();

				l_current_bucket = l_current_bucket->get_next();
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

		template< typename TMemory >
		void bc_memory_extender< TMemory >::_initialize(initializer_type&& p_initializer, cleanup_type&& p_cleanup)
		{
			m_initializer = std::move(p_initializer);
			m_cleanup = std::move(p_cleanup);
		}

		template< typename TMemory >
		void bc_memory_extender< TMemory >::_destroy() noexcept
		{
			m_first.destroy();
		}

		template< typename TMemory >
		void bc_memory_extender< TMemory >::_assign(this_type&& p_other)
		{
			m_initializer = std::move(p_other.m_initializer);
			m_cleanup = std::move(p_other.m_cleanup);
			m_first = std::move(p_other.m_first);
		}
	}
}