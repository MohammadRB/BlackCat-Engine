 // [03/13/2016 MRB]

#include "Game/GamePCH.h"
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

		bc_irender_pass* bc_render_pass_manager::get_pass(bcUINT p_location)
		{
			bcAssert(p_location < m_passes.size());

			if (p_location >= m_passes.size())
				return nullptr;

			auto l_location = std::begin(m_passes) + p_location;

			return (*l_location).get();
		}

		bc_irender_pass* bc_render_pass_manager::get_pass(core::bc_string p_name)
		{
			for (auto& l_pass : m_passes)
			{
				if (l_pass->get_name() == p_name)
				{
					return l_pass.get();
				}
			}

			return nullptr;
		}

		void bc_render_pass_manager::add_pass(bcUINT p_location, core::bc_unique_ptr<bc_irender_pass>&& p_pass)
		{
			bcAssert(p_location <= m_passes.size());

			p_location = (std::min)(m_passes.size(), p_location);
			auto l_location = std::begin(m_passes) + p_location;

			m_passes.insert(l_location, std::move(p_pass));
		}

		bool bc_render_pass_manager::remove_pass(bcUINT p_location)
		{
			bcAssert(p_location < m_passes.size());

			if (p_location >= m_passes.size())
				return false;

			auto l_location = std::begin(m_passes) + p_location;

			m_passes.erase(l_location);

			return true;
		}

		bool bc_render_pass_manager::remove_pass(core::bc_string p_name)
		{
			bool l_found = false;
			bcUINT l_location = 0;

			for(auto& l_pass : m_passes)
			{
				if (l_pass->get_name() == p_name)
				{
					l_found = true;
					break;
				}
				
				l_location++;
			}

			if(l_found)
				return remove_pass(l_location);

			return false;
		}

		void bc_render_pass_manager::pass_initialize_resources(bc_render_system& p_render_system, graphic::bc_device* p_device)
		{
			for (auto& l_pass : m_passes)
			{
				l_pass->initialize_resources(p_render_system, p_device);
			}
		}

		void bc_render_pass_manager::pass_update(core_platform::bc_clock::update_param p_clock_update_param)
		{
			for (auto& l_pass : m_passes)
			{
				l_pass->update(p_clock_update_param);
			}
		}

		void bc_render_pass_manager::pass_initialize_frame(bc_render_system& p_render_system, graphic::bc_device_pipeline* p_pipeline, graphic::bc_device_command_executer* p_executer)
		{
			for (auto& l_pass : m_passes)
			{
				l_pass->initialize_frame(p_render_system, p_pipeline, p_executer);
			}
		}

		void bc_render_pass_manager::pass_execute(bc_render_system& p_render_system, graphic::bc_device_pipeline* p_pipeline, graphic::bc_device_command_executer* p_executer)
		{
			for(auto& l_pass : m_passes)
			{
				l_pass->execute(p_render_system, p_pipeline, p_executer);
			}
		}

		void bc_render_pass_manager::before_reset(bc_render_system& p_render_system, graphic::bc_device* p_device, graphic::bc_device_parameters& p_old_parameters, graphic::bc_device_parameters& p_new_parameters)
		{
			for (auto& l_pass : m_passes)
			{
				l_pass->before_reset(p_render_system, p_device, p_old_parameters, p_new_parameters);
			}
		}

		void bc_render_pass_manager::after_reset(bc_render_system& p_render_system, graphic::bc_device* p_device, graphic::bc_device_parameters& p_old_parameters, graphic::bc_device_parameters& p_new_parameters)
		{
			for (auto& l_pass : m_passes)
			{
				l_pass->after_reset(p_render_system, p_device, p_old_parameters, p_new_parameters);
			}
		}

		void bc_render_pass_manager::pass_destroy(graphic::bc_device* p_device)
		{
			for (auto& l_pass : m_passes)
			{
				l_pass->destroy(p_device);
			}
		}
	}
}