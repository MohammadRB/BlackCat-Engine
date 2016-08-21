// [02/26/2016 MRB]

#pragma once

#include "Core/Utility/bcParameterPack.h"
#include "Graphic/bcGraphicDefinition.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "Game/bcExport.h"
#include "Game/Graphic/Command/bcPipelineDrawCommand.h"

namespace black_cat
{
	namespace game
	{
		struct bc_draw_indexed_command_parameters
		{
		public:
			bc_draw_indexed_command_parameters(bcUINT p_start_index,
			bcUINT p_index_count,
			bcINT p_vertex_offset)
				: m_start_index(p_start_index),
				m_index_count(p_index_count),
				m_vertex_offset(p_vertex_offset)
			{
			}

			bcUINT m_start_index;
			bcUINT m_index_count; 
			bcINT m_vertex_offset;
		};


		class BC_GAME_DLL bc_pipeline_draw_indexed_command : public bc_pipeline_draw_command
		{
		public:
			bc_pipeline_draw_indexed_command(graphic::bc_primitive p_primitive,
				graphic::bc_format p_index_format,
				graphic::bc_buffer_ptr p_vertex_buffer, 
				graphic::bc_buffer_ptr p_index_buffer);

			~bc_pipeline_draw_indexed_command() = default;

			void execute(graphic::bc_device_pipeline* p_pipeline, core::bc_parameter_pack&& p_parameters) override;

			void execute(graphic::bc_device_pipeline* p_pipeline, bcUINT p_start_index, bcUINT p_index_count, bcINT p_vertex_offset);

		protected:
			graphic::bc_buffer_ptr m_index_buffer;
			graphic::bc_format m_index_format;

		private:
		};
	}
}
