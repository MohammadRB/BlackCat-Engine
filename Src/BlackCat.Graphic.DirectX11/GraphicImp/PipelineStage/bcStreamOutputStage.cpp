// [02/09/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcExport.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "GraphicImp/PipelineStage/bcStreamOutputStage.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_stream_output_stage<g_api_dx11>::bc_platform_stream_output_stage(platform_pack p_pack) noexcept
			: m_pack(p_pack)
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_stream_output_stage<g_api_dx11>::bc_platform_stream_output_stage(bc_platform_stream_output_stage&& p_other) noexcept
			: m_pack(std::move(p_other.m_pack))
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_stream_output_stage<g_api_dx11>::~bc_platform_stream_output_stage()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_stream_output_stage<g_api_dx11>& bc_platform_stream_output_stage<g_api_dx11>::operator=(bc_platform_stream_output_stage&& p_other) noexcept
		{
			m_pack = std::move(p_other.m_pack);

			return *this;
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_stream_output_stage< g_api_dx11 >::apply_required_state(bc_device_pipeline& p_pipeline)
		{
			ID3D11DeviceContext* l_context = p_pipeline.get_platform_pack().m_pipeline_proxy->m_context;

			if (m_required_state.m_stream_buffers.update_needed() ||
				m_required_state.m_stream_offsets.update_needed())
			{
				ID3D11Buffer* l_buffers[bc_render_api_info::number_of_so_streams()];
				bcUINT* l_offsets = m_required_state.m_stream_offsets.get_first_slot();

				for (bcUINT i = 0; i < bc_render_api_info::number_of_so_streams(); ++i)
				{
					bc_buffer l_buffer = m_required_state.m_stream_buffers.get(i);
					l_buffers[i] = l_buffer.is_valid() ? l_buffer.get_platform_pack().m_buffer : nullptr;
				}

				const bcUINT l_dirty_slot_num = (std::max)
				(
					m_required_state.m_stream_buffers.get_dirty_count(),
					m_required_state.m_stream_offsets.get_dirty_count()
				);

				l_context->SOSetTargets
				(
					l_dirty_slot_num,
					l_buffers,
					l_offsets
				);
			}

			m_required_state.reset_tracking();
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_stream_output_stage<g_api_dx11>::set_to_default_state(bc_device_pipeline& p_pipeline)
		{
			m_required_state.set_to_initial_state();
		}
	}
}