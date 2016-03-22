// [01/20/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcPlatformRenderApi.h"
#include "Graphic/bcRenderApiInfo.h"
#include "Graphic/bcDeviceObject.h"
#include "Graphic/bcResourcePtr.h"
#include "Graphic/Resource/State/bcBlendState.h"
#include "Graphic/Resource/State/bcDepthStencilState.h"
#include "Graphic/Resource/State/bcRasterizerState.h"
#include "Graphic/Resource/State/bcInputLayout.h"
#include "Graphic/Shader/bcVertexShader.h"
#include "Graphic/Shader/bcHullShader.h"
#include "Graphic/Shader/bcDomainShader.h"
#include "Graphic/Shader/bcGeometryShader.h"
#include "Graphic/Shader/bcPixelShader.h"
#include "Graphic/Shader/bcComputeShader.h"

namespace black_cat
{
	namespace graphic
	{
		struct bc_device_pipeline_state_config
		{
			bc_device_pipeline_state_config()
				: m_sample_config(1, 0)
			{
			}

			bc_vertex_shader_ptr m_vertex_shader;
			bc_hull_shader_ptr m_hull_shader;
			bc_domain_shader_ptr m_domain_shader;
			bc_geometry_shader_ptr m_geometry_shader;
			bc_pixel_shader_ptr m_pixel_shader;

			bc_blend_state_config m_blend_state_config;
			bcUINT m_sample_mask;
			bc_depth_stencil_state_config m_depth_stencil_state_config;
			bc_rasterizer_state_config m_rasterizer_state_config;
			bc_input_layout_config m_input_layout_config;
			bcUINT m_num_render_target;
			bc_format m_render_target_format[bc_render_api_info::number_of_om_render_target_slots()];
			bc_format m_depth_stencil_format;
			bc_texture_ms_config m_sample_config;
		};

		template< bc_platform_render_api TRenderApi >
		struct bc_platform_device_pipeline_state_pack
		{
			bc_device_pipeline_state_config m_config;
		};

		template< bc_platform_render_api TRenderApi >
		class bc_platform_device_pipeline_state : public bc_device_object
		{
		public:
			using platform_pack = bc_platform_device_pipeline_state_pack<TRenderApi>;

		public:
			bc_platform_device_pipeline_state();

			bc_platform_device_pipeline_state(bc_platform_device_pipeline_state&&);

			~bc_platform_device_pipeline_state();

			bc_platform_device_pipeline_state& operator=(bc_platform_device_pipeline_state&&);

			bc_device_pipeline_state_config& get_config()
			{
				return m_pack.m_config;
			}

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			platform_pack m_pack;
		};

		using bc_device_pipeline_state = bc_platform_device_pipeline_state<g_current_platform_render_api>;

		using bc_device_pipeline_state_ptr = bc_resource_ptr<bc_device_pipeline_state>;
	}
}