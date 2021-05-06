// [05/08/2020 MRB]

#pragma once

#include "Core/Messaging/Query/bcQueryContext.h"

namespace black_cat
{
	namespace game
	{
		class bc_scene;
		
		class bc_scene_query_context : public core::bc_query_context
		{
		public:
			const bc_scene* m_scene;
		};
	}
}