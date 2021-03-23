// [01/08/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcRenderApi.h"

namespace black_cat
{
	namespace graphic
	{
		struct bc_rasterizer_state_config
		{
			bc_fill_mode m_fill_mode;
			bc_cull_mode m_cull_mode;
			bool m_front_counter_clockwise;
			bcINT m_depth_bias;
			bcFLOAT m_depth_bias_clamp;
			bcFLOAT m_slope_scaled_depth_bias;
			bool m_depth_clip_enable;
			bool m_scissor_enable;
			bool m_multisample_enable;
			bool m_antialiased_line_enable;

			static bc_rasterizer_state_config& default_config()
			{
				static bc_rasterizer_state_config s_config;

				s_config.m_fill_mode = bc_fill_mode::solid;
				s_config.m_cull_mode = bc_cull_mode::back;
				s_config.m_front_counter_clockwise = false;
				s_config.m_depth_bias = 0;
				s_config.m_slope_scaled_depth_bias = 0.0f;
				s_config.m_depth_bias_clamp = 0.0f;
				s_config.m_depth_clip_enable = true;
				s_config.m_scissor_enable = false;
				s_config.m_multisample_enable = false;
				s_config.m_antialiased_line_enable = false;

				return s_config;
			}
		};

		/*template< bc_platform_render_api TRenderApi >
		struct bc_platform_rasterizer_state_pack
		{

		};

		template< bc_platform_render_api TRenderApi >
		class bc_platform_rasterizer_state : public core_platform::bc_no_copy
		{
		public:
			using platform_pack = bc_platform_rasterizer_state_pack<TRenderApi>;

		public:
			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			platform_pack m_pack;
		};

		class bc_rasterizer_state : public bc_platform_rasterizer_state<g_current_render_api>
		{};*/
	}
}