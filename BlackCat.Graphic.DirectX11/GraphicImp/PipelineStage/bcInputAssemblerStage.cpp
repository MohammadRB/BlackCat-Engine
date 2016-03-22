// [02/09/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "GraphicImp/PipelineStage/bcInputAssemblerStage.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"

namespace black_cat
{
	namespace graphic
	{
		template< >
		BC_GRAPHICIMP_DLL_EXP
		bc_platform_input_assembler_stage<g_api_dx11>::bc_platform_input_assembler_stage()
			: m_pack()
		{
		}

		template < >
		BC_GRAPHICIMP_DLL_EXP
		bc_platform_input_assembler_stage<g_api_dx11>::bc_platform_input_assembler_stage(bc_platform_input_assembler_stage&& p_other)
			: m_pack(std::move(p_other.m_pack))
		{
		}

		template < >
		BC_GRAPHICIMP_DLL_EXP
		bc_platform_input_assembler_stage<g_api_dx11>::~bc_platform_input_assembler_stage()
		{
		}

		template < >
		BC_GRAPHICIMP_DLL_EXP
		bc_platform_input_assembler_stage<g_api_dx11>& bc_platform_input_assembler_stage<g_api_dx11>::operator=(bc_platform_input_assembler_stage&& p_other)
		{
			m_pack = std::move(p_other.m_pack);

			return *this;
		}

		template < >
		BC_GRAPHICIMP_DLL_EXP
		void bc_platform_input_assembler_stage<g_api_dx11>::apply_required_state(bc_device_pipeline* p_pipeline)
		{
			ID3D11DeviceContext* l_context = p_pipeline->get_platform_pack().m_context.Get();
			bc_input_assembler_stage_state& l_required_state = m_required_state;

			if (l_required_state.m_primitive_topology.update_needed())
			{
				l_context->IASetPrimitiveTopology
				(
					bc_graphic_cast(m_required_state.m_primitive_topology.get())
				);
			}

			if (l_required_state.m_vertex_buffers.update_needed() ||
				l_required_state.m_vertex_buffers_offsets.update_needed() ||
				l_required_state.m_vertex_buffers_strides.update_needed())
			{
				ID3D11Buffer* l_vertex_buffers[bc_render_api_info::number_of_ia_vertex_buffers()];
				bcUINT* l_vertex_offsets = l_required_state.m_vertex_buffers_offsets.get_first_slot();
				bcUINT* l_vertex_strides = l_required_state.m_vertex_buffers_strides.get_first_slot();

				for (bcUINT i = 0; i < bc_render_api_info::number_of_ia_vertex_buffers(); ++i)
				{
					bc_buffer* l_buffer = l_required_state.m_vertex_buffers.get(i);
					l_vertex_buffers[i] = l_buffer ? l_buffer->get_platform_pack().m_buffer.Get() : nullptr;
				}

				bcUINT l_dirty_slot_start = (std::min)
					(
						(std::min)
						(
							l_required_state.m_vertex_buffers.get_dirty_start(),
							l_required_state.m_vertex_buffers_offsets.get_dirty_start()
						),
						l_required_state.m_vertex_buffers_strides.get_dirty_start()
					);
				bcUINT l_dirty_slot_num = (std::max)
					(
						(std::max)
						(
							l_required_state.m_vertex_buffers.get_dirty_count(),
							l_required_state.m_vertex_buffers_offsets.get_dirty_count()
						),
						l_required_state.m_vertex_buffers_strides.get_dirty_count()
					);

				l_context->IASetVertexBuffers
					(
						l_dirty_slot_start,
						l_dirty_slot_num - l_dirty_slot_start + 1,
						l_vertex_buffers,
						l_vertex_strides,
						l_vertex_offsets
					);
			}

			if(l_required_state.m_index_buffer.update_needed())
			{
				l_context->IASetIndexBuffer
					(
						l_required_state.m_index_buffer.get()->get_platform_pack().m_buffer.Get(),
						bc_graphic_cast(l_required_state.m_index_buffer_format.get()),
						0
					);
			}

			m_required_state.reset_tracking();
		};

		template < >
		BC_GRAPHICIMP_DLL_EXP
		void bc_platform_input_assembler_stage<g_api_dx11>::set_to_default_state(bc_device_pipeline* p_pipeline)
		{
			m_required_state.set_to_initial_state();
		};
	}
}