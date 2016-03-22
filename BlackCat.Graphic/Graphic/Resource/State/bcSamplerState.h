// [01/08/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcPlatformRenderApi.h"
#include "Graphic/bcGraphicDefinition.h"
#include "Graphic/bcDeviceObject.h"
#include "Graphic/bcResourcePtr.h"

namespace black_cat
{
	namespace graphic
	{
		struct bc_sampler_state_config
		{
			bc_filter m_filter;
			bc_texture_address_mode m_address_u;
			bc_texture_address_mode m_address_v;
			bc_texture_address_mode m_address_w;
			bcFLOAT32 m_mip_lod_bias;
			bcUINT m_max_anisotropy;
			bc_comparison_func m_comparison_func;
			bcFLOAT32 m_border_color[4];
			bcFLOAT32 m_min_lod;
			bcFLOAT32 m_max_lod;

			static bc_sampler_state_config& default_config()
			{
				static bc_sampler_state_config s_config;

				s_config.m_filter = bc_filter::comparison_min_mag_mip_linear;
				s_config.m_address_u = bc_texture_address_mode::wrap;
				s_config.m_address_v = bc_texture_address_mode::wrap;
				s_config.m_address_w = bc_texture_address_mode::wrap;
				s_config.m_mip_lod_bias = 0.0f;
				s_config.m_max_anisotropy = 1;
				s_config.m_comparison_func = bc_comparison_func::always;
				s_config.m_border_color[0] = 0.0f; 
				s_config.m_border_color[1] = 0.0f; 
				s_config.m_border_color[2] = 0.0f; 
				s_config.m_border_color[3] = 0.0f;
				s_config.m_min_lod = 0;
				s_config.m_max_lod = (std::numeric_limits<bcFLOAT32>::max)();

				return s_config;
			}
		};

		template< bc_platform_render_api TRenderApi >
		struct bc_platform_sampler_state_pack
		{

		};

		template< bc_platform_render_api TRenderApi >
		class bc_platform_sampler_state : public bc_device_object
		{
		public:
			using platform_pack = bc_platform_sampler_state_pack<TRenderApi>;

		public:
			bc_platform_sampler_state();

			bc_platform_sampler_state(bc_platform_sampler_state&& p_other);

			~bc_platform_sampler_state();

			bc_platform_sampler_state& operator=(bc_platform_sampler_state&& p_other);

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			platform_pack m_pack;
		};

		using bc_sampler_state = bc_platform_sampler_state<g_current_platform_render_api>;

		using bc_sampler_state_ptr = bc_resource_ptr< bc_sampler_state >;
	}
}