// [01/20/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcRenderApi.h"
#include "Graphic/bcRenderApiInfo.h"
#include "Graphic/bcDeviceReference.h"
#include "Graphic/bcDeviceRef.h"
#include "Graphic/Resource/State/bcBlendState.h"
#include "Graphic/Resource/State/bcDepthStencilState.h"
#include "Graphic/Resource/State/bcRasterizerState.h"
#include "Graphic/Resource/State/bcInputLayout.h"
#include "Graphic/Shader/bcVertexShader.h"
#include "Graphic/Shader/bcHullShader.h"
#include "Graphic/Shader/bcDomainShader.h"
#include "Graphic/Shader/bcGeometryShader.h"
#include "Graphic/Shader/bcPixelShader.h"

namespace black_cat
{
	namespace graphic
	{
		struct bc_device_pipeline_state_config
		{
			bc_device_pipeline_state_config()
				: m_blend_state_config(),
				m_sample_mask(0),
				m_depth_stencil_state_config(),
				m_rasterizer_state_config(),
				m_num_render_target(0),
				m_render_target_format(),
				m_depth_stencil_format(),
				m_sample_config(1, 0)
			{
			}

			bc_vertex_shader m_vertex_shader;
			bc_hull_shader m_hull_shader;
			bc_domain_shader m_domain_shader;
			bc_geometry_shader m_geometry_shader;
			bc_pixel_shader m_pixel_shader;

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

		template<bc_render_api TRenderApi>
		struct bc_platform_device_pipeline_state_pack
		{
		};

		template<bc_render_api TRenderApi>
		class bc_platform_device_pipeline_state : public bc_platform_device_reference<TRenderApi>
		{
		public:
			using platform_pack = bc_platform_device_pipeline_state_pack<TRenderApi>;

		public:
			bc_platform_device_pipeline_state() noexcept;

			explicit bc_platform_device_pipeline_state(platform_pack& p_pack) noexcept;
			
			bc_platform_device_pipeline_state(const bc_platform_device_pipeline_state&) noexcept;

			~bc_platform_device_pipeline_state() override;

			bc_platform_device_pipeline_state& operator=(const bc_platform_device_pipeline_state&) noexcept;

			const bc_device_pipeline_state_config& get_config() const;

			bool is_valid() const noexcept override;

			void set_debug_name(const bcCHAR* p_name) noexcept override;
			
			bool operator==(const bc_platform_device_pipeline_state& p_other) const noexcept;

			bool operator!=(const bc_platform_device_pipeline_state& p_other) const noexcept;

			bool operator==(std::nullptr_t) const noexcept;

			bool operator!=(std::nullptr_t) const noexcept;

			platform_pack& get_platform_pack() noexcept override
			{
				return m_pack;
			}

			const platform_pack& get_platform_pack() const noexcept override
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;
		};

		using bc_device_pipeline_state = bc_platform_device_pipeline_state<g_current_render_api>;
		using bc_device_pipeline_state_ref = bc_device_ref<bc_device_pipeline_state>;
	}
}