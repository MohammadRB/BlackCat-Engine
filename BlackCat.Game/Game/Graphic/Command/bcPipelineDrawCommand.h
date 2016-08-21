// [02/24/2016 MRB]

#pragma once

#include "Core/Utility/bcParameterPack.h"
#include "Graphic/bcGraphicDefinition.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "Game/bcExport.h"
#include "Game/Graphic/bcRenderCommand.h"

namespace black_cat
{
	namespace game
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
		
		class BC_GAME_DLL bc_pipeline_draw_command : public bc_irender_command
		{
		public:
			bc_pipeline_draw_command(graphic::bc_primitive p_primitive, graphic::bc_buffer_ptr p_vertex_buffer);

			virtual ~bc_pipeline_draw_command() = default;

			void execute(graphic::bc_device_pipeline* p_pipeline, core::bc_parameter_pack&& p_parameters) override;

			void execute(graphic::bc_device_pipeline* p_pipeline, bcUINT p_start_vertex, bcUINT p_vertex_count);

		protected:
			graphic::bc_primitive m_primitive;
			graphic::bc_buffer_ptr m_vertex_buffer;

		private:
		};
	}
}
