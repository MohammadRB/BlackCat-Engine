// [03/23/2016 MRB]

#include "Game/GamePCH.h"
#include "Game/Graphic/Command/bcPipelineDispatchCommand.h"

namespace black_cat
{
	namespace game
	{
		void bc_pipeline_dispatch_command::execute(graphic::bc_device_pipeline* p_pipeline, core::bc_parameter_pack&& p_parameters)
		{
			bcAssert(p_parameters.has_value());

			bc_dispatch_command_parameters* l_parameters = p_parameters.as<bc_dispatch_command_parameters>();
			
			execute(p_pipeline, l_parameters->m_x, l_parameters->m_y, l_parameters->m_z);
		}

		void bc_pipeline_dispatch_command::execute(graphic::bc_device_pipeline* p_pipeline, bcUINT p_x, bcUINT p_y, bcUINT p_z)
		{
			p_pipeline->dispatch(p_x, p_y, p_z);
		}
	}
}