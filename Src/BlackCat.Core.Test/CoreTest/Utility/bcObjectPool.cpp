// [26/02/2022 MRB]

#include "CoreTest/CoreTestPCH.h"

#include "Core/Container/bcConcurrentQueue.h"
#include "Core/Utility/bcStopWatch.h"
#include "Core/Utility/bcObjectPoolAllocator.h"
#include "CoreTest/bcTestFixture.h"

namespace black_cat
{
	namespace core_test
	{
		TEST(ObjectPool, SingleThread)
		{
			bc_test_init();

			{
				constexpr auto l_pool_size = 1000000;
				core::bc_stop_watch l_stop_watch;

				core::bc_concurrent_memory_pool l_pool;
				l_pool.initialize(l_pool_size, sizeof(int), core::bc_alloc_type::program);

				std::vector<void*> l_allocated_pointers;
				l_allocated_pointers.reserve(l_pool_size);

				l_stop_watch.start();

				for (auto l_ite = 0U; l_ite < l_pool_size; ++l_ite)
				{
					l_allocated_pointers.push_back(std::malloc(sizeof(int)));
				}

				for (auto* l_pointer : l_allocated_pointers)
				{
					std::free(l_pointer);
				}

				l_stop_watch.stop();

				std::cout << "std elapsed: " << l_stop_watch.restart() << std::endl;

				l_allocated_pointers.clear();
				l_allocated_pointers.reserve(l_pool_size);

				l_stop_watch.start();

				for (auto l_ite = 0U; l_ite < l_pool_size; ++l_ite)
				{
					l_allocated_pointers.push_back(l_pool.alloc());
				}

				for (auto* l_pointer : l_allocated_pointers)
				{
					l_pool.free(l_pointer);
				}

				l_stop_watch.stop();

				std::cout << "object pool elapsed: " << l_stop_watch.restart() << std::endl;
			}

			bc_test_close();
		}

		TEST(ObjectPool, MultiThread)
		{
			bc_test_init();

			{
				constexpr auto l_push_thread_count = 4U;
				constexpr auto l_pop_thread_count = 4U;
				constexpr auto l_thread_item_count = 10000U;

				std::atomic<bcUINT32> l_producer_done(0U);
				std::atomic<bcUINT32> l_work_count(0U);
				core::bc_concurrent_queue<void*> l_allocated_pointers;
				core::bc_concurrent_memory_pool l_pool;
				l_pool.initialize(l_thread_item_count * l_push_thread_count, 64, core::bc_alloc_type::program);

				const auto l_std_elapsed = bc_multi_producer_multi_consumer_test
				(
					l_push_thread_count,
					l_pop_thread_count,
					[&]()
					{
						for (auto l_ite = 0U; l_ite < l_thread_item_count; ++l_ite)
						{
							l_allocated_pointers.push(std::malloc(64));
						}

						l_producer_done.fetch_add(1);
					},
					[&]()
					{
						bool l_pop = false;
						void* l_pop_item;

						while (l_producer_done.load() < l_push_thread_count || ((l_pop = l_allocated_pointers.pop(l_pop_item))))
						{
							if (l_pop)
							{
								std::free(l_pop_item);
								l_work_count.fetch_add(1, std::memory_order_relaxed);
							}
						}
					}
				);

				l_producer_done = 0;
				l_work_count = 0;

				const auto l_pool_elapsed = bc_multi_producer_multi_consumer_test
				(
					l_push_thread_count,
					l_pop_thread_count,
					[&]()
					{
						for (auto l_ite = 0U; l_ite < l_thread_item_count; ++l_ite)
						{
							l_allocated_pointers.push(l_pool.alloc());
						}

						l_producer_done.fetch_add(1);
					},
					[&]()
					{
						bool l_pop = false;
						void* l_pop_item;

						while (l_producer_done.load() < l_push_thread_count || ((l_pop = l_allocated_pointers.pop(l_pop_item))))
						{
							if (l_pop)
							{
								l_pool.free(l_pop_item);
								l_work_count.fetch_add(1, std::memory_order_relaxed);
							}
						}
					}
				);

				std::cout << "std elapsed ms: " << l_std_elapsed << std::endl;
				std::cout << "pool elapsed ms: " << l_pool_elapsed << std::endl;
				std::cout << "expected output value is " << l_push_thread_count * l_thread_item_count << std::endl;
				std::cout << "output value is " << l_work_count << std::endl;
				//std::cin.get();

				EXPECT_TRUE(l_work_count == l_push_thread_count * l_thread_item_count);
			}

			bc_test_close();
		}
	}
}
