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
			{
				auto l_iterator = p_context.m_scene->get_decal_manager().get_iterator_buffer();
				core_platform::bc_lock_guard<bc_decal_manager::iterator_buffer> l_lock(l_iterator);

				for (auto& l_decal_instance : l_iterator)
				{
					auto* l_decal = l_decal_instance.get_decal();
					const auto l_lod_factor = std::max(l_decal->get_width(), l_decal->get_height()) * l_decal->get_lod_scale() * get_global_config().get_lod_global_scale();
					const auto l_camera_distance = (l_decal_instance.get_world_transform().get_translation() - m_camera_position).magnitude();
					const auto l_culling_index = static_cast<bcUINT32>(l_camera_distance / l_lod_factor);

					if (l_culling_index <= get_global_config().get_lod_culling_index())
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