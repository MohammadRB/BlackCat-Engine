// [04/29/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Render/Light/bcLight.h"
#include "Game/Query/bcMainCameraSceneLightSharedQuery.h"
#include "Game/Query/bcSceneLightQuery.h"

namespace black_cat
{
	namespace game
	{
		void bc_main_camera_scene_light_shared_query::execute(const bc_light_instances_query_context& p_context) noexcept
		{
			{
				// TODO const_cast
				auto& l_iterator = const_cast<bc_light_manager::iterator_buffer&>(p_context.m_iterator);
				core_platform::bc_lock_guard<bc_light_manager::iterator_buffer> l_lock(l_iterator);

				for (auto& l_light : p_context.m_iterator)
				{
					const bool l_is_direct = l_light.get_type() == bc_light_type::direct;
					if (l_is_direct)
					{
						m_lights.push_back(bc_light_instance(l_light));
						continue;
					}

					const bool l_is_spot = l_light.get_type() == bc_light_type::spot;
					if (l_is_spot)
					{
						if (!m_frustum.intersects(l_light.get_bound_box()))
						{
							continue;
						}

						m_lights.push_back(bc_light_instance(l_light));
					}

					const bool l_is_point = l_light.get_type() == bc_light_type::point;
					if (l_is_point)
					{
						if (!m_frustum.intersects(l_light.get_bound_box()))
						{
							continue;
						}

						m_lights.push_back(bc_light_instance(l_light));
					}
				}
			}
		}
	}
}