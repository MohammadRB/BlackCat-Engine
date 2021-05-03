// [12/22/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcExport.h"
#include "GraphicImp/bcDeviceRef.h"
#include "Graphic/Device/bcDeviceOcclusionQuery.h"
#include "GraphicImp/Device/Command/bcDeviceCommandList.h"
#include "GraphicImp/Device/Command/bcDeviceCommandExecutor.h"
#include "GraphicImp/Device/bcDeviceComputeState.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "GraphicImp/Device/bcDeviceOcclusionQuery.h"
#include "GraphicImp/Resource/State/bcSamplerState.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/View/bcDepthStencilView.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "GraphicImp/Shader/bcShader.h"
#include "GraphicImp/Shader/bcComputeShader.h"
#include "GraphicImp/Shader/bcDomainShader.h"
#include "GraphicImp/Shader/bcGeometryShader.h"
#include "GraphicImp/Shader/bcHullShader.h"
#include "GraphicImp/Shader/bcPixelShader.h"
#include "GraphicImp/Shader/bcVertexShader.h"

namespace black_cat
{
	namespace graphic
	{
		void _add_ref(bc_platform_device_command_list< g_api_dx11 >& p_command_list)
		{
			// See bcDevicePipeline.finish_command_list function.
			p_command_list.get_platform_pack().m_command_list_proxy->add_ref();
		}

		void _release(bc_platform_device_command_list< g_api_dx11 >& p_command_list)
		{
			// See bcDevicePipeline.finish_command_list function.
			p_command_list.get_platform_pack().m_command_list_proxy->release();
		}

		void _add_ref(bc_platform_device_command_executor< g_api_dx11 >& p_command_executor)
		{
		}

		void _release(bc_platform_device_command_executor< g_api_dx11 >& p_command_executor)
		{
		}

		void _add_ref(bc_platform_device_compute_state< g_api_dx11 >& p_compute_state)
		{
			p_compute_state.get_platform_pack().m_compute_state_proxy->add_ref();
		}

		void _release(bc_platform_device_compute_state< g_api_dx11 >& p_compute_state)
		{
			p_compute_state.get_platform_pack().m_compute_state_proxy->release();
		}

		void _add_ref(bc_platform_device_pipeline< g_api_dx11 >& p_pipeline)
		{
			p_pipeline.get_platform_pack().m_pipeline_proxy->add_ref();
			p_pipeline.get_platform_pack().m_pipeline_proxy->m_context->AddRef();
			p_pipeline.get_platform_pack().m_pipeline_proxy->m_query->AddRef();
		}

		void _release(bc_platform_device_pipeline< g_api_dx11 >& p_pipeline)
		{
			p_pipeline.get_platform_pack().m_pipeline_proxy->m_context->Release();
			p_pipeline.get_platform_pack().m_pipeline_proxy->m_query->Release();
			p_pipeline.get_platform_pack().m_pipeline_proxy->release();
		}

		void _add_ref(bc_platform_device_pipeline_state< g_api_dx11 >& p_pipeline_state)
		{
			p_pipeline_state.get_platform_pack().m_pipeline_state_proxy->add_ref();
			p_pipeline_state.get_platform_pack().m_pipeline_state_proxy->m_blend_state->AddRef();
			p_pipeline_state.get_platform_pack().m_pipeline_state_proxy->m_depth_stencil_state->AddRef();
			p_pipeline_state.get_platform_pack().m_pipeline_state_proxy->m_rasterizer_state->AddRef();
			if (p_pipeline_state.get_platform_pack().m_pipeline_state_proxy->m_input_layout)
			{
				p_pipeline_state.get_platform_pack().m_pipeline_state_proxy->m_input_layout->AddRef();
			}
		}

		void _release(bc_platform_device_pipeline_state< g_api_dx11 >& p_pipeline_state)
		{
			p_pipeline_state.get_platform_pack().m_pipeline_state_proxy->m_blend_state->Release();
			p_pipeline_state.get_platform_pack().m_pipeline_state_proxy->m_depth_stencil_state->Release();
			p_pipeline_state.get_platform_pack().m_pipeline_state_proxy->m_rasterizer_state->Release();
			if (p_pipeline_state.get_platform_pack().m_pipeline_state_proxy->m_input_layout)
			{
				p_pipeline_state.get_platform_pack().m_pipeline_state_proxy->m_input_layout->Release();
			}
			
			p_pipeline_state.get_platform_pack().m_pipeline_state_proxy->release();
		}

		void _add_ref(bc_platform_buffer< g_api_dx11 >& p_buffer)
		{
			p_buffer.get_platform_pack().m_buffer->AddRef();
		}

		void _release(bc_platform_buffer< g_api_dx11 >& p_buffer)
		{
			p_buffer.get_platform_pack().m_buffer->Release();
		}

		void _add_ref(bc_platform_sampler_state< g_api_dx11 >& p_sampler_state)
		{
			p_sampler_state.get_platform_pack().m_sampler_state->AddRef();
		}

		void _release(bc_platform_sampler_state< g_api_dx11 >& p_sampler_state)
		{
			p_sampler_state.get_platform_pack().m_sampler_state->Release();
		}

		void _add_ref(bc_platform_texture2d< g_api_dx11 >& p_texture2)
		{
			p_texture2.get_platform_pack().m_texture->AddRef();
		}

		void _release(bc_platform_texture2d< g_api_dx11 >& p_texture2)
		{
			p_texture2.get_platform_pack().m_texture->Release();
		}

		void _add_ref(bc_platform_depth_stencil_view< g_api_dx11 >& p_depth_stencil)
		{
			p_depth_stencil.get_platform_pack().m_depth_stencil_view->AddRef();
		}

		void _release(bc_platform_depth_stencil_view< g_api_dx11 >& p_depth_stencil)
		{
			p_depth_stencil.get_platform_pack().m_depth_stencil_view->Release();
		}

		void _add_ref(bc_platform_render_target_view< g_api_dx11 >& p_render_target)
		{
			p_render_target.get_platform_pack().m_render_target_view->AddRef();
		}

		void _release(bc_platform_render_target_view< g_api_dx11 >& p_render_target)
		{
			p_render_target.get_platform_pack().m_render_target_view->Release();
		}

		void _add_ref(bc_platform_resource_view< g_api_dx11 >& p_resource_view)
		{
			auto& l_pack = p_resource_view.get_platform_pack();

			if(l_pack.m_shader_view)
			{
				l_pack.m_shader_view->AddRef();
			}
			else
			{
				l_pack.m_unordered_shader_view->AddRef();
			}
		}

		void _release(bc_platform_resource_view< g_api_dx11 >& p_resource_view)
		{
			auto& l_pack = p_resource_view.get_platform_pack();

			if (l_pack.m_shader_view)
			{
				l_pack.m_shader_view->Release();
			}
			else
			{
				l_pack.m_unordered_shader_view->Release();
			}
		}

		void _add_ref(bc_platform_compiled_shader<g_api_dx11>& p_shader)
		{
			p_shader.get_platform_pack().m_blob->AddRef();
		}

		void _release(bc_platform_compiled_shader<g_api_dx11>& p_shader)
		{
			p_shader.get_platform_pack().m_blob->Release();
		}

		void _add_ref(bc_platform_compute_shader< g_api_dx11 >& p_shader)
		{
			p_shader.get_platform_pack().m_shader->AddRef();
		}

		void _release(bc_platform_compute_shader< g_api_dx11 >& p_shader)
		{
			p_shader.get_platform_pack().m_shader->Release();
		}

		void _add_ref(bc_platform_domain_shader< g_api_dx11 >& p_shader)
		{
			p_shader.get_platform_pack().m_shader->AddRef();
		}

		void _release(bc_platform_domain_shader< g_api_dx11 >& p_shader)
		{
			p_shader.get_platform_pack().m_shader->Release();
		}

		void _add_ref(bc_platform_geometry_shader< g_api_dx11 >& p_shader)
		{
			p_shader.get_platform_pack().m_shader->AddRef();
		}

		void _release(bc_platform_geometry_shader< g_api_dx11 >& p_shader)
		{
			p_shader.get_platform_pack().m_shader->Release();
		}

		void _add_ref(bc_platform_hull_shader< g_api_dx11 >& p_shader)
		{
			p_shader.get_platform_pack().m_shader->AddRef();
		}

		void _release(bc_platform_hull_shader< g_api_dx11 >& p_shader)
		{
			p_shader.get_platform_pack().m_shader->Release();
		}

		void _add_ref(bc_platform_pixel_shader< g_api_dx11 >& p_shader)
		{
			p_shader.get_platform_pack().m_shader->AddRef();
		}

		void _release(bc_platform_pixel_shader< g_api_dx11 >& p_shader)
		{
			p_shader.get_platform_pack().m_shader->Release();
		}

		void _add_ref(bc_platform_vertex_shader< g_api_dx11 >& p_shader)
		{
			p_shader.get_platform_pack().m_shader->AddRef();
			p_shader.get_platform_pack().m_compiled_shader->AddRef();
		}

		void _release(bc_platform_vertex_shader< g_api_dx11 >& p_shader)
		{
			p_shader.get_platform_pack().m_shader->Release();
			p_shader.get_platform_pack().m_compiled_shader->Release();
		}

		void _add_ref(bc_platform_device_occlusion_query<g_api_dx11>& p_query)
		{
			p_query.get_platform_pack().m_query->AddRef();
		}

		void _release(bc_platform_device_occlusion_query<g_api_dx11>& p_query)
		{
			p_query.get_platform_pack().m_query->Release();
		}
	}
}