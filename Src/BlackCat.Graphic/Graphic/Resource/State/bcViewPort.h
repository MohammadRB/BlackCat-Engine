// [01/08/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"

namespace black_cat
{
	namespace graphic
	{
		struct bc_viewport
		{
			bcINT32 m_top_left_x;
			bcINT32 m_top_left_y;
			bcUINT32 m_width;
			bcUINT32 m_height;
			bcFLOAT m_min_depth;
			bcFLOAT m_max_depth;

			static bc_viewport default_config(bcUINT32 p_width, bcUINT32 p_height)
			{
				return default_config(0, 0, p_width, p_height);
			}

			static bc_viewport default_config(bcINT32 p_top_left_x, bcINT32 p_top_left_y, bcUINT32 p_width, bcUINT32 p_height)
			{
				bc_viewport l_config;

				l_config.m_top_left_x = p_top_left_x;
				l_config.m_top_left_y = p_top_left_y;
				l_config.m_width = p_width;
				l_config.m_height = p_height;
				l_config.m_min_depth = 0;
				l_config.m_max_depth = 1;

				return l_config;
			}
		};
	}
}