// [02/10/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/PipelineStage/bcComputeStage.h"
#include "GraphicImp/PipelineStage/bcVertexStage.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "GraphicImp/Resource/State/bcSamplerState.h"
#include "GraphicImp/bcExport.h"

namespace black_cat
{
	namespace graphic
	{
		template < >
		BC_GRAPHICIMP_DLL
		bc_platform_compute_stage<g_api_dx11>::bc_platform_compute_stage() noexcept
			: bc_platform_programmable_stage(platform_pack()),
			m_initial_counts()
		{
		}

		template < >
		BC_GRAPHICIMP_DLL
		bc_platform_compute_stage<g_api_dx11>::bc_platform_compute_stage(bc_platform_compute_stage&& p_other) noexcept
			: bc_platform_programmable_stage(std::move(p_other)),
			m_initial_counts(p_other.m_initial_counts)
		{
		}

		template < >
		BC_GRAPHICIMP_DLL
		bc_platform_compute_stage<g_api_dx11>::~bc_platform_compute_stage()
		{
		}

		template < >
		BC_GRAPHICIMP_DLL
		bc_platform_compute_stage<g_api_dx11>& bc_platform_compute_stage<g_api_dx11>::operator=(bc_platform_compute_stage&& p_other) noexcept
		{
			bc_programmable_stage::operator=(std::move(p_other));

			return *this;
		}

		template < >
		BC_GRAPHICIMP_DLL
		void bc_platform_compute_stage<g_api_dx11>::apply_required_state(bc_device_pipeline& p_pipeline, const initial_counts_array* p_initial_counts)
		{
			if(p_initial_counts)
			{
				m_initial_counts = *p_initial_counts;
			}
			else
			{
				std::for_each(std::begin(m_initial_counts), std::end(m_initial_counts), [](initial_counts_array::value_type& p_count)
				{
					p_count = -1;
				});
			}
			
			bc_programmable_stage::apply_required_state(p_pipeline);

			std::for_each(std::begin(m_initial_counts), std::end(m_initial_counts), [](initial_counts_array::value_type& p_count)
			{
				p_count = -1;
			});
		}

		template < >
		BC_GRAPHICIMP_DLL
		void bc_platform_compute_stage<g_api_dx11>::apply_shader_program(bc_device_pipeline& p_pipeline)
		{
			// shader programs are in pipeline state
		}

		template < >
		BC_GRAPHICIMP_DLL
		void bc_platform_compute_stage<g_api_dx11>::apply_constant_buffers(bc_device_pipeline& p_pipeline)
		{
			ID3D11DeviceContext* l_context = p_pipeline.get_platform_pack().m_pipeline_proxy->m_context;

			if (m_required_state.m_constant_buffers.update_needed())
			{
				ID3D11Buffer* l_constant_buffers[bc_render_api_info::number_of_shader_constant_buffer()];

				for (bcUINT i = 0; i < bc_render_api_info::number_of_shader_constant_buffer(); ++i)
				{
					bc_buffer l_buffer = m_required_state.m_constant_buffers.get(i);
					l_constant_buffers[i] = l_buffer.is_valid() ? l_buffer.get_platform_pack().m_buffer : nullptr;
				}

				const bcUINT l_dirty_slot_start = m_required_state.m_constant_buffers.get_dirty_start();
				const bcUINT l_dirty_slot_num = m_required_state.m_constant_buffers.get_dirty_count();

				l_context->CSSetConstantBuffers(l_dirty_slot_start, l_dirty_slot_num, &l_constant_buffers[l_dirty_slot_start]);
			}
		}

		template < >
		BC_GRAPHICIMP_DLL
		void bc_platform_compute_stage<g_api_dx11>::apply_sampler_states(bc_device_pipeline& p_pipeline)
		{
			ID3D11DeviceContext* l_context = p_pipeline.get_platform_pack().m_pipeline_proxy->m_context;

			if (m_required_state.m_sampler_states.update_needed())
			{
				ID3D11SamplerState* l_sampler_states[bc_render_api_info::number_of_shader_sampler()];

				for (bcUINT i = 0; i < bc_render_api_info::number_of_shader_sampler(); ++i)
				{
					bc_sampler_state l_sampler_state = m_required_state.m_sampler_states.get(i);
					l_sampler_states[i] = l_sampler_state.is_valid() ? l_sampler_state.get_platform_pack().m_sampler_state : nullptr;
				}

				const bcUINT l_dirty_slot_start = m_required_state.m_sampler_states.get_dirty_start();
				const bcUINT l_dirty_slot_num = m_required_state.m_sampler_states.get_dirty_count();

				l_context->CSSetSamplers(l_dirty_slot_start, l_dirty_slot_num, &l_sampler_states[l_dirty_slot_start]);
			}
		}

		template < >
		BC_GRAPHICIMP_DLL
		void bc_platform_compute_stage<g_api_dx11>::apply_shader_resource_views(bc_device_pipeline& p_pipeline)
		{
			ID3D11DeviceContext* l_context = p_pipeline.get_platform_pack().m_pipeline_proxy->m_context;

			if (m_required_state.m_shader_resource_views.update_needed())
			{
				ID3D11ShaderResourceView* l_views[bc_render_api_info::number_of_shader_resource()];

				for (bcUINT i = 0; i < bc_render_api_info::number_of_shader_resource(); ++i)
				{
					bc_resource_view l_shader_view = m_required_state.m_shader_resource_views.get(i);
					l_views[i] = l_shader_view.is_valid() ? l_shader_view.get_platform_pack().m_shader_view : nullptr;
				}

				const bcUINT l_dirty_slot_start = m_required_state.m_shader_resource_views.get_dirty_start();
				const bcUINT l_dirty_slot_num = m_required_state.m_shader_resource_views.get_dirty_count();

				l_context->CSSetShaderResources(l_dirty_slot_start, l_dirty_slot_num, &l_views[l_dirty_slot_start]);
			}

			if (m_required_state.m_unordered_access_views.update_needed())
			{
				ID3D11UnorderedAccessView* l_ua_views[bc_render_api_info::number_of_ps_cs_uav_resource()];
				bcUINT l_initial_counts[bc_render_api_info::number_of_ps_cs_uav_resource()];

				for (bcUINT i = 0; i < bc_render_api_info::number_of_ps_cs_uav_resource(); ++i)
				{
					bc_resource_view l_shader_view = m_required_state.m_unordered_access_views.get(i);
					l_ua_views[i] = l_shader_view.is_valid() ? l_shader_view.get_platform_pack().m_unordered_shader_view : nullptr;
					l_initial_counts[i] = m_initial_counts[i];
				}

				const bcUINT l_dirty_slot_start = m_required_state.m_unordered_access_views.get_dirty_start();
				const bcUINT l_dirty_slot_num = m_required_state.m_unordered_access_views.get_dirty_count();

				l_context->CSSetUnorderedAccessViews(l_dirty_slot_start, l_dirty_slot_num, &l_ua_views[l_dirty_slot_start], l_initial_counts);
			}
		}

		//template < >
		//void bc_platform_compute_stage<g_api_dx11>::apply_unordered_access_views(bc_device_pipeline& p_pipeline)
		//{
		//	ID3D11DeviceContext* l_context = p_pipeline.get_platform_pack().m_context.Get();
		//	bc_programmable_stage_state& l_required_state = m_required_state;

		//	if (m_required_state.m_unordered_access_views.update_needed())
		//	{
		//		ID3D11UnorderedAccessView* l_ua_views[bc_render_api_info::number_of_ps_cs_uav_registers()];
		//		bcUINT* l_initial_counts = l_required_state.m_uav_initial_counts.get_first_location();

		//		for (bcUINT i = 0; i < bc_render_api_info::number_of_ps_cs_uav_registers(); ++i)
		//		{
		//			// TODO Find a mechanism for accessing to UAV
		//			l_ua_views[i] = l_required_state.m_unordered_access_views.get(i)->get_platform_pack().m_sampler_state.Get();
		//		}

		//		bcUINT l_start_slot = m_required_state.m_unordered_access_views.get_dirty_start();
		//		bcUINT l_end_slot = m_required_state.m_unordered_access_views.get_dirty_end();

		//		l_context->CSSetUnorderedAccessViews(l_start_slot, l_end_slot - l_start_slot + 1, &l_ua_views[l_dirty_slot_start], l_initial_counts);
		//	
		//};
	}
}