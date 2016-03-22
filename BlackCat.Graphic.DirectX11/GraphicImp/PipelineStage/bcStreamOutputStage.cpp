// [02/09/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "GraphicImp/PipelineStage/bcStreamOutputStage.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"

namespace black_cat
{
	namespace graphic
	{
		template < >
		BC_GRAPHICIMP_DLL_EXP
		bc_platform_stream_output_stage<g_api_dx11>::bc_platform_stream_output_stage()
			: m_pack()
		{
		}

		template < >
		BC_GRAPHICIMP_DLL_EXP
		bc_platform_stream_output_stage<g_api_dx11>::bc_platform_stream_output_stage(bc_platform_stream_output_stage&& p_other)
			: m_pack(std::move(p_other.m_pack))
		{
		}

		template < >
		BC_GRAPHICIMP_DLL_EXP
		bc_platform_stream_output_stage<g_api_dx11>::~bc_platform_stream_output_stage()
		{
		}

		template < >
		BC_GRAPHICIMP_DLL_EXP
		bc_platform_stream_output_stage<g_api_dx11>& bc_platform_stream_output_stage<g_api_dx11>::operator=(bc_platform_stream_output_stage&& p_other)
		{
			m_pack = std::move(p_other.m_pack);

			return *this;
		}

		template < >
		BC_GRAPHICIMP_DLL_EXP
		void bc_platform_stream_output_stage< g_api_dx11 >::apply_required_state(bc_device_pipeline* p_pipeline)
		{
			ID3D11DeviceContext* l_context = p_pipeline->get_platform_pack().m_context.Get();
			bc_stream_output_stage_state& l_required_state = m_required_state;

			if (l_required_state.m_stream_buffers.update_needed() ||
				l_required_state.m_stream_offsets.update_needed())
			{
				ID3D11Buffer* l_buffers[bc_render_api_info::number_of_so_streams()];
				bcUINT* l_offsets = l_required_state.m_stream_offsets.get_first_slot();

				for (bcUINT i = 0; i < bc_render_api_info::number_of_so_streams(); ++i)
				{
					bc_buffer* l_buffer = l_required_state.m_stream_buffers.get(i);
					l_buffers[i] = l_buffer ? l_buffer->get_platform_pack().m_buffer.Get() : nullptr;
				}

				bcUINT l_dirty_slot_num = (std::max)
					(
						l_required_state.m_stream_buffers.get_dirty_count(),
						l_required_state.m_stream_offsets.get_dirty_count()
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

		template < >
		BC_GRAPHICIMP_DLL_EXP
		void bc_platform_stream_output_stage<g_api_dx11>::set_to_default_state(bc_device_pipeline* p_pipeline)
		{
			m_required_state.set_to_initial_state();
		}
	}
}