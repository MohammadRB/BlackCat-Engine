// [02/26/2019 MRB]

#include "Game/GamePCH.h"

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/Math/bcCoordinate.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "Game/System/Render/Light/bcLightManager.h"
#include "Game/Query/bcQueryContext.h"

namespace black_cat
{
	namespace game
	{
		/*core::bc_vector3f project_point_to_screen_space(const bc_camera_instance& p_camera, const core::bc_matrix4f& p_view_proj, const core::bc_vector3f& p_point)
		{
			auto l_point = p_view_proj * core::bc_vector4f(p_point, 1);
			l_point.x /= l_point.w;
			l_point.y /= l_point.w;
			l_point.z /= l_point.w;
			l_point.z = (p_camera.get_near_clip() * p_camera.get_far_clip()) / 
				(p_camera.get_far_clip() - l_point.z * (p_camera.get_far_clip() - p_camera.get_near_clip())) / 
				(p_camera.get_far_clip() - p_camera.get_near_clip());

			return { l_point.x, l_point.y, l_point.z };
		}*/

		bc_light_manager::bc_light_manager()
			: m_lights(200)
		{
		}

		bc_light_manager::bc_light_manager(bc_light_manager&& p_other) noexcept
			: m_lights(std::move(p_other.m_lights))
		{
		}

		bc_light_manager::~bc_light_manager() = default;

		bc_light_manager& bc_light_manager::operator=(bc_light_manager&& p_other) noexcept
		{
			m_lights = std::move(p_other.m_lights);
			
			return *this;
		}

		bc_light_ptr bc_light_manager::add_light(const bc_direct_light& p_light)
		{
			{
				core_platform::bc_shared_mutex_guard l_lock(m_lights_lock);

				m_lights.push_back(bc_light(p_light));
				return bc_light_ptr(&m_lights.back(), _bc_light_ptr_deleter(this));
			}
		}

		bc_light_ptr bc_light_manager::add_light(const bc_point_light& p_light)
		{
			{
				core_platform::bc_shared_mutex_guard l_lock(m_lights_lock);
				
				m_lights.push_back(bc_light(p_light));
				return bc_light_ptr(&m_lights.back(), _bc_light_ptr_deleter(this));
			}
		}

		bc_light_ptr bc_light_manager::add_light(const bc_spot_light& p_light)
		{
			{
				core_platform::bc_shared_mutex_guard l_lock(m_lights_lock);
				
				m_lights.push_back(bc_light(p_light));
				return bc_light_ptr(&m_lights.back(), _bc_light_ptr_deleter(this));
			}
		}
		
		void bc_light_manager::update(const core_platform::bc_clock::update_param& p_clock)
		{
		}

		bc_light_manager::iterator_buffer bc_light_manager::get_iterator_buffer() const noexcept
		{
			return iterator_buffer(m_lights_lock, m_lights);
		}

		void bc_light_manager::destroy_light(bc_light* p_light)
		{
			{
				core_platform::bc_shared_mutex_guard l_lock(m_lights_lock);

				m_lights.remove_if
				(
					[=](const bc_light& p_item)
					{
						return &p_item == p_light;
					}
				);
			}
		}
	}
}