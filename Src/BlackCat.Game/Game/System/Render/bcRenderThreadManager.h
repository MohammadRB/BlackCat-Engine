// [02/23/2017 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "CorePlatformImp/Concurrency/bcConditionVariable.h"
#include "Core/Container/bcVector.h"
#include "Game/bcExport.h"
#include "Game/System/Render/bcRenderThread.h"

namespace black_cat
{
	namespace game
	{
		class bc_render_system;
		class bc_render_thread_manager;

		class _bc_render_thread_entry
		{
		public:
			_bc_render_thread_entry();
			 
			_bc_render_thread_entry(_bc_render_thread_entry&&) = default;

			~_bc_render_thread_entry() = default;

			_bc_render_thread_entry& operator=(_bc_render_thread_entry&&) = default;

			bc_render_thread m_thread;
			bool m_is_available;
		};

		/**
		 * \brief RAII class for render threads
		 */
		class bc_render_thread_guard
		{
		public:
			bc_render_thread_guard(bc_render_thread_manager& p_thread_manager, bc_render_thread* p_thread);

			bc_render_thread_guard(bc_render_thread_guard&&) noexcept;

			~bc_render_thread_guard();

			bc_render_thread_guard& operator=(bc_render_thread_guard&&) noexcept;

			bool is_valid() const;

			bc_render_thread* get_thread() const;

		protected:

		private:
			bc_render_thread_manager& m_thread_manager;
			bc_render_thread* m_thread;
		};

		class BC_GAME_DLL bc_render_thread_manager
		{
		public:
			bc_render_thread_manager(bc_render_system& p_render_system, bcUINT32 p_thread_count);

			bc_render_thread_manager(bc_render_thread_manager&&) noexcept;

			~bc_render_thread_manager();

			bc_render_thread_manager& operator=(bc_render_thread_manager&&) noexcept;
			
			bcUINT32 get_thread_count() const;

			bcUINT32 get_available_thread_count() const;

			/**
			 * \brief Try to get a render thread or if there is no thread available return invalid wrapper
			 * \ThreadSafe
			 * \return null if there is no thread available
			 */
			bc_render_thread_guard get_available_thread() const;

			/**
			 * \brief Get a render thread or if there is no thread available wait until one become available
			 * \ThreadSafe
			 * \return First thread that is free
			 */
			bc_render_thread_guard get_available_thread_wait() const;

			/**
			 * \brief Give back a render thread to set it as available thread
			 * \ThreadSafe
			 * \param p_thread 
			 */
			void set_available_thread(bc_render_thread& p_thread);

		protected:

		private:
			mutable core_platform::bc_mutex m_threads_mutex;
			mutable core_platform::bc_condition_variable m_threads_cv;
			core::bc_vector<_bc_render_thread_entry> m_threads;
			mutable core_platform::bc_atomic<bcINT32> m_available_thread_count;
		};
	}
}