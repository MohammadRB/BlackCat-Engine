// [02/23/2017 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcRenderThreadManager.h"

namespace black_cat
{
	namespace game
	{
		_bc_render_thread_entry::_bc_render_thread_entry() noexcept
			: m_is_available(true)
		{
		}

		_bc_render_thread_entry::_bc_render_thread_entry(_bc_render_thread_entry&& p_other) noexcept = default;

		_bc_render_thread_entry::~_bc_render_thread_entry() noexcept = default;

		_bc_render_thread_entry& _bc_render_thread_entry::operator=(_bc_render_thread_entry&& p_other) noexcept = default;

		bc_render_thread_guard::bc_render_thread_guard(bc_render_thread_manager* p_thread_manager, bc_render_thread* p_thread)
			: m_thread_manager(p_thread_manager),
			m_thread(p_thread)
		{
		}

		bc_render_thread_guard::bc_render_thread_guard(bc_render_thread_guard&& p_other) noexcept
			: m_thread_manager(p_other.m_thread_manager),
			m_thread(p_other.m_thread)
		{
			p_other.m_thread_manager = nullptr;
			p_other.m_thread = nullptr;
		}

		bc_render_thread_guard::~bc_render_thread_guard()
		{
			if(is_valid())
			{
				m_thread_manager->set_available_thread(*m_thread);
			}
		}

		bc_render_thread_guard& bc_render_thread_guard::operator=(bc_render_thread_guard&& p_other) noexcept
		{
			m_thread_manager = p_other.m_thread_manager;
			m_thread = p_other.m_thread;
			p_other.m_thread_manager = nullptr;
			p_other.m_thread = nullptr;

			return *this;
		}

		bool bc_render_thread_guard::is_valid() const
		{
			return m_thread;
		}

		bc_render_thread* bc_render_thread_guard::get_thread() const
		{
			return m_thread;
		}

		bc_render_thread_manager::bc_render_thread_manager(bc_render_system& p_render_system, bcUINT32 p_thread_count)
			: m_available_threads_count(p_thread_count)
		{
			auto& l_device = p_render_system.get_device();
			
			m_default_render_thread.reset(l_device.get_default_pipeline());
			m_threads.reserve(p_thread_count);

			for(bcUINT32 i = 0; i < p_thread_count; ++i)
			{
				auto l_render_pipeline = l_device.create_pipeline();
				auto l_command_executor = l_device.create_command_executor();

				_bc_render_thread_entry l_entry;
				l_entry.m_thread.reset(std::move(l_render_pipeline), std::move(l_command_executor));

				m_threads.push_back(std::move(l_entry));
			}
		}

		bc_render_thread_manager::bc_render_thread_manager(bc_render_thread_manager&& p_other) noexcept
		{
			operator=(std::move(p_other));
		}

		bc_render_thread_manager::~bc_render_thread_manager() = default;

		bc_render_thread_manager& bc_render_thread_manager::operator=(bc_render_thread_manager&& p_other) noexcept
		{
			m_available_threads_count.store(p_other.m_available_threads_count.load());
			m_threads = std::move(p_other.m_threads);

			return *this;
		}

		bcUINT32 bc_render_thread_manager::get_thread_count() const noexcept
		{
			return m_threads.size();
		}

		bcUINT32 bc_render_thread_manager::get_available_thread_count() const noexcept
		{
			return m_available_threads_count.load(core_platform::bc_memory_order::relaxed);
		}

		bc_default_render_thread& bc_render_thread_manager::get_default_render_thread() noexcept
		{
			return m_default_render_thread;
		}

		bc_render_thread_guard bc_render_thread_manager::get_available_thread() const
		{
			{
				core_platform::bc_lock_guard< core_platform::bc_mutex > l_guard(m_threads_mutex);
				
				for (_bc_render_thread_entry& l_entry : m_threads)
				{
					if (!l_entry.m_is_available)
					{
						continue;
					}

					l_entry.m_is_available = false;
					const auto l_count = m_available_threads_count.fetch_sub(1, core_platform::bc_memory_order::relaxed) - 1;

					bcAssert(l_count >= 0);

					return bc_render_thread_guard(const_cast< bc_render_thread_manager* >(this), &l_entry.m_thread);
				}
			}

			return bc_render_thread_guard(nullptr, nullptr);
		}

		bc_render_thread_guard bc_render_thread_manager::get_available_thread_wait() const
		{
			auto l_thread = get_available_thread();

			if (l_thread.is_valid())
			{
				return l_thread;
			}

			{
				core_platform::bc_unique_lock< core_platform::bc_mutex > l_guard(m_threads_mutex);
				
				m_threads_cv.wait(l_guard, [this]()
				{
					return m_available_threads_count.load(core_platform::bc_memory_order::relaxed) > 0;
				});
			}

			return get_available_thread_wait();
		}

		void bc_render_thread_manager::set_available_thread(bc_render_thread& p_thread)
		{
			{
				core_platform::bc_lock_guard<core_platform::bc_mutex> l_guard(m_threads_mutex);
				
				for (_bc_render_thread_entry& l_entry : m_threads)
				{
					if(&l_entry.m_thread != &p_thread)
					{
						continue;
					}

					l_entry.m_is_available = true;
					const auto l_count = m_available_threads_count.fetch_add(1, core_platform::bc_memory_order::relaxed) + 1;

					bcAssert(l_count <= m_threads.size());

					m_threads_cv.notify_one();
				}
			}
		}
	}
}