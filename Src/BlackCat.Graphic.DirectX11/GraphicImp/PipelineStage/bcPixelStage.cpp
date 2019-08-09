// [02/10/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcExport.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "GraphicImp/PipelineStage/bcPixelStage.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "GraphicImp/Resource/State/bcSamplerState.h"

namespace black_cat
{
	namespace graphic
	{
		template < >
		BC_GRAPHICIMP_DLL
		bc_platform_pixel_stage<g_api_dx11>::bc_platform_pixel_stage()
			: bc_platform_programmable_stage(bc_platform_programmable_stage::platform_pack())
		{
		}

		template < >
		BC_GRAPHICIMP_DLL
		bc_platform_pixel_stage<g_api_dx11>::bc_platform_pixel_stage(bc_platform_pixel_stage&& p_other)
			: bc_programmable_stage(std::move(p_other))
		{
		}

		template < >
		BC_GRAPHICIMP_DLL
		bc_platform_pixel_stage<g_api_dx11>::~bc_platform_pixel_stage()
		{
		}

		template < >
		BC_GRAPHICIMP_DLL
		bc_platform_pixel_stage<g_api_dx11>& bc_platform_pixel_stage<g_api_dx11>::operator=(bc_platform_pixel_stage&& p_other)
		{
			bc_programmable_stage::operator=(std::move(p_other));

			return *this;
		}

		template < >
		BC_GRAPHICIMP_DLL
		void bc_platform_pixel_stage<g_api_dx11>::apply_shader_program(bc_device_pipeline* p_pipeline)
		{
			// shader programs are in pipeline state
		}

		template < >
		BC_GRAPHICIMP_DLL
		void bc_platform_pixel_stage<g_api_dx11>::apply_constant_buffers(bc_device_pipeline* p_pipeline)
		{
			ID3D11DeviceContext* l_context = p_pipeline->get_platform_pack().m_pipeline_proxy->m_context;
			bc_programmable_stage_state& l_required_state = m_required_state;

			if (m_required_state.m_constant_buffers.update_needed())
			{
				ID3D11Buffer* l_constant_buffers[bc_render_api_info::number_of_shader_constant_buffer()];

				for (bcUINT i = 0; i < bc_render_api_info::number_of_shader_constant_buffer(); ++i)
				{
					bc_buffer l_buffer = l_required_state.m_constant_buffers.get(i);
					l_constant_buffers[i] = l_buffer.is_valid() ? l_buffer.get_platform_pack().m_buffer : nullptr;
				}

				bcUINT l_dirty_slot_start = m_required_state.m_constant_buffers.get_dirty_start();
				bcUINT l_dirty_slot_num = m_required_state.m_constant_buffers.get_dirty_count();

				l_context->PSSetConstantBuffers(l_dirty_slot_start, l_dirty_slot_num, &l_constant_buffers[l_dirty_slot_start]);
			}
		}

		template < >
		BC_GRAPHICIMP_DLL
		void bc_platform_pixel_stage<g_api_dx11>::apply_sampler_states(bc_device_pipeline* p_pipeline)
		{
			ID3D11DeviceContext* l_context = p_pipeline->get_platform_pack().m_pipeline_proxy->m_context;
			bc_programmable_stage_state& l_required_state = m_required_state;

			if (m_required_state.m_sampler_states.update_needed())
			{
				ID3D11SamplerState* l_sampler_states[bc_render_api_info::number_of_shader_sampler()];

				for (bcUINT i = 0; i < bc_render_api_info::number_of_shader_sampler(); ++i)
				{
					bc_sampler_state l_sampler_state = l_required_state.m_sampler_states.get(i);
					l_sampler_states[i] = l_sampler_state.is_valid() ? l_sampler_state.get_platform_pack().m_sampler_state : nullptr;
				}

				bcUINT l_dirty_slot_start = m_required_state.m_sampler_states.get_dirty_start();
				bcUINT l_dirty_slot_num = m_required_state.m_sampler_states.get_dirty_count();

				l_context->PSSetSamplers(l_dirty_slot_start, l_dirty_slot_num, &l_sampler_states[l_dirty_slot_start]);
			}
		}

		template < >
		BC_GRAPHICIMP_DLL
		void bc_platform_pixel_stage<g_api_dx11>::apply_shader_resource_views(bc_device_pipeline* p_pipeline)
		{
			ID3D11DeviceContext* l_context = p_pipeline->get_platform_pack().m_pipeline_proxy->m_context;
			bc_programmable_stage_state& l_required_state = m_required_state;

			if (m_required_state.m_shader_resource_views.update_needed())
			{
				ID3D11ShaderResourceView* l_views[bc_render_api_info::number_of_shader_resource()];

				for (bcUINT i = 0; i < bc_render_api_info::number_of_shader_resource(); ++i)
				{
					bc_resource_view l_shader_view = l_required_state.m_shader_resource_views.get(i);
					l_views[i] = l_shader_view.is_valid() ? l_shader_view.get_platform_pack().m_shader_view : nullptr;
				}

				bcUINT l_dirty_slot_start = m_required_state.m_shader_resource_views.get_dirty_start();
				bcUINT l_dirty_slot_num = m_required_state.m_shader_resource_views.get_dirty_count();

				l_context->PSSetShaderResources(l_dirty_slot_start, l_dirty_slot_num, &l_views[l_dirty_slot_start]);
			}
		}

		//template < >
		//void bc_platform_pixel_stage<g_api_dx11>::apply_unordered_access_views(bc_device_pipeline& p_pipeline)
		//{
		//	// UAVs set in output merger stage
		//}
	}
}