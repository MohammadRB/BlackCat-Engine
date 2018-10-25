// [01/08/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcRenderApi.h"
#include "Graphic/bcGraphicDefinition.h"
#include "Graphic/bcRenderApiInfo.h"

namespace black_cat
{
	namespace graphic
	{
		struct bc_render_target_blend_config
		{
			bool m_blend_enable;
			bc_blend m_src_blend;
			bc_blend m_dest_blend;
			bc_blend_op m_blend_op;
			bc_blend m_src_blend_alpha;
			bc_blend m_dest_blend_alpha;
			bc_blend_op m_blend_op_alpha;
			bcUINT8 m_render_target_write_mask;
		};

		struct bc_blend_state_config
		{
			bool m_alpha_to_coverage_enable;
			bool m_independent_blend_enable;
			bc_render_target_blend_config m_render_target[bc_render_api_info::number_of_om_render_target_slots()];

			static bc_blend_state_config& default_config()
			{
				static bc_blend_state_config s_config;

				s_config.m_alpha_to_coverage_enable = false;
				s_config.m_independent_blend_enable = false;
				for (bcINT i = 0; i < bc_render_api_info::number_of_om_render_target_slots(); i++)
				{
					s_config.m_render_target[i].m_blend_enable = false;
					s_config.m_render_target[i].m_src_blend = bc_blend::one;
					s_config.m_render_target[i].m_dest_blend = bc_blend::zero;
					s_config.m_render_target[i].m_blend_op = bc_blend_op::add;
					s_config.m_render_target[i].m_src_blend_alpha = bc_blend::one;
					s_config.m_render_target[i].m_dest_blend_alpha = bc_blend::zero;
					s_config.m_render_target[i].m_blend_op_alpha = bc_blend_op::add;
					s_config.m_render_target[i].m_render_target_write_mask = static_cast<bcUINT8>(bc_color_write::all);
				}

				return s_config;
			}
		};

		/*template< bc_platform_render_api TRenderApi >
		struct bc_platform_blend_state_pack
		{
			
		};

		template< bc_platform_render_api TRenderApi >
		class bc_platform_blend_state : public core_platform::bc_no_copy
		{
		public:
			using platform_pack = bc_platform_blend_state_pack<TRenderApi>;

		public:
			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			platform_pack m_pack;
		};

		class bc_blend_state : public bc_platform_blend_state<g_current_render_api>
		{};*/
	}
}