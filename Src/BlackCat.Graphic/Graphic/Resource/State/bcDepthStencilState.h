// [08/01/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcRenderApi.h"
#include "Graphic/bcGraphicDefinition.h"

namespace black_cat
{
	namespace graphic
	{
		struct bc_depth_stencilop_config
		{
			bc_stencil_op m_stencil_fail_op;
			bc_stencil_op m_stencil_depth_fail_op;
			bc_stencil_op m_stencil_pass_op;
			bc_comparison_func m_stencil_func;
		};

		struct bc_depth_stencil_state_config
		{
			bool m_depth_enable;
			bc_depth_write_mask m_depth_write_mask;
			bc_comparison_func m_depth_func;
			bool m_stencil_enable;
			bcUINT8 m_stencil_read_mask;
			bcUINT8 m_stencil_write_mask;
			bc_depth_stencilop_config m_front_face;
			bc_depth_stencilop_config m_back_face;

			static bc_depth_stencil_state_config& default_config()
			{
				static bc_depth_stencil_state_config s_config;

				s_config.m_depth_enable = true;
				s_config.m_depth_write_mask = bc_depth_write_mask::all;
				s_config.m_depth_func = bc_comparison_func::less;
				s_config.m_stencil_enable = false;
				s_config.m_stencil_read_mask = 0xff;
				s_config.m_stencil_write_mask = 0xff;

				s_config.m_front_face.m_stencil_func = bc_comparison_func::always;
				s_config.m_front_face.m_stencil_pass_op = bc_stencil_op::keep;
				s_config.m_front_face.m_stencil_fail_op = bc_stencil_op::keep;
				s_config.m_front_face.m_stencil_depth_fail_op = bc_stencil_op::keep;

				s_config.m_back_face.m_stencil_func = bc_comparison_func::never;
				s_config.m_back_face.m_stencil_pass_op = bc_stencil_op::keep;
				s_config.m_back_face.m_stencil_fail_op = bc_stencil_op::keep;
				s_config.m_back_face.m_stencil_depth_fail_op = bc_stencil_op::keep;

				return s_config;
			}
		};

		/*template<bc_platform_render_api TRenderApi>
		struct bc_platform_depth_stencil_state_pack
		{

		};

		template<bc_platform_render_api TRenderApi>
		class bc_platform_depth_stencil_state : public platform::bc_no_copy
		{
		public:
			using platform_pack = bc_platform_depth_stencil_state_pack<TRenderApi>;

		public:
			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			platform_pack m_pack;
		};

		class bc_depth_stencil_state : public bc_platform_depth_stencil_state<g_current_render_api>
		{};*/
	}
}