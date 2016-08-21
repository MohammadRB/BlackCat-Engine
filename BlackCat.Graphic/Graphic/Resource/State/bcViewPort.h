// [01/08/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcPlatformRenderApi.h"

namespace black_cat
{
	namespace graphic
	{
		struct bc_viewport
		{
			bcFLOAT m_top_left_x;
			bcFLOAT m_top_left_y;
			bcFLOAT m_width;
			bcFLOAT m_height;
			bcFLOAT m_min_depth;
			bcFLOAT m_max_depth;

			static bc_viewport& default_config(bcFLOAT p_width, bcFLOAT p_height)
			{
				static bc_viewport s_config;

				s_config.m_top_left_x = 0;
				s_config.m_top_left_y = 0;
				s_config.m_width = p_width;
				s_config.m_height = p_height;
				s_config.m_min_depth = 0;
				s_config.m_max_depth = 1;

				return s_config;
			}
		};
	}
}
