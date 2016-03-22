// [02/24/2016 MRB]

#pragma once

#include "Core/Utility/bcParameterPack.h"
#include "Graphic/bcGraphicDefinition.h"
#include "Graphic/Rendering/bcRenderCommand.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"

namespace black_cat
{
	namespace graphic
	{
		struct bc_draw_command_parameters
		{
		public:
			bc_draw_command_parameters(bcUINT p_start_vertex, bcUINT p_vertex_count) 
				: m_start_vertex(p_start_vertex),
				m_vertex_count(p_vertex_count)
			{
			}

			bcUINT m_start_vertex;
			bcUINT m_vertex_count;
		};
		
		class bc_pipeline_draw_command : public bc_irender_command
		{
		public:
			bc_pipeline_draw_command(bc_primitive p_primitive, bc_buffer_ptr p_vertex_buffer);

			virtual ~bc_pipeline_draw_command() = default;

			void execute(bc_device_pipeline* p_pipeline, core::bc_parameter_pack&& p_parameters) override;

		protected:
			bc_primitive m_primitive;
			bc_buffer_ptr m_vertex_buffer;

		private:
		};
	}
}
