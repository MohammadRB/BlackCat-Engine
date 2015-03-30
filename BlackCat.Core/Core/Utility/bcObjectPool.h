// [2/12/2015 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "Core/CorePCH.h"
#include "Core/Memory/bcAlloc.h"

namespace black_cat
{
	namespace core
	{
		template< typename T >
		class bc_concurrent_object_pool;

		class BC_COREDLL_EXP bc_concurrent_memory_pool
		{
			template< typename T >
			friend class bc_concurrent_object_pool;

		public:
			bc_concurrent_memory_pool()
				: m_heap(nullptr)
			{
			};

			bc_concurrent_memory_pool(bc_concurrent_memory_pool&& p_other) noexcept(true)
			{
				_move(std::move(p_other));
			}

			~bc_concurrent_memory_pool()
			{
				if (m_heap)
					destroy();
			};

			bc_concurrent_memory_pool& operator =(bc_concurrent_memory_pool&& p_other) noexcept(true)
			{
				_move(std::move(p_other));

				return *this;
			}

			void initialize(bcUINT32 p_num_block, bcUINT32 p_block_size, bc_alloc_type p_alloc_type);

			void destroy() noexcept(true);

			bcUINT32 block_size() const noexcept(true) { return m_block_size; };

			bcUINT32 num_block() const noexcept(true) { return m_num_block; };

			// Return null pointer in case allocation failure
			void* alloc() noexcept(true);

			void free(void* p_pointer) noexcept(true);

			bool contain_pointer(const void* p_pointer) const noexcept(true);

			void clear() noexcept(true);

		protected:
			
		private:
			void _move(bc_concurrent_memory_pool&& p_other)
			{
				bcUINT32 l_allocated_block = p_other.m_allocated_block.load(core_platform::bc_memory_order::acquire);

				m_num_block = p_other.m_num_block;
				m_block_size = p_other.m_block_size;
				m_num_bitblocks = p_other.m_num_bitblocks;
				m_blocks = p_other.m_blocks;
				m_heap = p_other.m_heap;

				p_other.m_num_block = 0;
				p_other.m_block_size = 0;
				p_other.m_num_bitblocks = 0;
				p_other.m_blocks = nullptr;
				p_other.m_heap = nullptr;

				m_allocated_block.store(l_allocated_block, core_platform::bc_memory_order::release);
			}

			using bitblock_type = bcUINT32;
			static const bitblock_type s_bitblock_mask = 0xffffffff;
			static const bcSIZE s_bitblock_size = sizeof(bitblock_type);

			bcUINT32 m_num_block;
			bcUINT32 m_block_size;
			bcUINT32 m_num_bitblocks;
			// An index that searching for free block will continue from this place /
			core_platform::bc_atomic< bcUINT32 > m_allocated_block;
			// bit-vector indicating if a block is allocated or not /
			core_platform::bc_atomic< bitblock_type >* m_blocks;
			// block of data /
			bcUBYTE* m_heap;
		};

		template< typename T >
		class bc_concurrent_object_pool
		{
		public:
			bc_concurrent_object_pool() = default;

			bc_concurrent_object_pool(bc_concurrent_object_pool&& p_other)
				: m_memory_pool(std::move(p_other.m_memory_pool))
			{
			}

			~bc_concurrent_object_pool()
			{
				if (m_memory_pool.m_heap)
					destroy();
			}

			bc_concurrent_object_pool&& operator =(bc_concurrent_object_pool&& p_other)
			{
				m_memory_pool = std::move(p_other.m_memory_pool);

				return *this;
			}

			void initialize(bcUINT32 p_num_cached_objects, bc_alloc_type p_alloc_type)
			{
				m_alloc_type = p_alloc_type;
				m_memory_pool.initialize(p_num_cached_objects, sizeof(T), p_alloc_type);
			}

			void destroy() noexcept(true)
			{
				_destruct_objects();
				m_memory_pool.destroy();
			}

			bcUINT32 block_size() const { return m_memory_pool.block_size(); };

			bcUINT32 num_block() const { return m_memory_pool.num_block(); };

			// Allocate from pool or if it can't be done allocate object from global memory
			template< typename ...TArgs >
			T* alloc(TArgs&&... p_args) noexcept(std::is_nothrow_constructible<T, TArgs...>::value)
			{
				static_assert(std::is_constructible<T, TArgs...>::value, "class T is not constructible with parameters");

				T* l_result = m_memory_pool.alloc();
				
				if (!l_result)
				{
					l_result = bcAllocThrow(sizeof(T), m_alloc_type);
				}

				new(l_result)T(std::forward<TArgs>(p_args)...);

				return l_result;
			}

			void free(T* p_pointer) noexcept(true)
			{
				p_pointer->~T();

				if (m_memory_pool.contain_pointer(p_pointer))
				{
					m_memory_pool.free(p_pointer);
				}
				else
				{
					bcDelete(p_pointer);
				}
			}

			void clear() noexcept(true)
			{
				_destruct_objects();
				m_memory_pool.clear();
			}

		protected:

		private:
			void _destruct_objects()
			{
				for (bcUINT32 l_i = 0; l_i < m_memory_pool.m_num_bitblocks; ++l_i)
				{
					bc_concurrent_memory_pool::bitblock_type l_current_block = 
						m_memory_pool.m_blocks[l_i].load(core_platform::bc_memory_order::relaxed);
					for (bcUINT32 l_j = 0; l_j < bc_concurrent_memory_pool::s_bitblock_size; ++l_j)
					{
						bool l_is_alive = l_current_block & 1 << l_j;
						
						if (l_is_alive)
						{
							bcSIZE l_object_pos = l_i * bc_concurrent_memory_pool::s_bitblock_size + l_j;
							reinterpret_cast<T*>(m_memory_pool.m_heap + l_object_pos)->~T();
						}
					}
				}


			}

			bc_alloc_type m_alloc_type;
			bc_concurrent_memory_pool m_memory_pool;
		};
	}
}