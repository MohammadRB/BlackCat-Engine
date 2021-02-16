// [01/26/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/Utility/bcEnumOperand.h"
#include "Core/Math/bcMatrix4f.h"
#include "GraphicImp/bcExport.h"
#include "GraphicImp/Resource/bcResource.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "GraphicImp/Resource/View/bcDepthStencilView.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Device/bcDeviceComputeState.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "GraphicImp/Device/Command/bcDeviceCommandList.h"
#include "GraphicImp/Shader/bcVertexShader.h"
#include "GraphicImp/Shader/bcHullShader.h"
#include "GraphicImp/Shader/bcDomainShader.h"
#include "GraphicImp/Shader/bcGeometryShader.h"
#include "GraphicImp/Shader/bcPixelShader.h"
#include "GraphicImp/Shader/Parameter/bcConstantBufferParameter.h"
#include "GraphicImp/Shader/Parameter/bcSamplerParameter.h"
#include "GraphicImp/Shader/Parameter/bcResourceViewParameter.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_pipeline<g_api_dx11>::bc_platform_device_pipeline()
			: m_pack()
		{
			m_pack.m_pipeline_proxy = nullptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_pipeline< g_api_dx11 >::bc_platform_device_pipeline(platform_pack& p_pack)
			: m_pack()
		{
			m_pack.m_pipeline_proxy = p_pack.m_pipeline_proxy;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_pipeline< g_api_dx11 >::bc_platform_device_pipeline(const bc_platform_device_pipeline& p_other)
			: bc_platform_device_reference(p_other),
			m_pack()
		{
			m_pack.m_pipeline_proxy = p_other.m_pack.m_pipeline_proxy;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_pipeline< g_api_dx11 >::~bc_platform_device_pipeline()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_pipeline< g_api_dx11 >& bc_platform_device_pipeline< g_api_dx11 >::operator=(const bc_platform_device_pipeline& p_other)
		{
			bc_platform_device_reference::operator=(p_other);
			m_pack.m_pipeline_proxy = p_other.m_pack.m_pipeline_proxy;

			return *this;
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::bind_pipeline_state(bc_device_pipeline_state p_state)
		{
			if (p_state.get_platform_pack().m_pipeline_state_proxy->m_config.m_vertex_shader != nullptr)
			{
				m_pack.m_pipeline_proxy->m_context->VSSetShader
				(
					p_state.get_platform_pack().m_pipeline_state_proxy->m_config.m_vertex_shader.get_platform_pack().m_shader,
					nullptr,
					0
				);
			}
			if (p_state.get_platform_pack().m_pipeline_state_proxy->m_config.m_hull_shader != nullptr)
			{
				m_pack.m_pipeline_proxy->m_context->HSSetShader
				(
					p_state.get_platform_pack().m_pipeline_state_proxy->m_config.m_hull_shader.get_platform_pack().m_shader,
					nullptr,
					0
				);
			}
			if (p_state.get_platform_pack().m_pipeline_state_proxy->m_config.m_domain_shader != nullptr)
			{
				m_pack.m_pipeline_proxy->m_context->DSSetShader
				(
					p_state.get_platform_pack().m_pipeline_state_proxy->m_config.m_domain_shader.get_platform_pack().m_shader,
					nullptr,
					0
				);
			}
			if (p_state.get_platform_pack().m_pipeline_state_proxy->m_config.m_geometry_shader != nullptr)
			{
				m_pack.m_pipeline_proxy->m_context->GSSetShader
				(
					p_state.get_platform_pack().m_pipeline_state_proxy->m_config.m_geometry_shader.get_platform_pack().m_shader,
					nullptr,
					0
				);
			}
			if (p_state.get_platform_pack().m_pipeline_state_proxy->m_config.m_pixel_shader != nullptr)
			{
				m_pack.m_pipeline_proxy->m_context->PSSetShader
				(
					p_state.get_platform_pack().m_pipeline_state_proxy->m_config.m_pixel_shader.get_platform_pack().m_shader,
					nullptr,
					0
				);
			}

			const core::bc_vector4f l_blend_factor = m_pack.m_pipeline_proxy->m_output_merger_stage.get_required_state().m_blend_factors.get();
			const bcFLOAT l_blend_factors[] = { l_blend_factor.x, l_blend_factor.y, l_blend_factor.z, l_blend_factor.w };
			const bcUINT l_stencil_ref = m_pack.m_pipeline_proxy->m_output_merger_stage.get_required_state().m_stencil_ref.get();

			m_pack.m_pipeline_proxy->m_context->OMSetBlendState
			(
				p_state.get_platform_pack().m_pipeline_state_proxy->m_blend_state,
				l_blend_factors,
				p_state.get_platform_pack().m_pipeline_state_proxy->m_config.m_sample_mask
			);
			m_pack.m_pipeline_proxy->m_context->OMSetDepthStencilState
			(
				p_state.get_platform_pack().m_pipeline_state_proxy->m_depth_stencil_state,
				l_stencil_ref
			);
			m_pack.m_pipeline_proxy->m_context->RSSetState(p_state.get_platform_pack().m_pipeline_state_proxy->m_rasterizer_state);
			m_pack.m_pipeline_proxy->m_context->IASetInputLayout(p_state.get_platform_pack().m_pipeline_state_proxy->m_input_layout);
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::unbind_pipeline_state()
		{
			m_pack.m_pipeline_proxy->m_context->VSSetShader
			(
				nullptr,
				nullptr,
				0
			);
			m_pack.m_pipeline_proxy->m_context->HSSetShader
			(
				nullptr,
				nullptr,
				0
			);
			m_pack.m_pipeline_proxy->m_context->DSSetShader
			(
				nullptr,
				nullptr,
				0
			);
			m_pack.m_pipeline_proxy->m_context->GSSetShader
			(
				nullptr,
				nullptr,
				0
			);
			m_pack.m_pipeline_proxy->m_context->PSSetShader
			(
				nullptr,
				nullptr,
				0
			);

			const bcFLOAT l_blend_factors[] = { 1, 1, 1, 1 };
			bcUINT l_sample_mask = 1;
			bcUINT l_stencil_ref = 1;

			m_pack.m_pipeline_proxy->m_context->OMSetBlendState
			(
				nullptr,
				l_blend_factors,
				l_sample_mask
			);
			m_pack.m_pipeline_proxy->m_context->OMSetDepthStencilState
			(
				nullptr,
				l_stencil_ref
			);
			m_pack.m_pipeline_proxy->m_context->RSSetState(nullptr);
			m_pack.m_pipeline_proxy->m_context->IASetInputLayout(nullptr);
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::bind_compute_state(bc_device_compute_state p_state)
		{
			BC_ASSERT(p_state.get_platform_pack().m_compute_state_proxy->m_config.m_compute_shader != nullptr);

			m_pack.m_pipeline_proxy->m_context->CSSetShader
			(
				p_state.get_platform_pack().m_compute_state_proxy->m_config.m_compute_shader.get_platform_pack().m_shader,
				nullptr,
				0
			);
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::unbind_compute_state()
		{
			m_pack.m_pipeline_proxy->m_context->CSSetShader
			(
				nullptr,
				nullptr,
				0
			);
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::bind_ia_primitive_topology(bc_primitive p_primitive)
		{
			m_pack.m_pipeline_proxy->m_input_assembler_stage.get_required_state().m_primitive_topology.set(p_primitive);
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::unbind_ia_primitive_topology()
		{
			m_pack.m_pipeline_proxy->m_input_assembler_stage.get_required_state().m_primitive_topology.set_to_initial_state();
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::bind_ia_index_buffer(bc_buffer p_buffer, bc_format p_format)
		{
			m_pack.m_pipeline_proxy->m_input_assembler_stage.get_required_state().m_index_buffer.set(p_buffer);
			m_pack.m_pipeline_proxy->m_input_assembler_stage.get_required_state().m_index_buffer_format.set(p_format);
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::unbind_ia_index_buffer()
		{
			m_pack.m_pipeline_proxy->m_input_assembler_stage.get_required_state().m_index_buffer.set_to_initial_state();
			m_pack.m_pipeline_proxy->m_input_assembler_stage.get_required_state().m_index_buffer_format.set_to_initial_state();
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::bind_ia_vertex_buffers(bcUINT p_start_slot, bcUINT p_buffer_count, const bc_buffer* p_buffers, bcUINT* p_strides, bcUINT* p_offsets)
		{
			BC_ASSERT(p_start_slot + p_buffer_count <= bc_render_api_info::number_of_ia_vertex_buffers());

			for (bcUINT32 l_c = p_start_slot; l_c < p_buffer_count; ++l_c)
			{
				m_pack.m_pipeline_proxy->m_input_assembler_stage.get_required_state().m_vertex_buffers.set(l_c, p_buffers[l_c]);
				m_pack.m_pipeline_proxy->m_input_assembler_stage.get_required_state().m_vertex_buffers_strides.set(l_c, p_strides[l_c]);
				m_pack.m_pipeline_proxy->m_input_assembler_stage.get_required_state().m_vertex_buffers_offsets.set(l_c, p_offsets[l_c]);
			}
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::unbind_ia_vertex_buffers(bcUINT p_start_slot, bcUINT p_buffer_count)
		{
			BC_ASSERT(p_start_slot + p_buffer_count <= bc_render_api_info::number_of_ia_vertex_buffers());

			for (bcUINT32 l_c = p_start_slot; l_c < p_buffer_count; ++l_c)
			{
				m_pack.m_pipeline_proxy->m_input_assembler_stage.get_required_state().m_vertex_buffers.set(l_c, bc_buffer());
				m_pack.m_pipeline_proxy->m_input_assembler_stage.get_required_state().m_vertex_buffers_strides.set(l_c, 0);
				m_pack.m_pipeline_proxy->m_input_assembler_stage.get_required_state().m_vertex_buffers_offsets.set(l_c, 0);
			}
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::bind_ps_constant_buffer_parameter(const bc_constant_buffer_parameter& p_parameter)
		{
			BC_ASSERT(p_parameter.get_register_index() <= bc_render_api_info::number_of_shader_constant_buffer());

			const bool l_vertex_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::vertex);
			const bool l_hull_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::hull);
			const bool l_domain_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::domain);
			const bool l_geometry_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::geometry);
			const bool l_pixel_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::pixel);
			const bool l_compute_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::compute);

			bc_buffer l_buffer = p_parameter.get_buffer();

			if (l_vertex_shader)
			{
				m_pack.m_pipeline_proxy->m_shader_stages[0]
					->get_required_state()
					.m_constant_buffers
					.set(p_parameter.get_register_index(), l_buffer);
			}
			if (l_hull_shader)
			{
				m_pack.m_pipeline_proxy->m_shader_stages[1]
					->get_required_state()
					.m_constant_buffers
					.set(p_parameter.get_register_index(), l_buffer);
			}
			if (l_domain_shader)
			{
				m_pack.m_pipeline_proxy->m_shader_stages[2]
					->get_required_state()
					.m_constant_buffers
					.set(p_parameter.get_register_index(), l_buffer);
			}
			if (l_geometry_shader)
			{
				m_pack.m_pipeline_proxy->m_shader_stages[3]
					->get_required_state()
					.m_constant_buffers
					.set(p_parameter.get_register_index(), l_buffer);
			}
			if (l_pixel_shader)
			{
				m_pack.m_pipeline_proxy->m_shader_stages[4]
					->get_required_state()
					.m_constant_buffers
					.set(p_parameter.get_register_index(), l_buffer);
			}
			if (l_compute_shader)
			{
				m_pack.m_pipeline_proxy->m_shader_stages[5]
					->get_required_state()
					.m_constant_buffers
					.set(p_parameter.get_register_index(), l_buffer);
			}
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::unbind_ps_constant_buffer_parameter(const bc_constant_buffer_parameter& p_parameter)
		{
			BC_ASSERT(p_parameter.get_register_index() <= bc_render_api_info::number_of_shader_constant_buffer());

			const bool l_vertex_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::vertex);
			const bool l_hull_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::hull);
			const bool l_domain_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::domain);
			const bool l_geometry_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::geometry);
			const bool l_pixel_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::pixel);
			const bool l_compute_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::compute);

			if (l_vertex_shader)
			{
				m_pack.m_pipeline_proxy->m_shader_stages[0]
					->get_required_state()
					.m_constant_buffers
					.set(p_parameter.get_register_index(), bc_buffer());
			}
			if (l_hull_shader)
			{
				m_pack.m_pipeline_proxy->m_shader_stages[1]
					->get_required_state()
					.m_constant_buffers
					.set(p_parameter.get_register_index(), bc_buffer());
			}
			if (l_domain_shader)
			{
				m_pack.m_pipeline_proxy->m_shader_stages[2]
					->get_required_state()
					.m_constant_buffers
					.set(p_parameter.get_register_index(), bc_buffer());
			}
			if (l_geometry_shader)
			{
				m_pack.m_pipeline_proxy->m_shader_stages[3]
					->get_required_state()
					.m_constant_buffers
					.set(p_parameter.get_register_index(), bc_buffer());
			}
			if (l_pixel_shader)
			{
				m_pack.m_pipeline_proxy->m_shader_stages[4]
					->get_required_state()
					.m_constant_buffers
					.set(p_parameter.get_register_index(), bc_buffer());
			}
			if (l_compute_shader)
			{
				m_pack.m_pipeline_proxy->m_shader_stages[5]
					->get_required_state()
					.m_constant_buffers
					.set(p_parameter.get_register_index(), bc_buffer());
			}
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::bind_ps_sampler_parameter(const bc_sampler_parameter& p_parameter)
		{
			BC_ASSERT(p_parameter.get_register_index() <= bc_render_api_info::number_of_shader_sampler());

			const bool l_vertex_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::vertex);
			const bool l_hull_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::hull);
			const bool l_domain_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::domain);
			const bool l_geometry_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::geometry);
			const bool l_pixel_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::pixel);
			const bool l_compute_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::compute);

			bc_sampler_state l_sampler = p_parameter.get_sampler();

			if (l_vertex_shader)
			{
				m_pack.m_pipeline_proxy->m_shader_stages[0]
					->get_required_state()
					.m_sampler_states
					.set(p_parameter.get_register_index(), l_sampler);
			}
			if (l_hull_shader)
			{
				m_pack.m_pipeline_proxy->m_shader_stages[1]
					->get_required_state()
					.m_sampler_states
					.set(p_parameter.get_register_index(), l_sampler);
			}
			if (l_domain_shader)
			{
				m_pack.m_pipeline_proxy->m_shader_stages[2]
					->get_required_state()
					.m_sampler_states
					.set(p_parameter.get_register_index(), l_sampler);
			}
			if (l_geometry_shader)
			{
				m_pack.m_pipeline_proxy->m_shader_stages[3]
					->get_required_state()
					.m_sampler_states
					.set(p_parameter.get_register_index(), l_sampler);
			}
			if (l_pixel_shader)
			{
				m_pack.m_pipeline_proxy->m_shader_stages[4]
					->get_required_state()
					.m_sampler_states
					.set(p_parameter.get_register_index(), l_sampler);
			}
			if (l_compute_shader)
			{
				m_pack.m_pipeline_proxy->m_shader_stages[5]
					->get_required_state()
					.m_sampler_states
					.set(p_parameter.get_register_index(), l_sampler);
			}
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::unbind_ps_sampler_parameter(const bc_sampler_parameter& p_parameter)
		{
			BC_ASSERT(p_parameter.get_register_index() <= bc_render_api_info::number_of_shader_sampler());

			const bool l_vertex_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::vertex);
			const bool l_hull_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::hull);
			const bool l_domain_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::domain);
			const bool l_geometry_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::geometry);
			const bool l_pixel_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::pixel);
			const bool l_compute_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::compute);

			if (l_vertex_shader)
			{
				m_pack.m_pipeline_proxy->m_shader_stages[0]
					->get_required_state()
					.m_sampler_states
					.set(p_parameter.get_register_index(), bc_sampler_state());
			}
			if (l_hull_shader)
			{
				m_pack.m_pipeline_proxy->m_shader_stages[1]
					->get_required_state()
					.m_sampler_states
					.set(p_parameter.get_register_index(), bc_sampler_state());
			}
			if (l_domain_shader)
			{
				m_pack.m_pipeline_proxy->m_shader_stages[2]
					->get_required_state()
					.m_sampler_states
					.set(p_parameter.get_register_index(), bc_sampler_state());
			}
			if (l_geometry_shader)
			{
				m_pack.m_pipeline_proxy->m_shader_stages[3]
					->get_required_state()
					.m_sampler_states
					.set(p_parameter.get_register_index(), bc_sampler_state());
			}
			if (l_pixel_shader)
			{
				m_pack.m_pipeline_proxy->m_shader_stages[4]
					->get_required_state()
					.m_sampler_states
					.set(p_parameter.get_register_index(), bc_sampler_state());
			}
			if (l_compute_shader)
			{
				m_pack.m_pipeline_proxy->m_shader_stages[5]
					->get_required_state()
					.m_sampler_states
					.set(p_parameter.get_register_index(), bc_sampler_state());
			}
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::bind_ps_shader_view_parameter(const bc_resource_view_parameter& p_parameter)
		{
			const bc_resource_view l_resource = p_parameter.get_resource_view();

			BC_ASSERT
			(
				l_resource.get_view_type() == bc_resource_view_type::shader ?
				p_parameter.get_register_index() <= bc_render_api_info::number_of_shader_resource() :
				p_parameter.get_register_index() <= bc_render_api_info::number_of_ps_cs_uav_resource()
			);

			const bool l_vertex_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::vertex);
			const bool l_hull_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::hull);
			const bool l_domain_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::domain);
			const bool l_geometry_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::geometry);
			const bool l_pixel_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::pixel);
			const bool l_compute_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::compute);

			if (l_vertex_shader)
			{
				if (l_resource.get_view_type() == bc_resource_view_type::shader)
				{
					m_pack.m_pipeline_proxy->m_shader_stages[0]
						->get_required_state()
						.m_shader_resource_views
						.set(p_parameter.get_register_index(), l_resource);
				}
			}
			if (l_hull_shader)
			{
				if (l_resource.get_view_type() == bc_resource_view_type::shader)
				{
					m_pack.m_pipeline_proxy->m_shader_stages[1]
						->get_required_state()
						.m_shader_resource_views
						.set(p_parameter.get_register_index(), l_resource);
				}
			}
			if (l_domain_shader)
			{
				if (l_resource.get_view_type() == bc_resource_view_type::shader)
				{
					m_pack.m_pipeline_proxy->m_shader_stages[2]
						->get_required_state()
						.m_shader_resource_views
						.set(p_parameter.get_register_index(), l_resource);
				}
			}
			if (l_geometry_shader)
			{
				if (l_resource.get_view_type() == bc_resource_view_type::shader)
				{
					m_pack.m_pipeline_proxy->m_shader_stages[3]
						->get_required_state()
						.m_shader_resource_views
						.set(p_parameter.get_register_index(), l_resource);
				}
			}
			if (l_pixel_shader)
			{
				if (l_resource.get_view_type() == bc_resource_view_type::shader)
				{
					m_pack.m_pipeline_proxy->m_shader_stages[4]
						->get_required_state()
						.m_shader_resource_views
						.set(p_parameter.get_register_index(), l_resource);
				}
				else if(l_resource.get_view_type() == bc_resource_view_type::unordered)
				{
					m_pack.m_pipeline_proxy->m_shader_stages[4]
						->get_required_state()
						.m_unordered_access_views
						.set(p_parameter.get_register_index(), l_resource);
				}
			}
			if (l_compute_shader)
			{
				if (l_resource.get_view_type() == bc_resource_view_type::shader)
				{
					m_pack.m_pipeline_proxy->m_shader_stages[5]
						->get_required_state()
						.m_shader_resource_views
						.set(p_parameter.get_register_index(), l_resource);
				}
				else if (l_resource.get_view_type() == bc_resource_view_type::unordered)
				{
					m_pack.m_pipeline_proxy->m_shader_stages[5]
						->get_required_state()
						.m_unordered_access_views
						.set(p_parameter.get_register_index(), l_resource);
				}
			}
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::unbind_ps_shader_view_parameter(const bc_resource_view_parameter& p_parameter)
		{
			bc_resource_view l_resource = p_parameter.get_resource_view();

			BC_ASSERT
			(
				l_resource.get_view_type() == bc_resource_view_type::shader ?
				p_parameter.get_register_index() <= bc_render_api_info::number_of_shader_resource() :
				p_parameter.get_register_index() <= bc_render_api_info::number_of_ps_cs_uav_resource()
			);

			const bool l_vertex_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::vertex);
			const bool l_hull_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::hull);
			const bool l_domain_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::domain);
			const bool l_geometry_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::geometry);
			const bool l_pixel_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::pixel);
			const bool l_compute_shader = core::bc_enum::has(p_parameter.get_shader_types(), bc_shader_type::compute);

			if (l_vertex_shader)
			{
				if (l_resource.get_view_type() == bc_resource_view_type::shader)
				{
					m_pack.m_pipeline_proxy->m_shader_stages[0]
						->get_required_state()
						.m_shader_resource_views
						.set(p_parameter.get_register_index(), bc_resource_view());
				}
			}
			if (l_hull_shader)
			{
				if (l_resource.get_view_type() == bc_resource_view_type::shader)
				{
					m_pack.m_pipeline_proxy->m_shader_stages[1]
						->get_required_state()
						.m_shader_resource_views
						.set(p_parameter.get_register_index(), bc_resource_view());
				}
			}
			if (l_domain_shader)
			{
				if (l_resource.get_view_type() == bc_resource_view_type::shader)
				{
					m_pack.m_pipeline_proxy->m_shader_stages[2]
						->get_required_state()
						.m_shader_resource_views
						.set(p_parameter.get_register_index(), bc_resource_view());
				}
			}
			if (l_geometry_shader)
			{
				if (l_resource.get_view_type() == bc_resource_view_type::shader)
				{
					m_pack.m_pipeline_proxy->m_shader_stages[3]
						->get_required_state()
						.m_shader_resource_views
						.set(p_parameter.get_register_index(), bc_resource_view());
				}
			}
			if (l_pixel_shader)
			{
				if (l_resource.get_view_type() == bc_resource_view_type::shader)
				{
					m_pack.m_pipeline_proxy->m_shader_stages[4]
						->get_required_state()
						.m_shader_resource_views
						.set(p_parameter.get_register_index(), bc_resource_view());
				}
				else if (l_resource.get_view_type() == bc_resource_view_type::unordered)
				{
					m_pack.m_pipeline_proxy->m_shader_stages[4]
						->get_required_state()
						.m_unordered_access_views
						.set(p_parameter.get_register_index(), bc_resource_view());
				}
			}
			if (l_compute_shader)
			{
				if (l_resource.get_view_type() == bc_resource_view_type::shader)
				{
					m_pack.m_pipeline_proxy->m_shader_stages[5]
						->get_required_state()
						.m_shader_resource_views
						.set(p_parameter.get_register_index(), bc_resource_view());
				}
				else if (l_resource.get_view_type() == bc_resource_view_type::unordered)
				{
					m_pack.m_pipeline_proxy->m_shader_stages[5]
						->get_required_state()
						.m_unordered_access_views
						.set(p_parameter.get_register_index(), bc_resource_view());
				}
			}
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::bind_os_stream_outputs(bcUINT p_buffer_count, const bc_buffer* p_buffers, bcUINT* p_offsets)
		{
			BC_ASSERT(p_buffer_count <= bc_render_api_info::number_of_so_streams());

			m_pack.m_pipeline_proxy->m_stream_output_stage.get_required_state().m_stream_buffers.set_to_initial_state();
			m_pack.m_pipeline_proxy->m_stream_output_stage.get_required_state().m_stream_offsets.set_to_initial_state();

			for (bcUINT32 l_c = 0; l_c < p_buffer_count; ++l_c)
			{
				m_pack.m_pipeline_proxy->m_stream_output_stage.get_required_state().m_stream_buffers.set(l_c, p_buffers[l_c]);
				m_pack.m_pipeline_proxy->m_stream_output_stage.get_required_state().m_stream_offsets.set(l_c, p_offsets[l_c]);
			}
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::unbind_os_stream_outputs()
		{
			m_pack.m_pipeline_proxy->m_stream_output_stage.get_required_state().m_stream_buffers.set_to_initial_state();
			m_pack.m_pipeline_proxy->m_stream_output_stage.get_required_state().m_stream_offsets.set_to_initial_state();
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::bind_rs_viewports(bcUINT p_count, const bc_viewport* p_viewports)
		{
			BC_ASSERT(p_count <= bc_render_api_info::number_of_rs_viewport_scissorrect());

			m_pack.m_pipeline_proxy->m_rasterizer_stage.get_required_state().m_viewports.set_to_initial_state();

			for (bcUINT32 l_c = 0; l_c < p_count; ++l_c)
			{
				m_pack.m_pipeline_proxy->m_rasterizer_stage.get_required_state().m_viewports.set(l_c, &p_viewports[l_c]);
			}
			m_pack.m_pipeline_proxy->m_rasterizer_stage.get_required_state().m_viewport_count.set(p_count);
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::unbind_rs_viewports()
		{
			m_pack.m_pipeline_proxy->m_rasterizer_stage.get_required_state().m_viewports.set_to_initial_state();
			m_pack.m_pipeline_proxy->m_rasterizer_stage.get_required_state().m_viewport_count.set_to_initial_state();
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::bind_om_blend_factors(const core::bc_vector4f& p_factors)
		{
			m_pack.m_pipeline_proxy->m_output_merger_stage.get_required_state().m_blend_factors.set(p_factors);
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::bind_om_stencil_ref(bcUINT32 p_stencil_ref)
		{
			m_pack.m_pipeline_proxy->m_output_merger_stage.get_required_state().m_stencil_ref.set(p_stencil_ref);
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::bind_om_render_targets(bcUINT p_target_count, const bc_render_target_view* p_targets, bc_depth_stencil_view p_depth)
		{
			BC_ASSERT(p_target_count <= bc_render_api_info::number_of_om_render_target_slots());

			m_pack.m_pipeline_proxy->m_output_merger_stage.get_required_state().m_render_target_views.set_to_initial_state();
			m_pack.m_pipeline_proxy->m_output_merger_stage.get_required_state().m_depth_target_view.set_to_initial_state();

			for (bcUINT l_index = 0; l_index < p_target_count; ++l_index)
			{
				m_pack.m_pipeline_proxy->m_output_merger_stage.get_required_state().m_render_target_views.set(l_index, p_targets[l_index]);
			}
			m_pack.m_pipeline_proxy->m_output_merger_stage.get_required_state().m_depth_target_view.set(p_depth);
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::unbind_om_render_targets()
		{
			m_pack.m_pipeline_proxy->m_output_merger_stage.get_required_state().m_render_target_views.set_to_initial_state();
			m_pack.m_pipeline_proxy->m_output_merger_stage.get_required_state().m_depth_target_view.set_to_initial_state();
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::pipeline_apply_states(bc_pipeline_stage p_stages, const bc_compute_stage::initial_counts_array* p_initial_counts)
		{
			const bool l_input_assembler_stage = core::bc_enum::has(p_stages, bc_pipeline_stage::input_assembler_stage);
			const bool l_vertex_stage = core::bc_enum::has(p_stages, bc_pipeline_stage::vertex_stage);
			const bool l_hull_stage = core::bc_enum::has(p_stages, bc_pipeline_stage::hull_stage);
			const bool l_domain_stage = core::bc_enum::has(p_stages, bc_pipeline_stage::domain_stage);
			const bool l_geometry_stage = core::bc_enum::has(p_stages, bc_pipeline_stage::geometry_stage);
			const bool l_pixel_stage = core::bc_enum::has(p_stages, bc_pipeline_stage::pixel_stage);
			const bool l_compute_stage = core::bc_enum::has(p_stages, bc_pipeline_stage::compute_stage);
			const bool l_stream_output_stage = core::bc_enum::has(p_stages, bc_pipeline_stage::stream_output_stage);
			const bool l_rasterizer_stage = core::bc_enum::has(p_stages, bc_pipeline_stage::rasterizer_stage);
			const bool l_output_merger_stage = core::bc_enum::has(p_stages, bc_pipeline_stage::output_merger_stage);

			if (l_input_assembler_stage)
			{
				m_pack.m_pipeline_proxy->m_input_assembler_stage.apply_required_state(*this);
			}
			if (l_vertex_stage)
			{
				m_pack.m_pipeline_proxy->m_vertex_shader_stage.apply_required_state(*this);
			}
			if (l_hull_stage)
			{
				m_pack.m_pipeline_proxy->m_hull_shader_stage.apply_required_state(*this);
			}
			if (l_domain_stage)
			{
				m_pack.m_pipeline_proxy->m_domain_shader_stage.apply_required_state(*this);
			}
			if (l_geometry_stage)
			{
				m_pack.m_pipeline_proxy->m_geometry_shader_stage.apply_required_state(*this);
			}
			if (l_pixel_stage)
			{
				m_pack.m_pipeline_proxy->m_pixel_shader_stage.apply_required_state(*this);
			}
			if (l_compute_stage)
			{
				m_pack.m_pipeline_proxy->m_compute_shader_stage.apply_required_state(*this, p_initial_counts);
			}
			if (l_stream_output_stage)
			{
				m_pack.m_pipeline_proxy->m_stream_output_stage.apply_required_state(*this);
			}
			if (l_rasterizer_stage)
			{
				m_pack.m_pipeline_proxy->m_rasterizer_stage.apply_required_state(*this);
			}
			if (l_output_merger_stage)
			{
				m_pack.m_pipeline_proxy->m_output_merger_stage.apply_required_state(*this, m_pack.m_pipeline_proxy->m_pixel_shader_stage);
			}
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::pipeline_set_default_states(bc_pipeline_stage p_stages)
		{
			const bool l_input_assembler_stage = core::bc_enum::has(p_stages, bc_pipeline_stage::input_assembler_stage);
			const bool l_vertex_stage = core::bc_enum::has(p_stages, bc_pipeline_stage::vertex_stage);
			const bool l_hull_stage = core::bc_enum::has(p_stages, bc_pipeline_stage::hull_stage);
			const bool l_domain_stage = core::bc_enum::has(p_stages, bc_pipeline_stage::domain_stage);
			const bool l_geometry_stage = core::bc_enum::has(p_stages, bc_pipeline_stage::geometry_stage);
			const bool l_pixel_stage = core::bc_enum::has(p_stages, bc_pipeline_stage::pixel_stage);
			const bool l_compute_stage = core::bc_enum::has(p_stages, bc_pipeline_stage::compute_stage);
			const bool l_stream_output_stage = core::bc_enum::has(p_stages, bc_pipeline_stage::stream_output_stage);
			const bool l_rasterizer_stage = core::bc_enum::has(p_stages, bc_pipeline_stage::rasterizer_stage);
			const bool l_output_merger_stage = core::bc_enum::has(p_stages, bc_pipeline_stage::output_merger_stage);

			if (l_input_assembler_stage)
			{
				m_pack.m_pipeline_proxy->m_input_assembler_stage.set_to_default_state(*this);
			}
			if (l_vertex_stage)
			{
				m_pack.m_pipeline_proxy->m_vertex_shader_stage.set_to_default_state(*this);
			}
			if (l_hull_stage)
			{
				m_pack.m_pipeline_proxy->m_hull_shader_stage.set_to_default_state(*this);
			}
			if (l_domain_stage)
			{
				m_pack.m_pipeline_proxy->m_domain_shader_stage.set_to_default_state(*this);
			}
			if (l_geometry_stage)
			{
				m_pack.m_pipeline_proxy->m_geometry_shader_stage.set_to_default_state(*this);
			}
			if (l_pixel_stage)
			{
				m_pack.m_pipeline_proxy->m_pixel_shader_stage.set_to_default_state(*this);
			}
			if (l_compute_stage)
			{
				m_pack.m_pipeline_proxy->m_compute_shader_stage.set_to_default_state(*this);
			}
			if (l_stream_output_stage)
			{
				m_pack.m_pipeline_proxy->m_stream_output_stage.set_to_default_state(*this);
			}
			if (l_rasterizer_stage)
			{
				m_pack.m_pipeline_proxy->m_rasterizer_stage.set_to_default_state(*this);
			}
			if (l_output_merger_stage)
			{
				m_pack.m_pipeline_proxy->m_output_merger_stage.set_to_default_state(*this);
			}
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::draw(bcUINT p_start_vertex, bcUINT p_vertex_count)
		{
			m_pack.m_pipeline_proxy->m_context->Draw(p_vertex_count, p_start_vertex);
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::draw_instanced(bcUINT p_vertex_count_per_instance,
			bcUINT p_instance_count,
			bcUINT p_start_vertex_location,
			bcUINT p_start_instance_location)
		{
			m_pack.m_pipeline_proxy->m_context->DrawInstanced
			(
				p_vertex_count_per_instance,
				p_instance_count,
				p_start_vertex_location,
				p_start_instance_location
			);
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::draw_instanced_indirect(bc_buffer p_args_buffer, bcUINT p_offset)
		{
			m_pack.m_pipeline_proxy->m_context->DrawInstancedIndirect(p_args_buffer.get_platform_pack().m_buffer, p_offset);
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::draw_indexed(bcUINT p_start_index, bcUINT p_index_count, bcINT p_vertex_offset)
		{
			m_pack.m_pipeline_proxy->m_context->DrawIndexed(p_index_count, p_start_index, p_vertex_offset);
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::draw_indexed_instanced(bcUINT p_index_count_per_instance,
			bcUINT p_instance_count,
			bcUINT p_start_index_location,
			bcINT p_base_vertex_location,
			bcUINT p_start_instance_location)
		{
			m_pack.m_pipeline_proxy->m_context->DrawIndexedInstanced
			(
				p_index_count_per_instance,
				p_instance_count,
				p_start_index_location,
				p_base_vertex_location,
				p_start_instance_location
			);
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::draw_indexed_instanced_indirect(bc_buffer p_args_buffer, bcUINT p_offset)
		{
			m_pack.m_pipeline_proxy->m_context->DrawIndexedInstancedIndirect(p_args_buffer.get_platform_pack().m_buffer, p_offset);
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::dispatch(bcUINT p_x, bcUINT p_y, bcUINT p_z)
		{
			m_pack.m_pipeline_proxy->m_context->Dispatch(p_x, p_y, p_z);
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::dispatch_indirect(bc_buffer p_args, bcUINT p_offset)
		{
			m_pack.m_pipeline_proxy->m_context->DispatchIndirect(p_args.get_platform_pack().m_buffer, p_offset);
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::clear_buffers(core::bc_vector4f p_color, bcFLOAT p_depth, bcUINT p_stencil)
		{
			constexpr bcUINT32 l_target_count = bc_render_api_info::number_of_om_render_target_slots();
			ComPtr< ID3D11RenderTargetView > l_target_views[l_target_count];
			ComPtr< ID3D11DepthStencilView > l_depth_view;

			m_pack.m_pipeline_proxy->m_context->OMGetRenderTargets(l_target_count, l_target_views[0].GetAddressOf(), l_depth_view.GetAddressOf());

			bcFLOAT l_colors[] = { p_color.x, p_color.y, p_color.z, p_color.w };
			for (bcUINT i = 0; i < l_target_count; ++i)
			{
				ID3D11RenderTargetView* l_render_target = l_target_views[i].Get();
				if (l_render_target != nullptr)
				{
					m_pack.m_pipeline_proxy->m_context->ClearRenderTargetView(l_render_target, l_colors);
				}
			}

			if (l_depth_view != nullptr)
			{
				m_pack.m_pipeline_proxy->m_context->ClearDepthStencilView(l_depth_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, p_depth, p_stencil);
			}
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::update_subresource(bci_resource& p_resource, bcUINT p_dst_subresource, const void* p_src_data, bcUINT p_src_row_pitch, bcUINT p_src_depth_pitch)
		{
			m_pack.m_pipeline_proxy->m_context->UpdateSubresource
			(
				p_resource.get_platform_pack().m_resource,
				p_dst_subresource,
				nullptr,
				p_src_data,
				p_src_row_pitch,
				p_src_depth_pitch
			);
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::copy_subresource(bci_resource& p_dest_resource, bcUINT p_dst_subresource, bci_resource& p_src_resource, bcUINT p_src_subresource)
		{
			m_pack.m_pipeline_proxy->m_context->CopySubresourceRegion
			(
				p_dest_resource.get_platform_pack().m_resource,
				p_dst_subresource,
				0,
				0,
				0,
				p_src_resource.get_platform_pack().m_resource,
				p_src_subresource,
				nullptr
			);
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::copy_resource(bci_resource& p_dest_resource, bci_resource& p_src_resource)
		{
			m_pack.m_pipeline_proxy->m_context->CopyResource
			(
				p_dest_resource.get_platform_pack().m_resource,
				p_src_resource.get_platform_pack().m_resource
			);
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::copy_structure_count(bc_buffer p_dest_resource, bcUINT p_offset, bc_resource_view p_unordered_resource)
		{
			BC_ASSERT(p_unordered_resource.get_view_type() == bc_resource_view_type::unordered);

			m_pack.m_pipeline_proxy->m_context->CopyStructureCount
			(
				p_dest_resource.get_platform_pack().m_buffer,
				p_offset,
				p_unordered_resource.get_platform_pack().m_unordered_shader_view
			);
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::resolve_subresource(bci_resource& p_dest_resource, 
			bcUINT p_dest_subresource, 
			bci_resource& p_src_resource, 
			bcUINT p_src_subresource, 
			bc_format p_format)
		{
			m_pack.m_pipeline_proxy->m_context->ResolveSubresource
			(
				p_dest_resource.get_platform_pack().m_resource,
				p_dest_subresource,
				p_src_resource.get_platform_pack().m_resource,
				p_src_subresource,
				bc_graphic_cast(p_format)
			);
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::start_command_list()
		{
			if (m_pack.m_pipeline_proxy->m_context_type == D3D11_DEVICE_CONTEXT_IMMEDIATE)
			{
				auto& l_device_pack = m_pack.m_pipeline_proxy->m_device->get_platform_pack();
				l_device_pack.m_immediate_context_mutex.lock();
			}
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::finish_command_list(bc_device_command_list& p_command_list)
		{
			if (m_pack.m_pipeline_proxy->m_context_type == D3D11_DEVICE_CONTEXT_IMMEDIATE)
			{
				auto& l_device_pack = m_pack.m_pipeline_proxy->m_device->get_platform_pack();
				l_device_pack.m_immediate_context_mutex.unlock();
			}
			else
			{
				p_command_list.finished();
				dx_call(m_pack.m_pipeline_proxy->m_context->FinishCommandList
				(
					false,
					&p_command_list.get_platform_pack().m_command_list_proxy->m_command_list
				));

				m_pack.m_pipeline_proxy->m_input_assembler_stage.set_to_default_state(*this);
				m_pack.m_pipeline_proxy->m_stream_output_stage.set_to_default_state(*this);
				m_pack.m_pipeline_proxy->m_rasterizer_stage.set_to_default_state(*this);
				m_pack.m_pipeline_proxy->m_output_merger_stage.set_to_default_state(*this);
				m_pack.m_pipeline_proxy->m_vertex_shader_stage.set_to_default_state(*this);
				m_pack.m_pipeline_proxy->m_hull_shader_stage.set_to_default_state(*this);
				m_pack.m_pipeline_proxy->m_domain_shader_stage.set_to_default_state(*this);
				m_pack.m_pipeline_proxy->m_geometry_shader_stage.set_to_default_state(*this);
				m_pack.m_pipeline_proxy->m_pixel_shader_stage.set_to_default_state(*this);
				m_pack.m_pipeline_proxy->m_compute_shader_stage.set_to_default_state(*this);
			}
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline< g_api_dx11 >::start_monitoring_pipeline()
		{
			m_pack.m_pipeline_proxy->m_context->Begin(m_pack.m_pipeline_proxy->m_query);
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_device_pipeline_statistic bc_platform_device_pipeline< g_api_dx11 >::end_monitoring_pipeline()
		{
			bc_device_pipeline_statistic l_result;
			D3D11_QUERY_DATA_PIPELINE_STATISTICS l_statistics;

			m_pack.m_pipeline_proxy->m_context->End(m_pack.m_pipeline_proxy->m_query);
			dx_call
			(
				m_pack.m_pipeline_proxy->m_context->GetData(m_pack.m_pipeline_proxy->m_query, &l_statistics, sizeof(D3D11_QUERY_DATA_PIPELINE_STATISTICS), 0)
			);

			l_result.m_ia_vertices = l_statistics.IAVertices;
			l_result.m_ia_primitives = l_statistics.IAPrimitives;
			l_result.m_vs_invocations = l_statistics.VSInvocations;
			l_result.m_hs_invocations = l_statistics.HSInvocations;
			l_result.m_ds_invocations = l_statistics.DSInvocations;
			l_result.m_gs_invocations = l_statistics.GSInvocations;
			l_result.m_gs_primitives = l_statistics.GSPrimitives;
			l_result.m_ps_invocations = l_statistics.PSInvocations;
			l_result.m_cs_invocations = l_statistics.CSInvocations;
			l_result.m_r_invocations = l_statistics.CInvocations;
			l_result.m_r_primitives = l_statistics.CPrimitives;

			return l_result;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_device_pipeline<g_api_dx11>::is_valid() const noexcept
		{
			return m_pack.m_pipeline_proxy != nullptr && m_pack.m_pipeline_proxy->m_context != nullptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_pipeline<g_api_dx11>::set_debug_name(const bcCHAR* p_name) noexcept
		{
			if (is_valid())
			{
				m_pack.m_pipeline_proxy->m_context->SetPrivateData(WKPDID_D3DDebugObjectName, std::strlen(p_name), p_name);
			}
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_pipeline_type bc_platform_device_pipeline<g_api_dx11>::get_type() const noexcept
		{
			const auto l_context_type = m_pack.m_pipeline_proxy->m_context->GetType();
			switch (l_context_type)
			{
			case D3D11_DEVICE_CONTEXT_IMMEDIATE:
				return bc_pipeline_type::default;
			case D3D11_DEVICE_CONTEXT_DEFERRED:
				return bc_pipeline_type::deferred;
			default: 
				BC_ASSERT(false);
			}
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_device_pipeline<g_api_dx11>::operator==(const bc_platform_device_pipeline& p_other) const noexcept
		{
			return m_pack.m_pipeline_proxy == p_other.m_pack.m_pipeline_proxy;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_device_pipeline<g_api_dx11>::operator!=(const bc_platform_device_pipeline& p_other) const noexcept
		{
			return !operator==(p_other);
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_device_pipeline<g_api_dx11>::operator==(std::nullptr_t) const noexcept
		{
			return !is_valid();
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_device_pipeline<g_api_dx11>::operator!=(std::nullptr_t) const noexcept
		{
			return is_valid();
		}
	}
}