// [06/08/2016 MRB]

#pragma once

#include "Graphic/Resource/State/bcBlendState.h"
#include "Graphic/Resource/State/bcDepthStencilState.h"
#include "Graphic/Resource/State/bcInputLayout.h"
#include "Graphic/Resource/State/bcRasterizerState.h"
#include "Graphic/Resource/State/bcViewPort.h"
#include "GraphicImp/Resource/State/bcSamplerState.h"
#include "Game/bcExport.h"
#include "Game/System/Render/State/bcVertexLayout.h"

namespace black_cat
{
	namespace game
	{
		using bc_surface_format_type = graphic::bc_format;

		enum class bc_blend_type
		{
			opaque,
			alpha
		};

		enum class bc_depth_stencil_type
		{
			depth_off_stencil_off,
			depth_less_stencil_off
		};

		enum class bc_rasterizer_type
		{
			fill_solid_cull_back,
			fill_solid_cull_front,
			fill_solid_cull_none,
			fill_wireframe_cull_back,
			fill_wireframe_cull_front,
			fill_wireframe_cull_none
		};

		enum class bc_multi_sample_type
		{
			c1_q1,
		};

		class BC_GAME_DLL bc_graphic_state_configs
		{
		public:
			static graphic::bc_blend_state_config bc_blend_config(bc_blend_type p_value);

			static graphic::bc_depth_stencil_state_config bc_depth_stencil_config(bc_depth_stencil_type p_value);

			static graphic::bc_rasterizer_state_config bc_rasterizer_config(bc_rasterizer_type p_value);

			static graphic::bc_input_layout_config bc_input_layout_config(bc_vertex_type p_value);

			static graphic::bc_texture_ms_config bc_texture_multi_sample(bc_multi_sample_type p_ms);

		protected:

		private:
		};
	}
}