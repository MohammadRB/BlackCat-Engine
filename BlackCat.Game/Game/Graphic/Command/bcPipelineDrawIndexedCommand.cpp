// [02/26/2016 MRB]

#include "Game/GamePCH.h"
#include "Game/Graphic/Command/bcPipelineDrawIndexedCommand.h"

namespace black_cat
{
	namespace game
	{
		bc_pipeline_draw_indexed_command::bc_pipeline_draw_indexed_command(graphic::bc_primitive p_primitive,
			graphic::bc_format p_index_format,
			graphic::bc_buffer_ptr p_vertex_buffer,
			graphic::bc_buffer_ptr p_index_buffer)
			: bc_pipeline_draw_command(p_primitive, p_vertex_buffer),
			m_index_buffer(p_index_buffer),
			m_index_format(p_index_format)
		{
		}

		void bc_pipeline_draw_indexed_command::execute(graphic::bc_device_pipeline* p_pipeline, core::bc_parameter_pack&& p_parameters)
		{
			bcAssert(p_parameters.has_value());

			bc_draw_indexed_command_parameters* l_parameters = p_parameters.as<bc_draw_indexed_command_parameters>();
			
			execute(p_pipeline, l_parameters->m_start_index, l_parameters->m_index_count, l_parameters->m_vertex_offset);
		}

		void bc_pipeline_draw_indexed_command::execute(graphic::bc_device_pipeline* p_pipeline, bcUINT p_start_index, bcUINT p_index_count, bcINT p_vertex_offset)
		{
			p_pipeline->pipeline_set_default_states(graphic::bc_pipeline_stage::input_assembler_stage);

			bcUINT32 l_strides = m_vertex_buffer->get_structure_byte_stride();
			bcUINT32 l_offsets = 0;

			p_pipeline->bind_ia_primitive_topology(m_primitive);
			p_pipeline->bind_ia_vertex_buffers(0, 1, m_vertex_buffer.get(), &l_strides, &l_offsets);
			p_pipeline->bind_ia_index_buffer(m_index_buffer.get(), m_index_format);

			p_pipeline->pipeline_apply_states(graphic::bc_pipeline_stage::input_assembler_stage);

			p_pipeline->draw_indexed(p_start_index, p_index_count, p_vertex_offset);
		}
	}
}