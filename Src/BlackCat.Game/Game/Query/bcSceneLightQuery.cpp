// [10/30/2020 MRB]

#include "Game/GamePCH.h"

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/Utility/bcEnumOperand.h"
#include "Game/System/Render/Light/bcLightManager.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Query/bcSceneLightQuery.h"

namespace black_cat
{
	namespace game
	{
		void bc_scene_light_query::execute(const bc_scene_query_context& p_context) noexcept
		{
			const bool l_need_direct = core::bc_enum::has(m_types, bc_light_type::direct);
			const bool l_need_spot = core::bc_enum::has(m_types, bc_light_type::spot);
			const bool l_need_point = core::bc_enum::has(m_types, bc_light_type::point);

			{
				auto l_iterator = p_context.m_scene->get_light_manager().get_iterator_buffer();
				core_platform::bc_lock_guard<bc_light_manager::iterator_buffer> l_lock(l_iterator);

				for (auto& l_light : l_iterator)
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
						if (m_frustum.has_value() && !m_frustum->intersects(l_light.get_bound_box()))
						{
							continue;
						}

						m_lights.push_back(bc_light_instance(l_light));
						continue;
					}

					const bool l_is_point = l_need_point && l_light.get_type() == bc_light_type::point;
					if (l_is_point)
					{
						if (m_frustum.has_value() && !m_frustum->intersects(l_light.get_bound_box()))
						{
							continue;
						}

						m_lights.push_back(bc_light_instance(l_light));
						continue;
					}
				}
			}
		}
	}	
}