// [09/16/2016 MRB]

#include "CoreTest/CoreTestPCH.h"
#include "Core/Memory/bcMemoryHeap.h"
#include <vector>
#include <thread>
#include <filesystem>

namespace black_cat
{
	namespace core_test
	{
		TEST(MemoryHeap, AllocationTest)
		{
			core::bc_memory_heap l_memory_heap;

			l_memory_heap.initialize(10000000, "");

			std::vector<std::pair<void*, core::bc_memblock>> l_memblocks;

			for (auto l_ite = 0U; l_ite < 5; ++l_ite)
			{
				core::bc_memblock l_block;
				l_block.size(4);

				void* l_result = l_memory_heap.alloc(&l_block);
				auto* l_pointer = static_cast<bcINT32*>(l_result);

				*l_pointer = l_ite;

				l_memblocks.push_back(std::make_pair(l_result, l_block));
			}

			for (auto i = 4; i >= 0; --i)
			{
				auto& l_block = l_memblocks[i];
				const auto* l_pointer = static_cast<bcINT32*>(l_block.first);

				EXPECT_TRUE(*l_pointer == i);

				l_memory_heap.free(l_block.first, &l_block.second);
			}

			EXPECT_TRUE(l_memory_heap.fragmentation_count() == 0);
			EXPECT_TRUE(l_memory_heap.tracer().alloc_count() == 0);
			EXPECT_TRUE(l_memory_heap.tracer().used_size() == 0);

			l_memory_heap.destroy();
		}

		TEST(MemoryHeap, RandomPopTest)
		{
			core::bc_memory_heap l_memory_heap;

			l_memory_heap.initialize(1000, "");

			std::vector<std::pair<void*, core::bc_memblock>> l_memblocks;

			for (auto l_ite = 0; l_ite < 5; ++l_ite)
			{
				core::bc_memblock l_block;
				l_block.size(10);

				void* l_result = l_memory_heap.alloc(&l_block);
				auto* l_pointer = static_cast<bcINT32*>(l_result);

				*l_pointer = l_ite;

				l_memblocks.push_back(std::make_pair(l_result, l_block));
			}

			const auto l_pop_array = { 3, 4, 1, 0, 2 };

			for (const auto l_pop_index : l_pop_array)
			{
				auto& l_block = l_memblocks[l_pop_index];
				const auto* l_pointer = static_cast<bcINT32*>(l_block.first);

				EXPECT_TRUE(*l_pointer == l_pop_index);

				l_memory_heap.free(l_block.first, &l_block.second);
			}

			EXPECT_TRUE(l_memory_heap.fragmentation_count() == 0);
			EXPECT_TRUE(l_memory_heap.tracer().alloc_count() == 0);
			EXPECT_TRUE(l_memory_heap.tracer().used_size() == 0);

			l_memory_heap.destroy();
		}

		TEST(MemoryHeap, MultithreadTest)
		{
			core::bc_memory_heap l_memory_heap;
			std::vector< std::thread > l_threads;
			std::atomic<int> l_start_flag;

			l_memory_heap.initialize(10000000, "");
			l_start_flag.store(0);

			for (auto l_thread_ite = 0; l_thread_ite < 25; ++l_thread_ite)
			{
				l_threads.push_back(std::thread([&]()
				{
					while (l_start_flag.load() == 0)
					{
					}

					std::vector<std::pair<void*, core::bc_memblock>> l_memblocks;

					for (auto i = 0; i < 1000; ++i)
					{
						core::bc_memblock l_block;
						l_block.size(4);

						void* l_result = l_memory_heap.alloc(&l_block);
						auto* l_pointer = static_cast<bcINT32*>(l_result);

						*l_pointer = i;

						l_memblocks.push_back(std::make_pair(l_result, l_block));
					}

					for (auto i = 999; i >= 0; --i)
					{
						auto& l_block = l_memblocks[i];
						const auto* l_pointer = static_cast<bcINT32*>(l_block.first);

						EXPECT_TRUE(*l_pointer == i);

						l_memory_heap.free(l_block.first, &l_block.second);
					}

				}));
			}

			l_start_flag.store(1);

			for (auto& l_thread : l_threads)
			{
				if (l_thread.joinable())
				{
					l_thread.join();
				}
			}

			EXPECT_TRUE(l_memory_heap.fragmentation_count() == 0);
			EXPECT_TRUE(l_memory_heap.tracer().alloc_count() == 0);
			EXPECT_TRUE(l_memory_heap.tracer().used_size() == 0);

			l_memory_heap.destroy();
		}

		TEST(MemoryHeap, DefragmentTest)
		{
#ifdef BC_MEMORY_DEFRAG
			constexpr bcSIZE l_alloc_count = 10;

			core::bc_memory_heap l_memory_heap;

			l_memory_heap.initialize(1000000, "");

			std::vector<std::pair<void*, core::bc_memblock>> l_memblocks;
			l_memblocks.reserve(l_alloc_count);

			for (auto l_ite = 0; l_ite < l_alloc_count; ++l_ite)
			{
				core::bc_memblock l_block;
				core::bc_memblock::initialize_mem_block_before_allocation(4, BC_MEMORY_MIN_ALIGN, &l_block);

				void* l_result = l_memory_heap.alloc(&l_block);

				core::bc_memblock::initialize_mem_block_after_allocation(&l_result, true, nullptr, &l_block);

				auto* l_pointer = static_cast<bcINT32*>(l_result);
				*l_pointer = l_ite;

				l_memblocks.push_back(std::make_pair(l_result, l_block));

				l_memory_heap.register_pointer(&l_memblocks.rbegin()->first, &l_memblocks.rbegin()->second);
			}

			auto l_pop_array = { 2, 3, 5, 9 };

			for (const auto l_pop_index : l_pop_array)
			{
				auto& l_entry = l_memblocks[l_pop_index];
				auto* l_pointer = l_entry.first;
				auto* l_memblock = &l_entry.second;
				auto* l_actual_pointer = reinterpret_cast<void*>(reinterpret_cast<bcUINTPTR>(l_pointer) - l_memblock->offset());

				EXPECT_TRUE(*static_cast<bcINT32*>(l_pointer) == l_pop_index);

				l_memory_heap.free(l_actual_pointer, l_memblock);
			}

			l_memory_heap.defrag(10, core::bc_memory_heap::defrag_callback());

			for (auto l_pop_index = 0; l_pop_index < l_alloc_count; ++l_pop_index)
			{
				const bool l_pop = std::find(std::begin(l_pop_array), std::end(l_pop_array), l_pop_index) != std::end(l_pop_array);
				
				if(!l_pop)
				{
					auto& l_entry = l_memblocks[l_pop_index];
					auto* l_pointer = l_entry.first;
					auto* l_memblock = &l_entry.second;
					auto* l_actual_pointer = reinterpret_cast< void* >(reinterpret_cast<bcUINTPTR>(l_pointer) - l_memblock->offset());

					EXPECT_TRUE(*static_cast<bcINT32*>(l_pointer) == l_pop_index);

					l_memory_heap.free(l_actual_pointer, l_memblock);
				}
			}

			EXPECT_TRUE(l_memory_heap.fragmentation_count() == 0);
			EXPECT_TRUE(l_memory_heap.tracer().alloc_count() == 0);
			EXPECT_TRUE(l_memory_heap.tracer().used_size() == 0);

			l_memory_heap.destroy();
#endif
		}

		TEST(MemoryHeap, RandomDefragmentTest)
		{
#ifdef BC_MEMORY_DEFRAG
			constexpr bcSIZE l_alloc_count = 1000;
			constexpr bcSIZE l_free_count = 100;

			core::bc_memory_heap l_memory_heap;
			std::vector< std::pair<void*, core::bc_memblock>> l_memblocks;
			std::vector<bcSIZE> l_free_indices;

			l_memory_heap.initialize(100000000, "");
			l_memblocks.reserve(l_alloc_count);
			l_free_indices.reserve(l_free_count);

			for (auto l_ite = 0; l_ite < l_alloc_count; ++l_ite)
			{
				core::bc_memblock l_block;
				core::bc_memblock::initialize_mem_block_before_allocation(4, BC_MEMORY_MIN_ALIGN, &l_block);

				void* l_result = l_memory_heap.alloc(&l_block);

				core::bc_memblock::initialize_mem_block_after_allocation(&l_result, true, nullptr, &l_block);

				auto* l_pointer = static_cast<bcINT32*>(l_result);
				*l_pointer = l_ite;

				l_memblocks.push_back(std::make_pair(l_result, l_block));

				l_memory_heap.register_pointer(&l_memblocks.rbegin()->first, &l_memblocks.rbegin()->second);
			}

			for(auto i = 0; i < l_free_count; ++i)
			{
				bcSIZE l_rand = (std::rand() * 1.0 / RAND_MAX) * l_alloc_count;
				const bool l_exist = std::find(std::begin(l_free_indices), std::end(l_free_indices), l_rand) != std::end(l_free_indices);

				if(!l_exist)
				{
					l_free_indices.push_back(l_rand);
				}
				else
				{
					--i;
				}
			}

			for (auto l_pop_index : l_free_indices)
			{
				auto& l_entry = l_memblocks[l_pop_index];
				auto* l_pointer = l_entry.first;
				auto* l_memblock = &l_entry.second;
				auto* l_actual_pointer = reinterpret_cast<void*>(reinterpret_cast<bcUINTPTR>(l_pointer) - l_memblock->offset());

				EXPECT_TRUE(*static_cast<bcINT32*>(l_pointer) == l_pop_index);

				l_memory_heap.free(l_actual_pointer, l_memblock);
			}

			l_memory_heap.defrag(l_memory_heap.fragmentation_count(), core::bci_memory_movable::defrag_callback());

			for (auto l_pop_index = 0; l_pop_index < l_alloc_count; ++l_pop_index)
			{
				const bool l_pop = std::find(std::begin(l_free_indices), std::end(l_free_indices), l_pop_index) != std::end(l_free_indices);

				if (!l_pop)
				{
					auto& l_entry = l_memblocks[l_pop_index];
					auto* l_pointer = l_entry.first;
					auto* l_memblock = &l_entry.second;
					auto* l_actual_pointer = reinterpret_cast<void*>(reinterpret_cast<bcUINTPTR>(l_pointer) - l_memblock->offset());

					EXPECT_TRUE(*static_cast<bcINT32*>(l_pointer) == l_pop_index);

					l_memory_heap.free(l_actual_pointer, l_memblock);
				}
			}

			EXPECT_TRUE(l_memory_heap.fragmentation_count() == 0);
			EXPECT_TRUE(l_memory_heap.tracer().alloc_count() == 0);
			EXPECT_TRUE(l_memory_heap.tracer().used_size() == 0);

			l_memory_heap.destroy();
#endif
		}
	}
}