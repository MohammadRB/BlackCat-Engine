 // [03/13/2016 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcRenderPassManager.h"

namespace black_cat
{
	namespace game
	{
		bc_render_pass_manager::bc_render_pass_manager()
		{
			// Force vector to allocate memory so when we call begin on it we get valid iterator
			m_passes.reserve(1);
		}

		bc_render_pass_manager::~bc_render_pass_manager()
		{
		}

		bc_irender_pass* bc_render_pass_manager::get_pass(bcUINT32 p_location)
		{
			for (auto& l_entry : m_passes)
			{
				if (l_entry.m_position == p_location)
				{
					return l_entry.m_pass.get();
				}
			}

			return nullptr;
		}

		bool bc_render_pass_manager::remove_pass(bcUINT32 p_location)
		{
			bool l_found = false;
			bcUINT32 l_location = 0;

			for (auto& l_entry : m_passes)
			{
				if (l_entry.m_position == p_location)
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

		void bc_render_pass_manager::pass_initialize_resources(bc_render_system& p_render_system)
		{
			for (auto& l_entry : m_passes)
			{
				l_entry.m_pass->initialize_resources(p_render_system);
			}
		}

		void bc_render_pass_manager::pass_update(const bc_render_system_update_param& p_clock_update_param)
		{
			for (auto& l_entry : m_passes)
			{
				l_entry.m_pass->update(p_clock_update_param);
			}
		}

		void bc_render_pass_manager::pass_execute(bc_render_system& p_render_system, bc_scene& p_scene, bc_render_thread& p_thread)
		{
			for(auto& l_entry : m_passes)
			{
				l_entry.m_pass->initialize_frame(p_render_system, p_scene, p_thread);
				l_entry.m_pass->execute(p_render_system, p_scene, p_thread);
			}
		}

		void bc_render_pass_manager::before_reset(bc_render_system& p_render_system, graphic::bc_device& p_device, graphic::bc_device_parameters& p_old_parameters, graphic::bc_device_parameters& p_new_parameters)
		{
			for (auto& l_entry : m_passes)
			{
				l_entry.m_pass->before_reset(p_render_system, p_device, p_old_parameters, p_new_parameters);
			}
		}

		void bc_render_pass_manager::after_reset(bc_render_system& p_render_system, graphic::bc_device& p_device, graphic::bc_device_parameters& p_old_parameters, graphic::bc_device_parameters& p_new_parameters)
		{
			for (auto& l_entry : m_passes)
			{
				l_entry.m_pass->after_reset(p_render_system, p_device, p_old_parameters, p_new_parameters);
			}
		}

		void bc_render_pass_manager::pass_destroy(graphic::bc_device& p_device)
		{
			for (auto& l_entry : m_passes)
			{
				l_entry.m_pass->destroy(p_device);
			}
		}

		void bc_render_pass_manager::_add_pass(_bc_pass_entry&& p_entry)
		{
			p_entry.m_pass->_set_pass_resource_share(&m_state_share);

			m_passes.push_back(std::move(p_entry));

			std::sort(std::begin(m_passes), std::end(m_passes), [](_bc_pass_entry& p_first, _bc_pass_entry& p_second)
			{
				return p_first.m_position < p_second.m_position;
			});
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

		bc_irender_pass* bc_render_pass_manager::_get_pass(const bcCHAR* p_name)
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