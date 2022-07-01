// [02/09/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcExport.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "GraphicImp/PipelineStage/bcInputAssemblerStage.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_input_assembler_stage<g_api_dx11>::bc_platform_input_assembler_stage(platform_pack p_pack) noexcept
			: m_pack(p_pack)
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_input_assembler_stage<g_api_dx11>::bc_platform_input_assembler_stage(bc_platform_input_assembler_stage&& p_other) noexcept
			: m_pack(std::move(p_other.m_pack))
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_input_assembler_stage<g_api_dx11>::~bc_platform_input_assembler_stage()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_input_assembler_stage<g_api_dx11>& bc_platform_input_assembler_stage<g_api_dx11>::operator=(bc_platform_input_assembler_stage&& p_other) noexcept
		{
			m_pack = std::move(p_other.m_pack);

			return *this;
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_input_assembler_stage<g_api_dx11>::apply_required_state(bc_device_pipeline& p_pipeline)
		{
			ID3D11DeviceContext* l_context = p_pipeline.get_platform_pack().m_pipeline_proxy->m_context;

			if (m_required_state.m_primitive_topology.update_needed())
			{
				l_context->IASetPrimitiveTopology
				(
					bc_graphic_cast(m_required_state.m_primitive_topology.get())
				);
			}

			if (m_required_state.m_vertex_buffers.update_needed() ||
				m_required_state.m_vertex_buffers_offsets.update_needed() ||
				m_required_state.m_vertex_buffers_strides.update_needed())
			{
				ID3D11Buffer* l_vertex_buffers[bc_render_api_info::number_of_ia_vertex_buffers()];
				bcUINT* l_vertex_offsets = m_required_state.m_vertex_buffers_offsets.get_first_slot();
				bcUINT* l_vertex_strides = m_required_state.m_vertex_buffers_strides.get_first_slot();

				for (bcUINT i = 0; i < bc_render_api_info::number_of_ia_vertex_buffers(); ++i)
				{
					bc_buffer l_buffer = m_required_state.m_vertex_buffers.get(i);
					l_vertex_buffers[i] = l_buffer.is_valid() ? l_buffer.get_platform_pack().m_buffer : nullptr;
				}

				const bcUINT l_dirty_slot_start = (std::min)
				(
					(std::min)
					(
						m_required_state.m_vertex_buffers.get_dirty_start(),
						m_required_state.m_vertex_buffers_offsets.get_dirty_start()
					),
					m_required_state.m_vertex_buffers_strides.get_dirty_start()
				);
				const bcUINT l_dirty_slot_num = (std::max)
				(
					(std::max)
					(
						m_required_state.m_vertex_buffers.get_dirty_count(),
						m_required_state.m_vertex_buffers_offsets.get_dirty_count()
					),
					m_required_state.m_vertex_buffers_strides.get_dirty_count()
				);

				l_context->IASetVertexBuffers
				(
					l_dirty_slot_start,
					l_dirty_slot_num - l_dirty_slot_start + 1,
					&l_vertex_buffers[l_dirty_slot_start],
					&l_vertex_strides[l_dirty_slot_start],
					&l_vertex_offsets[l_dirty_slot_start]
				);
			}

			if(m_required_state.m_index_buffer.update_needed())
			{
				bc_buffer l_index_buffer = m_required_state.m_index_buffer.get();
				l_context->IASetIndexBuffer
				(
					l_index_buffer.is_valid() ? l_index_buffer.get_platform_pack().m_buffer : nullptr,
					bc_graphic_cast(m_required_state.m_index_buffer_format.get()),
					0
				);
			}

			m_required_state.reset_tracking();
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_input_assembler_stage<g_api_dx11>::set_to_default_state(bc_device_pipeline& p_pipeline)
		{
			m_required_state.set_to_initial_state();
		}
	}
}