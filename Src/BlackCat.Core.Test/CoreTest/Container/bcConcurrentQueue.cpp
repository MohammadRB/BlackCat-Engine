// [10/22/2021 MRB]

#include "CoreTest/CoreTestPCH.h"

#include "Core/Container/bcConcurrentQueue.h"
#include "CoreTest/bcTestFixture.h"

namespace black_cat
{
	namespace core_test
	{
		TEST(ConcurrentQueue, OveralTest)
		{
			bc_test_init();
			
			const auto l_thread_count = 8U;
			const auto l_thread_item_count = 1000U;
			core::bc_concurrent_queue<int> l_queue;
			std::atomic<int> l_started_threads(0);
			std::atomic<int> l_work(-1);
			
			std::vector<std::thread> l_threads;
			for(auto l_thread_ite = 0U; l_thread_ite < l_thread_count; ++l_thread_ite)
			{
				l_threads.push_back
				(
					std::thread
					(
						[&]()
						{
							++l_started_threads;

							while (l_work == -1)
							{
							}

							for (auto i = 0U; i < l_thread_item_count; ++i)
							{
								l_queue.push(i);
							}

							++l_work;
						}
					)
				);
			}

			while(l_started_threads != l_thread_count)
			{
			}
			
			l_work = 0;
			
			for (auto& l_thread : l_threads)
			{
				l_thread.join();
			}
			
			std::vector<int> l_result;
			int l_queue_item;
			while (l_queue.pop(l_queue_item))
			{
				l_result.push_back(l_queue_item);
			}

			EXPECT_TRUE(l_result.size() == l_thread_count * l_thread_item_count);
			
			//bc_test_close();
		}
	}
}