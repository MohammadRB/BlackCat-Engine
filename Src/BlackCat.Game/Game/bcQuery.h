// [05/08/2020 MRB]

#pragma once

#include "Core/Messaging/Query/bcQuery.h"
#include "Game/Object/Scene/bcScene.h"

namespace black_cat
{
	namespace game
	{
		class bc_scene_camera_frustum_query_context
		{
		public:
			const bc_scene& m_scene;
		};
	}
}