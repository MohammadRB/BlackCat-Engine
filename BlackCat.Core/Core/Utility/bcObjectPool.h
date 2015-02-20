// [2/12/2015 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/Memory/bcAlloc.h"
#include "Core/Utility/bcMemoryPool.h"

namespace black_cat
{
	namespace core
	{
		template< typename T >
		class bc_concurrent_object_pool : private bc_concurrent_memory_pool
		{
		public:
			bc_concurrent_object_pool()
			{	
			}

			bc_concurrent_object_pool(bc_concurrent_object_pool&& p_other)
				: m_pool(std::move(p_other.m_pool))
			{
			}

			~bc_concurrent_object_pool()
			{
				if (bc_concurrent_memory_pool::m_heap != nullptr)
					destroy();
			}

			bc_concurrent_object_pool&& operator =(bc_concurrent_object_pool&& p_other)
			{
				bc_concurrent_memory_pool::operator=(std::move(p_other));

				return *this;
			}

			void initialize(bcUINT32 p_num_cached_objects, bc_alloc_type p_alloc_type)
			{
				m_alloc_type = p_alloc_type;
				bc_concurrent_memory_pool::initialize(p_num_cached_objects, sizeof(T), p_alloc_type);
			}

			void destroy() noexcept(true)
			{
				_destruct_objects();
				bc_concurrent_memory_pool::destroy();
			}

			bcUINT32 block_size() const { return bc_concurrent_memory_pool::block_size(); };

			bcUINT32 num_block() const { return bc_concurrent_memory_pool::num_block(); };

			// Allocate from pool or if it can't be done allocate object from global memory
			template< typename ...TArgs >
			T* alloc(TArgs&&... p_args) noexcept(std::is_nothrow_constructible<T, TArgs...>::value)
			{
				static_assert(std::is_constructible<T, TArgs...>::value, "class T is not constructible with parameters");

				T* l_result = bc_concurrent_memory_pool::alloc();
				
				if (!l_result)
				{
					l_result = bcAllocThrow(sizeof(T), m_alloc_type);
				}

				new(l_result)T(std::forward<TArgs>(p_args)...);

				return l_result;
			}

			void free(T* p_pointer) noexcept(true)
			{
				bcDelete(p_pointer);
			}

			bool contain_pointer(const T* p_pointer) const noexcept(true)
			{
				return bc_concurrent_memory_pool::contain_pointer(p_pointer);
			}

			void clear() noexcept(true)
			{
				_destruct_objects();
				bc_concurrent_memory_pool::clear();
			}

		protected:

		private:
			// TODO find a better way to destruct allocated objects without any need to inherite from memory pool
			void _destruct_objects()
			{
				for (bcUINT32 l_i = 0; l_i < bc_concurrent_memory_pool::m_num_bitblocks; ++l_i)
				{
					bitblock_type l_current_block = bc_concurrent_memory_pool::m_blocks[l_i].load(core_platform::bc_memory_order::relaxed);
					for (bcUINT32 l_j = 0; l_j < bc_concurrent_memory_pool::s_bitblock_size; ++l_j)
					{
						bool l_is_alive = l_current_block & 1 << l_j;
						
						if (l_is_alive)
						{
							bcSIZE l_object_pos = l_i * bc_concurrent_memory_pool::s_bitblock_size + l_j;
							reinterpret_cast<T*>(bc_concurrent_memory_pool::m_heap + l_object_pos)->~T();
						}
					}
				}


			}

			bc_alloc_type m_alloc_type;
		};
	}
}