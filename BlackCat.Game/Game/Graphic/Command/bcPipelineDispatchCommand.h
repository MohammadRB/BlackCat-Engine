// [03/23/2016 MRB]

#pragma once

#include "Core/Utility/bcParameterPack.h"
#include "Graphic/bcGraphicDefinition.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "Game/bcExport.h"
#include "Game/Graphic/bcRenderCommand.h"

namespace black_cat
{
	namespace game
	{
		struct bc_dispatch_command_parameters
		{
		public:
			bc_dispatch_command_parameters(bcUINT p_x, bcUINT p_y, bcUINT p_z)
				: m_x(p_x),
				m_y(p_y),
				m_z(p_z)
			{
			}

			bcUINT m_x;
			bcUINT m_y;
			bcUINT m_z;
		};

		class BC_GAME_DLL bc_pipeline_dispatch_command : public bc_irender_command
		{
		public:
			bc_pipeline_dispatch_command() = default;

			virtual ~bc_pipeline_dispatch_command() = default;

			void execute(graphic::bc_device_pipeline* p_pipeline, core::bc_parameter_pack&& p_parameters) override;

			void execute(graphic::bc_device_pipeline* p_pipeline, bcUINT m_x, bcUINT m_y, bcUINT m_z);

		protected:

		private:
		};
	}
}
