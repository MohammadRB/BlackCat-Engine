// [10/30/2020 MRB]

#include "Game/GamePCH.h"

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/Utility/bcEnumOperand.h"
#include "Game/Query/bcSceneLightQuery.h"

namespace black_cat
{
	namespace game
	{
		inline void bc_scene_light_query::execute(const bc_light_instances_query_context& p_context) noexcept
		{
			const bool l_need_direct = core::bc_enum::has(m_types, bc_light_type::direct),
				l_need_spot = core::bc_enum::has(m_types, bc_light_type::spot),
				l_need_point = core::bc_enum::has(m_types, bc_light_type::point);

			{
				// TODO const_cast
				auto& l_iterator = const_cast<bc_light_manager::iterator_buffer&>(p_context.m_iterator);
				core_platform::bc_lock_guard<bc_light_manager::iterator_buffer> l_lock(l_iterator);

				for (auto& l_light : p_context.m_iterator)
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
						if (m_frustum.is_set() && !m_frustum->intersects(l_light.get_bound_box()))
						{
							continue;
						}

						m_lights.push_back(bc_light_instance(l_light));
					}

					const bool l_is_point = l_need_point && l_light.get_type() == bc_light_type::point;
					if (l_is_point)
					{
						if (m_frustum.is_set() && !m_frustum->intersects(l_light.get_bound_box()))
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