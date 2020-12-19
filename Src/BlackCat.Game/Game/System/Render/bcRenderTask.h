// [01/04/2017 MRB]

#pragma once

#include "Core/Concurrency/bcThreadManager.h"
#include "Graphic/bcRenderApi.h"

namespace black_cat
{
	namespace game
	{
		class bc_render_system;
		class bc_render_thread;

		/**
		 * \brief Represent a rendering task that can execute commands on hardware device without bc_render_pass.
		 * Task will be executed on different cpu and render thread from the thread which has submitted task.
		 */
		class bc_irender_task
		{
		public:
			virtual ~bc_irender_task() = default;

			/**
			 * \brief Executor function.
			 * \n This function will be executed concurrent by a cpu worker thread.
			 * \param p_render_system 
			 * \param p_render_thread 
			 */
			virtual void execute(bc_render_system& p_render_system, bc_render_thread& p_render_thread) = 0;

			void wait() const
			{
				m_task.wait();
			}

			void _set_cpu_task(core::bc_task<void>&& p_task)
			{
				m_task = std::move(p_task);
			}

		protected:
			bc_irender_task() = default;

			bc_irender_task(bc_irender_task&&) = default;

			bc_irender_task& operator=(bc_irender_task&&) = default;

		private:
			core::bc_task<void> m_task;
		};
	}
}