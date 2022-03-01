// [06/13/2021 MRB]

#pragma once

#include "Core/Memory/bcMemoryManagment.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcStopWatch.h"

namespace black_cat
{
	inline void bc_test_init_memory()
	{
		core::bc_memory_manager::startup(8, 16, 10, 16, 128, 1024, 1024, 1024);
	}

	inline void bc_test_close_memory()
	{
		core::bc_memory_manager::close();
	}

	inline void bc_test_init()
	{
		bc_test_init_memory();
		core::bc_service_manager::start_up();
	}

	inline void bc_test_close()
	{
		core::bc_service_manager::close();
		bc_test_close_memory();
	}

	template<typename TProducer, typename TConsumer>
	bcFLOAT bc_multi_producer_multi_consumer_test(bcUINT32 p_producer_count, bcUINT32 p_consumer_count, TProducer p_producer, TConsumer p_consumer)
	{
		core::bc_stop_watch l_watch;
		std::vector<std::thread> l_producer_threads;
		std::vector<std::thread> l_consumer_threads;
		std::atomic<unsigned> l_started_threads_count(0);
		std::atomic<bool> l_start_work(false);

		for (auto l_thread_ite = 0U; l_thread_ite < p_producer_count; ++l_thread_ite)
		{
			l_producer_threads.push_back
			(
				std::thread
				(
					[&]()
					{
						l_started_threads_count.fetch_add(1, std::memory_order_relaxed);

						while (!l_start_work.load(std::memory_order_relaxed))
						{
						}

						p_producer();
					}
				)
			);
		}

		for (auto l_thread_ite = 0U; l_thread_ite < p_consumer_count; ++l_thread_ite)
		{
			l_consumer_threads.push_back
			(
				std::thread
				(
					[&]()
					{
						l_started_threads_count.fetch_add(1, std::memory_order_relaxed);

						while (!l_start_work.load(std::memory_order_relaxed))
						{
						}

						p_consumer();
					}
				)
			);
		}

		while (l_started_threads_count.load(std::memory_order_relaxed) != p_producer_count + p_consumer_count)
		{
		}

		l_watch.start();

		l_start_work.exchange(true, std::memory_order_relaxed);

		for (auto& l_thread : l_producer_threads)
		{
			l_thread.join();
		}

		l_start_work.exchange(false, std::memory_order_relaxed);

		for (auto& l_thread : l_consumer_threads)
		{
			l_thread.join();
		}

		l_watch.stop();

		return l_watch.restart();
	}
}
