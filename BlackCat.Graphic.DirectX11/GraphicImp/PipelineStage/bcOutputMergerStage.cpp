// [02/09/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "GraphicImp/PipelineStage/bcOutputMergerStage.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Resource/View/bcShaderView.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "GraphicImp/Resource/View/bcDepthStencilView.h"

namespace black_cat
{
	namespace graphic
	{
		template< >
		BC_GRAPHICIMP_DLL_EXP
		bc_platform_output_merger_stage<g_api_dx11>::bc_platform_output_merger_stage()
			: m_pack()
		{
		}

		template < >
		BC_GRAPHICIMP_DLL_EXP
		bc_platform_output_merger_stage<g_api_dx11>::bc_platform_output_merger_stage(bc_platform_output_merger_stage&& p_other)
			: m_pack(std::move(p_other.m_pack))
		{
		}

		template < >
		BC_GRAPHICIMP_DLL_EXP
		bc_platform_output_merger_stage<g_api_dx11>::~bc_platform_output_merger_stage()
		{
		}

		template < >
		BC_GRAPHICIMP_DLL_EXP
		bc_platform_output_merger_stage<g_api_dx11>& bc_platform_output_merger_stage<g_api_dx11>::operator=(bc_platform_output_merger_stage&& p_other)
		{
			m_pack = std::move(p_other.m_pack);

			return *this;
		}

		template < >
		BC_GRAPHICIMP_DLL_EXP
		void bc_platform_output_merger_stage<g_api_dx11>::apply_required_state(bc_device_pipeline* p_pipeline)
		{
			ID3D11DeviceContext* l_context = p_pipeline->get_platform_pack().m_context.Get();
			bc_output_merger_stage_state& l_required_state = m_required_state;

			if(l_required_state.m_blend_factors.update_needed())
			{
				ComPtr<ID3D11BlendState> l_current_blend_state;
				bcFLOAT32 l_current_blend_factors[4];
				bcUINT l_current_sample_mask;

				l_context->OMGetBlendState(l_current_blend_state.GetAddressOf(), l_current_blend_factors, &l_current_sample_mask);

				l_current_blend_factors[0] = l_required_state.m_blend_factors.get().x;
				l_current_blend_factors[1] = l_required_state.m_blend_factors.get().y;
				l_current_blend_factors[2] = l_required_state.m_blend_factors.get().z;
				l_current_blend_factors[3] = l_required_state.m_blend_factors.get().w;

				l_context->OMSetBlendState(l_current_blend_state.Get(), l_current_blend_factors, l_current_sample_mask);
			}

			if(l_required_state.m_stencil_ref.update_needed())
			{
				ComPtr<ID3D11DepthStencilState> l_current_depth_stencil_state;
				bcUINT32 l_current_stencil_ref;

				l_context->OMGetDepthStencilState(l_current_depth_stencil_state.GetAddressOf(), &l_current_stencil_ref);

				l_context->OMSetDepthStencilState(l_current_depth_stencil_state.Get(), l_required_state.m_stencil_ref.get());
			}

			if (l_required_state.m_render_target_views.update_needed() ||
				l_required_state.m_depth_target_view.update_needed() ||
				l_required_state.m_unordered_access_views.update_needed()
				/*l_required_state.m_uav_initial_counts.update_needed()*/)
			{
				bcUINT l_num_rtv = l_required_state.m_render_target_views.get_dirty_count();
				ID3D11RenderTargetView* l_render_target_views[bc_render_api_info::number_of_om_render_target_slots()];
				ID3D11DepthStencilView* l_depth_stencil_view;
				ID3D11UnorderedAccessView* l_unordered_views[bc_render_api_info::number_of_ps_cs_uav_registers()];
				bcUINT l_uav_initialCounts[bc_render_api_info::number_of_ps_cs_uav_registers()];

				for (bcUINT i = 0; i < bc_render_api_info::number_of_om_render_target_slots(); ++i)
				{
					bc_render_target_view* l_target_view = l_required_state.m_render_target_views.get(i);
					l_render_target_views[i] = l_target_view ?
						                           l_target_view->get_platform_pack().m_render_target_view.Get() :
						                           nullptr;
				}
				l_depth_stencil_view = l_required_state.m_depth_target_view.get() ?
					                       l_required_state.m_depth_target_view.get()->get_platform_pack().m_depth_stencil_view.Get() :
					                       nullptr;
				for (bcUINT i = 0; i < bc_render_api_info::number_of_ps_cs_uav_registers(); ++i)
				{
					bc_shader_view* l_view = l_required_state.m_unordered_access_views.get(i);
					l_unordered_views[i] = l_view ? l_view->get_platform_pack().m_unordered_shader_view.Get() : nullptr;
					l_uav_initialCounts[i] = -1;
				}

				bcUINT l_uav_dirty_slot_start = l_required_state.m_unordered_access_views.get_dirty_start();
				bcUINT l_uav_dirty_slot_num = l_required_state.m_unordered_access_views.get_dirty_count();

				l_context->OMSetRenderTargetsAndUnorderedAccessViews
					(
						l_num_rtv,
						l_render_target_views,
						l_depth_stencil_view,
						l_uav_dirty_slot_start,
						l_uav_dirty_slot_num,
						l_unordered_views,
						l_uav_initialCounts
					);
			}

			m_required_state.reset_tracking();
		}

		template < >
		BC_GRAPHICIMP_DLL_EXP
		void bc_platform_output_merger_stage<g_api_dx11>::set_to_default_state(bc_device_pipeline* p_pipeline)
		{
			m_required_state.set_to_initial_state();
		}
	}
}