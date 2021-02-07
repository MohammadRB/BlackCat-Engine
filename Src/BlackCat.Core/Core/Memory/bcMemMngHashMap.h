//  [9/23/2013 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcConditionVariable.h"
#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/CorePCH.h"
#include "Core/Memory/bcMemBlock.h"

namespace black_cat
{
	namespace core
	{
#ifdef BC_MEMORY_ENABLE

		template< typename T, bcUINT32 ...tLevelSizes >
		class bc_memmng_hashmap;

		template< typename T, bcUINT32 ...tLevelSizes >
		class bc_memmng_hashmap
		{
		public:
			bc_memmng_hashmap() 
				: m_min_allocation_size(bc_memblock::get_required_size(1, BC_MEMORY_MIN_ALIGN)),
				m_level_count(sizeof...(tLevelSizes))
			{
				BC_ASSERT(m_level_count > 0);
				//bcAssert(sizeof...(tLevelSizes) == tLevelCount);

				bcUINT32 l_temp_level_sizes[sizeof...(tLevelSizes)] = { tLevelSizes... };
				std::memcpy(&m_level_sizes, &l_temp_level_sizes, m_level_count * sizeof(bcUINT32));

				m_chunks = new storage_chunk[m_level_sizes[0]];
				m_item_allocator.initialize();
			}

			~bc_memmng_hashmap()
			{
				_free_chunk(m_chunks, 0);
			}

			T* insert(void* p_pointer, const T& p_value)
			{
				storage_item* l_item = m_item_allocator.alloc();
				new (l_item)storage_item();

				storage_chunk* l_chunk = _find_chunk(p_pointer);

				if (!l_chunk->m_items)
				{
					l_chunk->m_items = l_item;
				}
				else
				{
					l_item->m_next = l_chunk->m_items;
					l_chunk->m_items = l_item;
				}

				l_chunk->unlock();

				l_item->m_key = p_pointer;
				l_item->m_data = p_value;

				return &l_item->m_data;
			}

			void remove(void* p_pointer)
			{
				storage_chunk* l_chunk = _find_chunk(p_pointer);

				storage_item* l_item = l_chunk->m_items;
				storage_item* l_prev_item = nullptr;
				while (l_item && l_item->m_key != p_pointer)
				{
					l_prev_item = l_item;
					l_item = l_item->m_next;
				}

				if (l_prev_item && l_item)
				{
					l_prev_item->m_next = l_item->m_next;
				}
				else if (!l_prev_item && l_item)
				{
					l_chunk->m_items = l_item->m_next;
				}
				else
				{
					BC_ASSERT(false);
				}

				l_chunk->unlock();

				l_item->~storage_item();
				m_item_allocator.free(l_item);
			}

			void relocate(void* p_old_pointer, void* p_new_pointer)
			{
				storage_item* l_item = _find_item(p_old_pointer);
				T l_data = l_item->m_data;

				remove(p_old_pointer);
				insert(p_new_pointer, l_data);
			}

			bcUINT32 count()
			{
				return m_item_allocator.count();
			}

			// Func(void*, T&)
			template<typename TFunc>
			void iterate_over(TFunc p_function)
			{
				_iterate_over(p_function, m_chunks, 0);
			}

		protected:

		private:
			struct storage_item
			{
			public:
				void* m_key;
				storage_item* m_next;
				T m_data;

				storage_item()
				{
					m_key = nullptr;
					m_next = nullptr;
				}
				storage_item(void* pKey, const T& pData)
				{
					m_data = pData;
					m_key = pKey;
					m_next = nullptr;
				}
				~storage_item() {}
			};

			struct storage_chunk
			{
			public:
				storage_chunk* m_sub_levels;
				storage_item* m_items;
				core_platform::bc_hybrid_mutex m_mutext;

			public:
				storage_chunk()
				{
					m_sub_levels = nullptr;
					m_items = nullptr;
				}

				~storage_chunk() {}

				void lock(core_platform::bc_lock_operation p_operation)
				{
					m_mutext.lock(p_operation);
				}

				bool try_lock(core_platform::bc_lock_operation p_operation)
				{
					return m_mutext.try_lock(p_operation);
				}

				void unlock()
				{
					m_mutext.unlock();
				}
			};

			bcUINT32 _convert_pointer_to_key(const void* p_pointer)
			{
				return std::hash<bcUINTPTR>()(reinterpret_cast<bcUINTPTR>(p_pointer));
			}

			storage_chunk* _find_chunk(void* p_pointer)
			{
				bcUINT32 l_key = _convert_pointer_to_key(p_pointer);
				storage_chunk* l_target_chunk = &m_chunks[l_key % m_level_sizes[0]];

				core_platform::bc_unique_lock<storage_chunk> l_gaurd(*l_target_chunk, core_platform::bc_lock_operation::light);

				for (bcUINT32 l = 1; l < m_level_count; ++l)
				{
					if (!l_target_chunk->m_sub_levels)
						l_target_chunk->m_sub_levels = new storage_chunk[m_level_sizes[l]];

					storage_chunk* l_sub_level = &l_target_chunk->m_sub_levels[l_key % m_level_sizes[l]];

					core_platform::bc_unique_lock<storage_chunk> l_gaurd_sub_level(*l_sub_level, core_platform::bc_lock_operation::light);
					l_gaurd.unlock();

					l_target_chunk = l_sub_level;
					l_gaurd = std::move(l_gaurd_sub_level);
				}

				return l_gaurd.release();
			}

			storage_item* _find_item(void* p_pointer)
			{
				storage_chunk* l_chunk = _find_chunk(p_pointer);

				storage_item* l_item = l_chunk->m_items;
				while (l_item && l_item->m_key != p_pointer)
				{
					l_item = l_item->m_next;
				}

				l_chunk->unlock();

				BC_ASSERT(l_item != nullptr);

				return l_item;
			}

			void _free_chunk(storage_chunk* l_chunk, bcUINT32 p_level)
			{
				// TODO is there any need to synchronization?
				for (bcUINT32 c = 0; c < m_level_sizes[p_level]; ++c)
				{
					if (l_chunk[c].m_sub_levels)
					{
						_free_chunk(l_chunk[c].m_sub_levels, p_level + 1);
					}
				}
				delete[] l_chunk;
			}

			template<typename TFunc>
			void _iterate_over(TFunc p_function, storage_chunk* p_chunks, bcUINT32 p_level)
			{
				for (bcUINT32 c = 0; c < m_level_sizes[p_level]; ++c)
				{
					storage_chunk* l_chunk = &p_chunks[c];
					core_platform::bc_unique_lock<storage_chunk> l_gaurd(*l_chunk, core_platform::bc_lock_operation::medium);
					storage_chunk* l_sub_chunks = l_chunk->m_sub_levels;

					if (l_sub_chunks)
					{
						BC_ASSERT(!l_chunk->m_items);

						l_gaurd.unlock();

						_iterate_over(p_function, l_sub_chunks, p_level + 1);
					}
					else
					{
						storage_item* l_item = l_chunk->m_items;
						while (l_item)
						{
							p_function(l_item->m_key, l_item->m_data);
							l_item = l_item->m_next;
						}
					}
				}
			}

			const bcUINT32 m_min_allocation_size;
			const bcUINT32 m_level_count;
			storage_chunk* m_chunks;
			bcUINT32 m_level_sizes[sizeof...(tLevelSizes)];
			bc_memmng_freelist<storage_item> m_item_allocator;

		};
#endif
	}
}