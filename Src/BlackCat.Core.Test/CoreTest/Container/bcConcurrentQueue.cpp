// [10/22/2021 MRB]

#include "CoreTest/CoreTestPCH.h"

#include "Core/Container/bcConcurrentQueue.h"
#include "CoreTest/bcTestFixture.h"

namespace black_cat
{
	namespace core_test
	{
		TEST(ConcurrentQueue, multiwriter_singlereader)
		{
			bc_test_init();

			{
				constexpr auto l_thread_count = 8U;
				constexpr auto l_thread_item_count = 1000U;

				core::bc_concurrent_queue<unsigned> l_queue;
				std::vector<std::thread> l_threads;
				std::atomic<unsigned> l_started_threads_count(0);
				std::atomic<bool> l_start_work(false);

				for (auto l_thread_ite = 0U; l_thread_ite < l_thread_count; ++l_thread_ite)
				{
					l_threads.push_back
					(
						std::thread
						(
							[&]()
							{
								++l_started_threads_count;

								while (!l_start_work)
								{
								}

								for (auto l_ite = 0U; l_ite < l_thread_item_count; ++l_ite)
								{
									l_queue.push(l_ite);
								}
							}
						)
					);
				}

				while (l_started_threads_count != l_thread_count)
				{
				}

				l_start_work = true;

				for (auto& l_thread : l_threads)
				{
					l_thread.join();
				}

				std::vector<int> l_result;
				unsigned l_queue_item;
				while (l_queue.pop(l_queue_item))
				{
					l_result.push_back(l_queue_item);
				}

				EXPECT_TRUE(l_result.size() == l_thread_count * l_thread_item_count);
			}
			
			bc_test_close();
		}

		TEST(ConcurrentQueue, multiwriter_multireader)
		{
			bc_test_init();

			{
				constexpr auto l_thread_count = 8U;
				constexpr auto l_thread_item_count = 5000U;

				core::bc_concurrent_queue<unsigned> l_queue;
				std::vector<std::thread> l_threads;
				std::atomic<unsigned> l_started_threads_count(0U);
				std::atomic<unsigned> l_work_count(0U);
				std::atomic<bool> l_start_work(false);

				for (auto l_thread_ite = 0U; l_thread_ite < l_thread_count; ++l_thread_ite)
				{
					l_threads.push_back
					(
						std::thread
						(
							[&]()
							{
								++l_started_threads_count;

								while (!l_start_work)
								{
								}

								for (auto l_ite = 0U; l_ite < l_thread_item_count; ++l_ite)
								{
									l_queue.push(l_ite);
								}
							}
						)
					);
				}

				while (l_started_threads_count != l_thread_count)
				{
				}

				l_start_work = true;

				for (auto& l_thread : l_threads)
				{
					l_thread.join();
				}

				l_threads.clear();
				l_started_threads_count = 0;
				l_start_work = false;

				for (auto l_thread_ite = 0U; l_thread_ite < l_thread_count; ++l_thread_ite)
				{
					l_threads.push_back
					(
						std::thread
						(
							[&]()
							{
								++l_started_threads_count;

								while (!l_start_work)
								{
								}

								for (auto l_ite = 0U; l_ite < l_thread_item_count; ++l_ite)
								{
									unsigned l_pop_item;
									l_queue.pop(l_pop_item);

									++l_work_count;
								}
							}
						)
					);
				}

				while (l_started_threads_count != l_thread_count)
				{
				}

				l_start_work = true;

				for (auto& l_thread : l_threads)
				{
					l_thread.join();
				}

				EXPECT_TRUE(l_work_count == l_thread_count * l_thread_item_count);
			}

			bc_test_close();
		}

		TEST(ConcurrentQueue, concurrent_multiwriter_multireader)
		{
			bc_test_init();

			{
				constexpr auto l_push_thread_count = 4U;
				constexpr auto l_pop_thread_count = 4U;
				constexpr auto l_thread_item_count = 5000U;

				core::bc_concurrent_queue<unsigned> l_queue;
				std::vector<std::thread> l_push_threads;
				std::vector<std::thread> l_pop_threads;
				std::atomic<unsigned> l_started_threads_count(0);
				std::atomic<unsigned> l_work_count(0U);
				std::atomic<bool> l_start_work(false);

				for (auto l_thread_ite = 0U; l_thread_ite < l_push_thread_count; ++l_thread_ite)
				{
					l_push_threads.push_back
					(
						std::thread
						(
							[&]()
							{
								++l_started_threads_count;

								while (!l_start_work)
								{
								}

								for (auto l_ite = 0U; l_ite < l_thread_item_count; ++l_ite)
								{
									l_queue.push(l_ite);
								}
							}
						)
					);
				}

				for (auto l_thread_ite = 0U; l_thread_ite < l_pop_thread_count; ++l_thread_ite)
				{
					l_pop_threads.push_back
					(
						std::thread
						(
							[&]()
							{
								++l_started_threads_count;

								while (!l_start_work)
								{
								}

								bool l_pop = false;
								unsigned l_pop_item;

								while (l_start_work || ((l_pop = l_queue.pop(l_pop_item))))
								{
									if(l_pop)
									{
										++l_work_count;
									}
								}
							}
						)
					);
				}

				while (l_started_threads_count != l_push_thread_count + l_pop_thread_count)
				{
				}

				l_start_work = true;

				for (auto& l_thread : l_push_threads)
				{
					l_thread.join();
				}

				l_start_work = false;

				for (auto& l_thread : l_pop_threads)
				{
					l_thread.join();
				}

				std::cout << "number of expected value is " << l_push_thread_count * l_thread_item_count << std::endl;
				std::cout << "output value is " << l_work_count << std::endl;

				EXPECT_TRUE(l_work_count == l_push_thread_count * l_thread_item_count);
			}

			bc_test_close();
		}
	}
}