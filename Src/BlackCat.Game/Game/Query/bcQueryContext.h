// [05/08/2020 MRB]

#pragma once

#include "Core/Messaging/Query/bcQueryContext.h"
#include "Game/System/Render/Light/bcLightManager.h"

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

		class bc_light_instances_query_context : public core::bc_query_context
		{
		public:
			bc_light_instances_query_context(bc_light_manager::iterator_buffer p_iterator)
				: m_iterator(std::move(p_iterator))
			{
			}
			
			 bc_light_manager::iterator_buffer m_iterator;
		};
	}
}