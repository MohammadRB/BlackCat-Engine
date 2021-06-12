// [08/24/2016 MRB]

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
			const bcSIZE l_alloc_count = 10000;

			core::bc_memory_stack l_memory_stack;
			std::vector<std::pair<void*, core::bc_memblock>> l_memblocks;

			l_memory_stack.initialize(8, 10000000, "");
			
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

			for (auto i = l_alloc_count - 1; i >= 0; --i)
			{
				auto& l_block = l_memblocks[i];
				auto* l_pointer = static_cast<bcINT32*>(l_block.first);

				EXPECT_TRUE(*l_pointer == i);

				l_memory_stack.free(l_block.first, &l_block.second);
				//delete l_pointer;
			}

			/*Assert::IsTrue(l_memory_stack.tracer().alloc_count() == 0);
			Assert::IsTrue(l_memory_stack.tracer().used_size() == 0);*/

			l_memory_stack.destroy();
		}

		TEST(MemoryStack, RandomPopTest)
		{
			core::bc_memory_stack l_memory_stack;
			std::vector<std::pair<void*, core::bc_memblock>> l_memblocks;

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

			auto l_pop_array = { 3, 4, 1, 0, 2 };

			for (auto l_pop_index : l_pop_array)
			{
				auto& l_block = l_memblocks[l_pop_index];
				auto* l_pointer = static_cast<bcINT32*>(l_block.first);

				EXPECT_TRUE(*l_pointer == l_pop_index);

				l_memory_stack.free(l_block.first, &l_block.second);
			}

			EXPECT_TRUE(l_memory_stack.tracer().alloc_count() == 0);
			EXPECT_TRUE(l_memory_stack.tracer().used_size() == 0);

			l_memory_stack.destroy();
		}

		TEST(MemoryStack, MultithreadTest)
		{
			const bcSIZE l_thread_count = 25;
			const bcSIZE l_alloc_count = 100;

			core::bc_memory_stack l_memory_stack;
			std::vector<std::thread> l_threads;
			std::atomic<int> l_start_flag;

			l_memory_stack.initialize(l_thread_count, 1000000, "");
			l_start_flag.store(0);

			for (auto ti = 0U; ti < l_thread_count; ++ti)
			{
				l_threads.push_back(std::thread([&]()
				{
					while (l_start_flag.load() == 0)
					{
					}

					std::vector<std::pair<void*, core::bc_memblock>> l_memblocks;

					for (int i = 0; i < l_alloc_count; ++i)
					{
						core::bc_memblock l_block;
						l_block.size(4);

						//void* l_result = new bcINT32;
						void* l_result = l_memory_stack.alloc(&l_block);
						auto* l_pointer = static_cast<bcINT32*>(l_result);

						*l_pointer = i;

						l_memblocks.push_back(std::make_pair(l_result, l_block));
					}

					for (int i = l_alloc_count - 1; i >= 0; --i)
					{
						auto& l_block = l_memblocks[i];
						auto* l_pointer = static_cast<bcINT32*>(l_block.first);

						EXPECT_TRUE(*l_pointer == i);

						l_memory_stack.free(l_block.first, &l_block.second);
						//delete l_block.first;
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

			l_memory_stack.destroy();
		}
	}
}