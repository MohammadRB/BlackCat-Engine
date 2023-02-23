 // [03/13/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/Concurrency/bcConcurrency.h"
#include "Core/Container/bcStringStream.h"
#include "Core/Utility/bcCounterValueManager.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcRenderTask.h"
#include "Game/System/Render/bcDefaultRenderThread.h"
#include "Game/System/Render/Pass/bcRenderPassManager.h"

namespace black_cat
{
	namespace game
	{
		class bc_concurrent_render_pass_task : public bci_render_task
		{
		public:
			bc_concurrent_render_pass_task(_bc_render_pass_entry& p_pass, const bc_render_pass_render_context& p_context)
				: m_pass(&p_pass),
				m_context(&p_context)
			{
			}

			_bc_render_pass_entry& get_pass() const noexcept
			{
				return *m_pass;
			}

			void execute(bc_render_system& p_render_system, bc_render_thread& p_render_thread) override
			{
				auto* l_pass = static_cast<bci_concurrent_render_pass*>(m_pass->m_pass.get());
				const auto l_context = bc_concurrent_render_pass_render_context(*m_context, p_render_thread);

				m_pass->m_stop_watch.start();
				l_pass->initialize_frame(l_context);
				l_pass->execute(l_context);
				m_pass->m_stop_watch.stop();
			}

		private:
			_bc_render_pass_entry* m_pass;
			const bc_render_pass_render_context* m_context;
		};

		bc_render_pass_manager::bc_render_pass_manager(bc_render_system& p_render_system) noexcept
			: m_render_system(&p_render_system),
			m_texture_manager(m_render_system->get_device()),
			m_all_queries_fetched(true)
		{
			// Force vector to allocate memory so when we call begin on it we get valid iterator
			m_passes.reserve(20);
			m_device_start_query = _bc_render_pass_entry::device_query({ m_render_system->get_device().create_timestamp_query() });
		}

		bc_render_pass_manager::bc_render_pass_manager(bc_render_pass_manager&& p_other) noexcept
			: m_render_system(p_other.m_render_system),
			m_passes(std::move(p_other.m_passes)),
			m_resource_manager(std::move(p_other.m_resource_manager)),
			m_texture_manager(std::move(p_other.m_texture_manager)),
			m_device_start_query(std::move(p_other.m_device_start_query)),
			m_all_queries_fetched(p_other.m_all_queries_fetched)
		{
		}

		bc_render_pass_manager::~bc_render_pass_manager() = default;

		bc_render_pass_manager& bc_render_pass_manager::operator=(bc_render_pass_manager&& p_other) noexcept
		{
			m_render_system = p_other.m_render_system;
			m_passes = std::move(p_other.m_passes);
			m_resource_manager = std::move(p_other.m_resource_manager);
			m_texture_manager = std::move(p_other.m_texture_manager);
			m_device_start_query = std::move(p_other.m_device_start_query);
			m_all_queries_fetched = p_other.m_all_queries_fetched;
			return *this;
		}

		void bc_render_pass_manager::pass_initialize_resources(bc_render_system& p_render_system)
		{
			for (const auto& l_entry : m_passes)
			{
				l_entry.m_pass->initialize_resources(p_render_system);
			}
		}

		void bc_render_pass_manager::pass_update(const bc_render_pass_update_context& p_context)
		{
			for (const auto& l_entry : m_passes)
			{
				l_entry.m_pass->update(p_context);
			}
		}

		void bc_render_pass_manager::pass_execute(const bc_render_pass_render_context& p_context)
		{
			auto* l_counter_value_manager = core::bc_get_service<core::bc_counter_value_manager>();
			auto l_string_stream = core::bc_wstring_stream();

			auto l_fetched_query_count = 0U;
			auto l_device_query_results = core::bc_vector_frame<bcUINT64>();
			auto l_concurrent_tasks = core::bc_vector_frame<bc_concurrent_render_pass_task>();
			l_device_query_results.reserve(m_passes.size() + 1);
			l_concurrent_tasks.reserve(5);

			const auto l_fetch_query_data = [&](_bc_render_pass_entry::device_query& p_query)
			{
				p_context.m_render_thread.start();

				if (m_all_queries_fetched)
				{
					p_query.end(p_context.m_render_thread);
				}

				l_fetched_query_count += p_query.get_data(p_context.m_render_thread).first;
				l_device_query_results.push_back(p_query.get_last_data());

				p_context.m_render_thread.finish();
			};

			l_fetch_query_data(m_device_start_query);

			for (auto& l_pass : m_passes)
			{
				if (!l_pass.m_is_concurrent)
				{
					if (!l_concurrent_tasks.empty())
					{
						core::bc_concurrency::when_all(std::begin(l_concurrent_tasks), std::end(l_concurrent_tasks));

						// Use last pass device query to estimate elapsed time of all concurrent passes together
						auto& l_last_pass = l_concurrent_tasks.back().get_pass();

						l_fetch_query_data(l_last_pass.m_device_query);

						l_concurrent_tasks.clear();
					}
					
					l_pass.m_stop_watch.start();
					l_pass.m_pass->initialize_frame(p_context);
					l_pass.m_pass->execute(p_context);
					l_pass.m_stop_watch.stop();

					l_fetch_query_data(l_pass.m_device_query);
				}
				else
				{
					l_concurrent_tasks.push_back(bc_concurrent_render_pass_task(l_pass, p_context));
					p_context.m_render_system.add_render_task(l_concurrent_tasks.back());
				}
			}

			m_all_queries_fetched = l_fetched_query_count == l_device_query_results.size();

			auto l_device_query_ite = 0;
			for (auto l_ite = 0U; l_ite < m_passes.size(); ++l_ite)
			{
				auto& l_pass = m_passes[l_ite];

				l_pass.m_stop_watch.start();
				l_pass.m_pass->cleanup_frame(p_context);
				l_pass.m_stop_watch.stop();
				l_pass.m_stop_watch.restart();

				if(l_pass.m_is_concurrent)
				{
					const auto& l_next_pass = m_passes[l_ite + 1];
					if(l_next_pass.m_is_concurrent)
					{
						// Only device query of last concurrent pass is used
						l_string_stream.str(core::bc_wstring());
						l_string_stream.clear();
						l_string_stream << L"C " << core::bc_to_wstring_frame(l_pass.m_stop_watch.average_total_elapsed(), L"%.1f");

						l_counter_value_manager->add_counter(l_pass.m_name, l_string_stream.str());
						continue;
					}
				}

				if(m_all_queries_fetched)
				{
					const auto l_previous_pass_device_timestamp = l_device_query_results[l_device_query_ite];
					const auto l_current_pass_device_timestamp = l_device_query_results[l_device_query_ite + 1];
					const auto l_pass_device_clocks = l_current_pass_device_timestamp - l_previous_pass_device_timestamp;
					const auto l_pass_device_elapsed = static_cast<platform::bc_clock::small_time>(l_pass_device_clocks) / p_context.m_render_system.get_device_clock() * 1000.f;
					l_pass.m_device_elapsed_sampler.add_sample(l_pass_device_elapsed);
				}

				l_string_stream.str(core::bc_wstring());
				l_string_stream.clear();
				l_string_stream << L"C " << core::bc_to_wstring_frame(l_pass.m_stop_watch.average_total_elapsed(), L"%.1f") << L" "
								<< L"G " << core::bc_to_wstring_frame(l_pass.m_device_elapsed_sampler.average_value(), L"%.1f");

				l_counter_value_manager->add_counter(l_pass.m_name, l_string_stream.str());
				++l_device_query_ite;
			}
		}

		void bc_render_pass_manager::before_reset(const bc_render_pass_reset_context& p_context)
		{
			for (const auto& l_entry : m_passes)
			{
				l_entry.m_pass->before_reset(p_context);
			}
			m_texture_manager.before_reset(p_context);
		}

		void bc_render_pass_manager::after_reset(const bc_render_pass_reset_context& p_context)
		{
			for (const auto& l_entry : m_passes)
			{
				l_entry.m_pass->after_reset(p_context);
			}
			m_texture_manager.after_reset(p_context);
		}

		void bc_render_pass_manager::config_changed(const bc_render_pass_config_change_context& p_context)
		{
			for (const auto& l_entry : m_passes)
			{
				l_entry.m_pass->config_changed(p_context);
			}
		}

		void bc_render_pass_manager::pass_destroy(bc_render_system& p_render_system)
		{
			for (const auto& l_entry : m_passes)
			{
				l_entry.m_pass->destroy(p_render_system);
			}
		}

		void bc_render_pass_manager::pass_clear()
		{
			m_passes.clear();
		}

		void bc_render_pass_manager::_add_pass(const bcCHAR* p_name, bool p_is_concurrent, core::bc_unique_ptr<bci_render_pass> p_pass, const bcCHAR* p_before)
		{
			_bc_render_pass_entry l_entry;
			l_entry.m_name = p_name;
			l_entry.m_is_concurrent = p_is_concurrent;
			l_entry.m_pass = std::move(p_pass);
			l_entry.m_pass->_set_private_fields(m_render_system->get_device(), m_render_system->get_device_swap_buffer(), m_resource_manager, m_texture_manager);
			l_entry.m_pass->initialize_resources(*m_render_system);
			l_entry.m_device_query = _bc_render_pass_entry::device_query({ m_render_system->get_device().create_timestamp_query() });

			if (p_before == nullptr)
			{
				m_passes.push_back(std::move(l_entry));
			}
			else
			{
				const auto l_before_ite = std::find_if
				(
					std::begin(m_passes),
					std::end(m_passes),
					[=](const _bc_render_pass_entry& p_pass)
					{
						return p_pass.m_name == p_before;
					}
				);

				BC_ASSERT(l_before_ite != std::end(m_passes));

				m_passes.insert(l_before_ite, std::move(l_entry));
			}
		}

		bool bc_render_pass_manager::_remove_pass(const bcCHAR* p_name)
		{
			bool l_found = false;
			bcINT32 l_location = 0;

			for (const auto& l_entry : m_passes)
			{
				if (std::strcmp(l_entry.m_name, p_name) == 0)
				{
					l_found = true;
					break;
				}

				l_location++;
			}

			if (l_found)
			{
				const auto l_pass_ite = std::next(std::cbegin(m_passes), l_location);
				l_pass_ite->m_pass->destroy(*m_render_system);
				m_passes.erase(l_pass_ite);
				return true;
			}

			return false;
		}

		bci_render_pass* bc_render_pass_manager::_get_pass(const bcCHAR* p_name)
		{
			for (auto& l_entry : m_passes)
			{
				if (std::strcmp(l_entry.m_name, p_name) == 0)
				{
					return l_entry.m_pass.get();
				}
			}

			return nullptr;
		}
	}
}