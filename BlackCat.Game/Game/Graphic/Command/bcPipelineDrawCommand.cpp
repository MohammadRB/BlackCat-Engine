// [02/24/2016 MRB]

#include "Game/GamePCH.h"
#include "Game/Graphic/Command/bcPipelineDrawCommand.h"

namespace black_cat
{
	namespace game
	{
		bc_pipeline_draw_command::bc_pipeline_draw_command(graphic::bc_primitive p_primitive, graphic::bc_buffer_ptr p_vertex_buffer)
			: m_primitive(p_primitive),
			m_vertex_buffer(p_vertex_buffer) 
		{
		}

		void bc_pipeline_draw_command::execute(graphic::bc_device_pipeline* p_pipeline, core::bc_parameter_pack&& p_parameters)
		{
			bcAssert(p_parameters.has_value());

			bc_draw_command_parameters* l_parameters = p_parameters.as<bc_draw_command_parameters>();

			execute(p_pipeline, l_parameters->m_start_vertex, l_parameters->m_vertex_count);
		}

		void bc_pipeline_draw_command::execute(graphic::bc_device_pipeline* p_pipeline, bcUINT p_start_vertex, bcUINT p_vertex_count)
		{
			// TODO it remove all states that some of them are required
			p_pipeline->pipeline_set_default_states(graphic::bc_pipeline_stage::input_assembler_stage);

			bcUINT32 l_strides = m_vertex_buffer->get_structure_byte_stride();
			bcUINT32 l_offsets = 0;

			p_pipeline->bind_ia_primitive_topology(m_primitive);
			p_pipeline->bind_ia_vertex_buffers(0, 1, m_vertex_buffer.get(), &l_strides, &l_offsets);

			p_pipeline->pipeline_apply_states(graphic::bc_pipeline_stage::input_assembler_stage);

			p_pipeline->draw(p_start_vertex, p_vertex_count);
		}
	}
}