// [02/09/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcExport.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "GraphicImp/PipelineStage/bcRasterizerStage.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_rasterizer_stage< g_api_dx11 >::bc_platform_rasterizer_stage(platform_pack p_pack) noexcept
			: m_pack(p_pack)
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_rasterizer_stage< g_api_dx11 >::~bc_platform_rasterizer_stage()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_rasterizer_stage< g_api_dx11 >::bc_platform_rasterizer_stage(bc_platform_rasterizer_stage&& p_other) noexcept
			: m_pack(std::move(p_other.m_pack))
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_rasterizer_stage< g_api_dx11 >& bc_platform_rasterizer_stage< g_api_dx11 >::operator=(bc_platform_rasterizer_stage&& p_other) noexcept
		{
			m_pack = std::move(p_other.m_pack);

			return *this;
		}

		template < >
		BC_GRAPHICIMP_DLL
		void bc_platform_rasterizer_stage<g_api_dx11>::apply_required_state(bc_device_pipeline& p_pipeline)
		{
			ID3D11DeviceContext* l_context = p_pipeline.get_platform_pack().m_pipeline_proxy->m_context;

			if (m_required_state.m_viewports.update_needed() || m_required_state.m_viewport_count.update_needed())
			{
				D3D11_VIEWPORT l_viewports[bc_render_api_info::number_of_rs_viewport_scissorrect()];

				for (bcUINT i = 0; i < bc_render_api_info::number_of_rs_viewport_scissorrect(); ++i)
				{
					const bc_viewport* l_viewport = m_required_state.m_viewports.get(i);
					if(l_viewport)
					{
						l_viewports[i] = D3D11_VIEWPORT
						{
							static_cast<bcFLOAT>(l_viewport->m_top_left_x),
							static_cast<bcFLOAT>(l_viewport->m_top_left_y),
							static_cast<bcFLOAT>(l_viewport->m_width),
							static_cast<bcFLOAT>(l_viewport->m_height),
							l_viewport->m_min_depth,
							l_viewport->m_max_depth
						};
					}
				}

				l_context->RSSetViewports(m_required_state.m_viewport_count.get(), l_viewports);
			}

			m_required_state.reset_tracking();
		}

		template < >
		BC_GRAPHICIMP_DLL
		void bc_platform_rasterizer_stage<g_api_dx11>::set_to_default_state(bc_device_pipeline& p_pipeline)
		{
			m_required_state.set_to_initial_state();
		}
	}
}