// [24/08/2016 MRB]

#include "CoreTest/CoreTestPCH.h"
#include "Core/Memory/bcMemoryStack.h"
#include <vector>
#include <thread>
#include <filesystem>

namespace black_cat
{
	namespace core_test
	{
		TEST(MemoryStack, AllocationTest)
		{
			constexpr bcSIZE l_capacity = 10'000'000;
			constexpr bcSIZE l_alloc_count = 10000;

			core::bc_memory_stack l_memory_stack;
			l_memory_stack.initialize(8, l_capacity, "");
			std::vector<std::pair<void*, core::bc_memblock>> l_memblocks;
			
			for (auto i = 0U; i < l_alloc_count; ++i)
			{
				core::bc_memblock l_block;
				l_block.size(4);

				void* l_result = l_memory_stack.alloc(&l_block);
				//void* l_result = new bcINT32;
				auto* l_pointer = static_cast<bcINT32*>(l_result);

				*l_pointer = i;

				l_memblocks.push_back(std::make_pair(l_result, l_block));
			}

			for (bcINT32 i = l_alloc_count - 1; i >= 0; --i)
			{
				auto& l_block = l_memblocks[i];
				auto* l_pointer = static_cast<bcINT32*>(l_block.first);

				EXPECT_TRUE(*l_pointer == i);

				l_memory_stack.free(l_block.first, &l_block.second);
				//delete l_pointer;
			}

			EXPECT_TRUE(l_memory_stack.tracer().alloc_count() == 0);
			EXPECT_TRUE(l_memory_stack.tracer().used_size() == 0);
		}

		TEST(MemoryStack, RandomPopTest)
		{
			std::vector<std::pair<void*, core::bc_memblock>> l_memblocks;
			core::bc_memory_stack l_memory_stack;
			l_memory_stack.initialize(8, 1000, "");

			for (auto i = 0U; i < 5; ++i)
			{
				core::bc_memblock l_block;
				l_block.size(10);

				void* l_result = l_memory_stack.alloc(&l_block);
				auto* l_pointer = static_cast<bcINT32*>(l_result);

				*l_pointer = i;

				l_memblocks.push_back(std::make_pair(l_result, l_block));
			}

			const auto l_pop_array = { 3, 4, 1, 0, 2 };

			for (const auto l_pop_index : l_pop_array)
			{
				auto& l_block = l_memblocks[l_pop_index];
				auto* l_pointer = static_cast<bcINT32*>(l_block.first);

				EXPECT_TRUE(*l_pointer == l_pop_index);

				l_memory_stack.free(l_block.first, &l_block.second);
			}

			EXPECT_TRUE(l_memory_stack.tracer().alloc_count() == 0);
			EXPECT_TRUE(l_memory_stack.tracer().used_size() == 0);
		}

		TEST(MemoryStack, MultithreadTest)
		{
			constexpr bcSIZE l_thread_count = 2;
			constexpr bcSIZE l_alloc_count = 100'000;
			constexpr bcSIZE l_capacity = 10'000'000;

			std::vector<std::thread> l_threads;
			std::atomic<int> l_start_flag;

			core::bc_memory_stack l_memory_stack;
			l_memory_stack.initialize(l_thread_count, l_capacity, "");
			l_start_flag.store(0);

			for (auto ti = 0U; ti < l_thread_count; ++ti)
			{
				l_threads.push_back(std::thread([&]()
				{
					std::cout << platform::bc_thread::current_thread_id() << std::endl;

					while (l_start_flag.load() == 0)
					{
					}

					std::vector<std::pair<void*, core::bc_memblock>> l_memblocks;

					for (auto i = 0U; i < l_alloc_count; ++i)
					{
						core::bc_memblock l_block;
						l_block.size(4);

						//void* l_result = new bcINT32;
						void* l_result = l_memory_stack.alloc(&l_block);
						auto* l_pointer = static_cast<bcUINT32*>(l_result);

						*l_pointer = i;
						//*l_pointer = platform::bc_thread::current_thread_id();

						l_memblocks.push_back(std::make_pair(l_result, l_block));
					}

					for (int i = l_alloc_count - 1; i >= 0; --i)
					{
						auto& l_block = l_memblocks[i];
						const auto* l_pointer = static_cast<bcUINT32*>(l_block.first);

						EXPECT_TRUE(*l_pointer == i);

						l_memory_stack.free(l_block.first, &l_block.second);
						//delete l_pointer;
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

			EXPECT_TRUE(l_memory_stack.tracer().alloc_count() == 0);
			EXPECT_TRUE(l_memory_stack.tracer().used_size() == 0);

			//l_memory_stack.destroy();
		}
	}
}