// [01/12/2016 MRB]

#include "Graphic/GraphicPCH.h"
//#include "Graphic/PipelineStage/bcInputAssemblerStage.h"
//#include "Graphic/bcRenderApiInfo.h"
//
//namespace black_cat
//{
//	namespace graphic
//	{
//		bc_input_assembler_stage_state::bc_input_assembler_stage_state()
//			: m_index_buffer(bc_buffer()),
//			m_index_buffer_format(bc_format::R32_UINT),
//			m_vertex_buffers(bc_buffer()),
//			m_vertex_buffers_strides(0),
//			m_vertex_buffers_offsets(0),
//			//m_input_layout(nullptr),
//			m_primitive_topology(bc_primitive::undefined)
//			
//		{
//		}
//
//		bc_input_assembler_stage_state::~bc_input_assembler_stage_state()
//		{
//		}
//
//		void bc_input_assembler_stage_state::set_to_initial_state() noexcept
//		{
//			m_index_buffer.set_to_initial_state();
//			m_index_buffer_format.set_to_initial_state();
//			m_vertex_buffers.set_to_initial_state();
//			m_vertex_buffers_strides.set_to_initial_state();
//			m_vertex_buffers_offsets.set_to_initial_state();
//			//m_input_layout.set_to_initial_state();
//			m_primitive_topology.set_to_initial_state();
//		}
//
//		void bc_input_assembler_stage_state::reset_tracking() noexcept
//		{
//			m_index_buffer.reset_tracking();
//			m_index_buffer_format.reset_tracking();
//			m_vertex_buffers.reset_tracking();
//			m_vertex_buffers_strides.reset_tracking();
//			m_vertex_buffers_offsets.reset_tracking();
//			//m_input_layout.reset_tracking();
//			m_primitive_topology.reset_tracking();
//		}
//	}
//}