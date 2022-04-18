// [10/22/2021 MRB]

#include "CoreTest/CoreTestPCH.h"

//#include "Core/Container/bcConcurrentQueue.h"
#include "Core/Utility/bcObjectPoolAllocator.h"
#include "Core/Utility/bcStopWatch.h"
#include "CoreTest/bcTestFixture.h"

#include "concurrentqueue.h"
#include "bcConcurrentQueue.h"
#include "bcConcurrentQueue1.h"

namespace black_cat
{
	namespace core_test
	{
		template<typename T>
		class bc_test_allocator : public std::allocator<T>
		{
		public:
			using is_movable_type = std::false_type;

			template<typename TOther>
			struct rebind
			{
				using other = bc_test_allocator<TOther>;
			};

			bc_test_allocator() noexcept = default;

			bc_test_allocator(const bc_test_allocator&) noexcept = default;

			template<typename TOther>
			bc_test_allocator(const bc_test_allocator<TOther>&) noexcept
			{
			}

			bc_test_allocator(bc_test_allocator&&) noexcept = default;

			~bc_test_allocator() = default;

			bc_test_allocator& operator=(const bc_test_allocator&) noexcept = default;

			template<typename TOther>
			bc_test_allocator& operator=(const bc_test_allocator<TOther>&) noexcept
			{
				return *this;
			}

			bc_test_allocator& operator=(bc_test_allocator&&) noexcept = default;

			void deallocate(pointer p_pointer, size_type n = 0) noexcept
			{
				std::allocator<T>::deallocate(p_pointer, n);
			}
		};

		template<typename T>
		class bc_pool_allocator : public std::allocator<T>
		{
		public:
			using is_movable_type = std::false_type;

			template<typename TOther>
			struct rebind
			{
				using other = bc_pool_allocator<TOther>;
			};

			bc_pool_allocator() noexcept = default;

			bc_pool_allocator(core::bc_concurrent_memory_pool& p_pool)
				: m_pool(&p_pool)
			{
			}

			bc_pool_allocator(const bc_pool_allocator&) noexcept = default;

			template<typename TOther>
			bc_pool_allocator(const bc_pool_allocator<TOther>& p_other) noexcept
				: m_pool(p_other.m_pool)
			{
			}

			bc_pool_allocator(bc_pool_allocator&&) noexcept = default;

			~bc_pool_allocator() = default;

			bc_pool_allocator& operator=(const bc_pool_allocator&) noexcept = default;

			template<typename TOther>
			bc_pool_allocator& operator=(const bc_pool_allocator<TOther>& p_other) noexcept
			{
				m_pool = p_other.m_pool;
				return *this;
			}

			bc_pool_allocator& operator=(bc_pool_allocator&&) noexcept = default;

			pointer allocate(const size_t p_count, const void*) noexcept
			{
				auto* l_pointer = static_cast<pointer>(m_pool->alloc());
				return l_pointer;
			}

			void deallocate(pointer p_pointer, size_type n = 0) noexcept
			{
				return m_pool->free(p_pointer);
			}

			core::bc_concurrent_memory_pool* m_pool;
		};

		struct bc_queue_test_entry
		{
			unsigned m_int;
			bcBYTE m_padding[64];
		};

		TEST(ConcurrentQueue, MultiwriterSinglereader)
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

		TEST(ConcurrentQueue, MultiwriterMultireader)
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

		TEST(ConcurrentQueue, ConcurrentMultiwriterMultireader)
		{
			bc_test_init();

			{
				constexpr auto l_push_thread_count = 4U;
				constexpr auto l_pop_thread_count = 4U;
				constexpr auto l_thread_item_count = 10000U;

				//core::bc_concurrent_memory_pool l_pool;
				//l_pool.initialize(l_thread_item_count * l_push_thread_count + 1, sizeof(core::bc_concurrent_queue<bc_queue_test_entry>::node_type), core::bc_alloc_type::program);

				core::bc_stop_watch l_watch;
				l_watch.start();

				core::bc_concurrent_queue1<bc_queue_test_entry, bc_test_allocator<bc_queue_test_entry>> l_queue;
				//const bc_pool_allocator<bc_queue_test_entry> l_allocator(l_pool);
				//core::bc_concurrent_queue<bc_queue_test_entry, bc_pool_allocator<bc_queue_test_entry>> l_queue(l_allocator);
				//moodycamel::ConcurrentQueue<bc_queue_test_entry> l_moody_queue;

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
								l_started_threads_count.fetch_add(1, std::memory_order_relaxed);

								while (!l_start_work.load(std::memory_order_relaxed))
								{
								}

								for (auto l_ite = 0U; l_ite < l_thread_item_count; ++l_ite)
								{
									l_queue.push({ l_ite });
									//l_moody_queue.enqueue({ l_ite });
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
								l_started_threads_count.fetch_add(1, std::memory_order_relaxed);

								while (!l_start_work.load(std::memory_order_relaxed))
								{
								}

								bool l_pop = false;
								bc_queue_test_entry l_pop_item;

								while (l_start_work.load(std::memory_order_relaxed) || ((l_pop = l_queue.pop(l_pop_item))))
								//while (l_start_work.load(std::memory_order_relaxed) || ((l_pop = l_moody_queue.try_dequeue(l_pop_item))))
								{
									if(l_pop)
									{
										l_work_count.fetch_add(1, std::memory_order_relaxed);
									}
								}
							}
						)
					);
				}

				while (l_started_threads_count.load(std::memory_order_relaxed) != l_push_thread_count + l_pop_thread_count)
				{
				}

				l_start_work.exchange(true, std::memory_order_relaxed);

				for (auto& l_thread : l_push_threads)
				{
					l_thread.join();
				}

				l_start_work.exchange(false, std::memory_order_relaxed);

				for (auto& l_thread : l_pop_threads)
				{
					l_thread.join();
				}

				l_watch.stop();

				std::cout << "elapsed milliseconds: " << l_watch.total_elapsed() << std::endl;
				std::cout << "expected output value is " << l_push_thread_count * l_thread_item_count << std::endl;
				std::cout << "output value is " << l_work_count << std::endl;
				//std::cin.get();

				EXPECT_TRUE(l_work_count == l_push_thread_count * l_thread_item_count);
			}

			bc_test_close();
		}
	}
}