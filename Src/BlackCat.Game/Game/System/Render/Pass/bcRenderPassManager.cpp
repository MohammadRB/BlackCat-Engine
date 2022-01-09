 // [03/13/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/Utility/bcCounterValueManager.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/Pass/bcRenderPassManager.h"
#include "Game/bcException.h"

namespace black_cat
{
	namespace game
	{
		bc_render_pass_manager::bc_render_pass_manager(graphic::bc_device& p_device) noexcept
			: m_texture_manager(p_device)
		{
			// Force vector to allocate memory so when we call begin on it we get valid iterator
			m_passes.reserve(1);
		}

		bc_render_pass_manager::bc_render_pass_manager(bc_render_pass_manager&& p_other) noexcept
			: m_passes(std::move(p_other.m_passes)),
			m_resource_manager(std::move(p_other.m_resource_manager)),
			m_texture_manager(std::move(p_other.m_texture_manager))
		{
			
		}

		bc_render_pass_manager::~bc_render_pass_manager() = default;

		bc_render_pass_manager& bc_render_pass_manager::operator=(bc_render_pass_manager&& p_other) noexcept
		{
			m_passes = std::move(p_other.m_passes);
			m_resource_manager = std::move(p_other.m_resource_manager);
			m_texture_manager = std::move(p_other.m_texture_manager);
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
			
			for (const _bc_render_pass_entry& l_entry : m_passes)
			{
				l_entry.m_stop_watch->start();
				l_entry.m_pass->initialize_frame(p_context);
				l_entry.m_stop_watch->stop();
			}
			
			for(const _bc_render_pass_entry& l_entry : m_passes)
			{
				l_entry.m_stop_watch->start();
				l_entry.m_pass->execute(p_context);
				l_entry.m_stop_watch->stop();
			}

			for (const _bc_render_pass_entry& l_entry : m_passes)
			{
				l_entry.m_stop_watch->start();
				l_entry.m_pass->cleanup_frame(p_context);
				l_entry.m_stop_watch->stop();

				l_entry.m_stop_watch->restart();
				l_counter_value_manager->add_counter(l_entry.m_name, l_entry.m_stop_watch->average_total_elapsed());
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

		void bc_render_pass_manager::_add_pass(const bcCHAR* p_name, core::bc_unique_ptr<bci_render_pass> p_pass, const bcCHAR* p_before)
		{
			_bc_render_pass_entry l_entry;
			l_entry.m_name = p_name;
			l_entry.m_pass = std::move(p_pass);
			l_entry.m_stop_watch = core::bc_make_unique<core::bc_stop_watch>();
			l_entry.m_pass->_set_private_fields(m_resource_manager, m_texture_manager);

			if(p_before == nullptr)
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
			bcUINT32 l_location = 0;

			for (auto& l_entry : m_passes)
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
				m_passes.erase(std::next(std::cbegin(m_passes), l_location));
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