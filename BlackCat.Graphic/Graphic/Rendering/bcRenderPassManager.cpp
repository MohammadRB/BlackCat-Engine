 // [03/13/2016 MRB]

#include "Graphic/GraphicPCH.h"
#include "Graphic/Rendering/bcRenderPassManager.h"

namespace black_cat
{
	namespace graphic
	{
		bc_render_pass_manager::bc_render_pass_manager()
		{
			// Force vector to allocate memory so when we call begin on it we get valid iterator
			m_passes.reserve(1);
		}

		bc_render_pass_manager::~bc_render_pass_manager()
		{
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

		void bc_render_pass_manager::pass_initialize_resources(bc_device* p_device)
		{
			for (auto& l_pass : m_passes)
			{
				l_pass->initialize_resources(p_device);
			}
		}

		void bc_render_pass_manager::pass_update(bcFLOAT32 p_time_elapsed)
		{
			for (auto& l_pass : m_passes)
			{
				l_pass->update(p_time_elapsed);
			}
		}

		void bc_render_pass_manager::pass_initialize_frame(bc_device_command_executer* p_executer, bc_device_pipeline* p_pipeline)
		{
			for (auto& l_pass : m_passes)
			{
				l_pass->initialize_frame(p_executer, p_pipeline);
			}
		}

		void bc_render_pass_manager::pass_execute(bc_device_command_executer* p_executer, bc_device_pipeline* p_pipeline)
		{
			for(auto& l_pass : m_passes)
			{
				l_pass->execute(p_executer, p_pipeline);
			}
		}

		void bc_render_pass_manager::pass_destroy(bc_device* p_device)
		{
			for (auto& l_pass : m_passes)
			{
				l_pass->destroy(p_device);
			}
		}
	}
}