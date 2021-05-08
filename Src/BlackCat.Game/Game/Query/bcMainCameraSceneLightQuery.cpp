// [10/30/2020 MRB]

#include "Game/GamePCH.h"

#include "Core/Messaging/Query/bcQueryManager.h"
#include "Core/Utility/bcEnumOperand.h"
#include "Game/System/Render/Light/bcLight.h"
#include "Game/Query/bcMainCameraSceneLightQuery.h"
#include "Game/Query/bcMainCameraSceneLightSharedQuery.h"

namespace black_cat
{
	namespace game
	{
		/*void bc_main_camera_scene_light_query::execute(const bc_scene_query_context& p_context) noexcept
		{
			const bool l_need_direct = core::bc_enum::has(m_types, bc_light_type::direct);
			const bool l_need_spot = core::bc_enum::has(m_types, bc_light_type::spot);
			const bool l_need_point = core::bc_enum::has(m_types, bc_light_type::point);
			const auto l_light_instances = p_context.get_shared_query<bc_main_camera_scene_light_shared_query>().get_lights();
			
			for (const auto& l_light : l_light_instances)
			{
				const bool l_is_direct = l_need_direct && l_light.get_type() == bc_light_type::direct;
				if (l_is_direct)
				{
					m_lights.push_back(bc_light_instance(l_light));
					continue;
				}

				const bool l_is_spot = l_need_spot && l_light.get_type() == bc_light_type::spot;
				if (l_is_spot)
				{
					m_lights.push_back(bc_light_instance(l_light));
					continue;
				}

				const bool l_is_point = l_need_point && l_light.get_type() == bc_light_type::point;
				if (l_is_point)
				{
					m_lights.push_back(bc_light_instance(l_light));
					continue;
				}
			}
		}*/
	}
}