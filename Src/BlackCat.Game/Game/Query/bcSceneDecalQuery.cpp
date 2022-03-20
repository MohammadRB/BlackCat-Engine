// [04/12/2021 MRB]

#include "Game/GamePCH.h"

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/System/Render/Decal/bcDecalManager.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Query/bcSceneDecalQuery.h"

namespace black_cat
{
	namespace game
	{
		void bc_scene_decal_query::execute(const bc_scene_query_context& p_context) noexcept
		{
			auto l_iterator = p_context.m_scene->get_decal_manager().get_iterator_buffer();
			
			{
				platform::bc_lock_guard l_lock(l_iterator);

				for (auto& l_decal_instance : l_iterator)
				{
					const auto* l_decal = l_decal_instance.get_decal();
					const auto l_camera_distance = (l_decal_instance.get_world_transform().get_translation() - m_camera_position).magnitude();

					if (l_camera_distance <= l_decal->get_view_distance())
					{
						m_buffer.add_decal_instance
						(
							l_decal_instance.get_decal_ptr(),
							bc_render_instance(l_decal_instance.get_world_transform(), l_decal_instance.get_render_group())
						);
					}
				}
			}
		}
	}
}