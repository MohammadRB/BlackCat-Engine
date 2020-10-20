// [02/26/2019 MRB]

#include "Game/GamePCH.h"

#include "Core/Math/bcCoordinate.h"
#include "Game/System/Render/Light/bcLightManager.h"

namespace black_cat
{
	namespace game
	{
		core::bc_vector3f project_point_to_screen_space(const bc_camera_instance& p_camera, const core::bc_matrix4f& p_view_proj, const core::bc_vector3f& p_point)
		{
			auto l_point = p_view_proj * core::bc_vector4f(p_point, 1);
			l_point.x /= l_point.w;
			l_point.y /= l_point.w;
			l_point.z /= l_point.w;
			l_point.z = (p_camera.get_near_clip() * p_camera.get_far_clip()) / 
				(p_camera.get_far_clip() - l_point.z * (p_camera.get_far_clip() - p_camera.get_near_clip())) / 
				(p_camera.get_far_clip() - p_camera.get_near_clip());

			return { l_point.x, l_point.y, l_point.z };
		}

		bc_light_instance get_light_bounds(const bc_light& p_light)
		{
			const auto l_light_box = p_light.get_bound_box();
			core::bc_array<core::bc_vector3f, 8> l_light_box_points;

			l_light_box.get_points(l_light_box_points);

			bcFLOAT l_min_x = std::numeric_limits<bcFLOAT>::max(), l_min_y = std::numeric_limits<bcFLOAT>::max(), l_min_z = std::numeric_limits<bcFLOAT>::max();
			bcFLOAT	l_max_x = std::numeric_limits<bcFLOAT>::lowest(), l_max_y = std::numeric_limits<bcFLOAT>::lowest(), l_max_z = std::numeric_limits<bcFLOAT>::lowest();

			for(auto& l_point : l_light_box_points)
			{
				l_min_x = std::min(l_min_x, l_point.x);
				l_min_y = std::min(l_min_y, l_point.y);
				l_min_z = std::min(l_min_z, l_point.z);
				l_max_x = std::max(l_max_x, l_point.x);
				l_max_y = std::max(l_max_y, l_point.y);
				l_max_z = std::max(l_max_z, l_point.z);
			}

			return bc_light_instance
			(
				p_light,
				core::bc_vector3f(l_min_x, l_min_y, l_min_z),
				core::bc_vector3f(l_max_x, l_max_y, l_max_z)
			);
		}

		bc_light_manager::bc_light_manager()
			: m_lights(core::bc_memory_pool_allocator< bc_light >(m_pool))
		{
			// TODO get size of list internal node
			m_pool.initialize(200, sizeof(bc_light) + sizeof(void*) * 2, core::bc_alloc_type::unknown);
		}

		bc_light_manager::bc_light_manager(bc_light_manager&& p_other) noexcept
			: m_pool(std::move(p_other.m_pool)),
			m_lights(std::move(p_other.m_lights))
		{
		}

		bc_light_manager::~bc_light_manager()
		{
		}

		bc_light_manager& bc_light_manager::operator=(bc_light_manager&& p_other) noexcept
		{
			m_pool = std::move(p_other.m_pool);
			m_lights = std::move(p_other.m_lights);

			return *this;
		}

		bc_light_ptr bc_light_manager::add_light(const bc_direct_light& p_light)
		{
			m_lights.push_back(bc_light(p_light));
			return bc_light_ptr(&m_lights.back(), _bc_light_ptr_deleter(this));
		}

		bc_light_ptr bc_light_manager::add_light(const bc_point_light& p_light)
		{
			m_lights.push_back(bc_light(p_light));
			return bc_light_ptr(&m_lights.back(), _bc_light_ptr_deleter(this));
		}

		bc_light_ptr bc_light_manager::add_light(const bc_spot_light& p_light)
		{
			m_lights.push_back(bc_light(p_light));
			return bc_light_ptr(&m_lights.back(), _bc_light_ptr_deleter(this));
		}

		core::bc_vector_frame< bc_light_instance > bc_light_manager::get_light_instances(const bc_camera_instance& p_camera)
		{
			core::bc_vector_frame<bc_light_instance> l_lights;
			l_lights.reserve(m_lights.size());

			for (bc_light& l_light : m_lights)
			{
				auto l_light_instance = get_light_bounds(l_light);
				l_lights.push_back(l_light_instance);
			}

			return l_lights;
		}

		void bc_light_manager::update(const core_platform::bc_clock::update_param& p_clock)
		{
		}

		void bc_light_manager::destroy_light(bc_light* p_light)
		{
			m_lights.remove_if([=](const bc_light& p_item)
			{
				return &p_item == p_light;
			});
		}

		bc_light_instance bc_light_manager::_get_light_bounds(const bc_camera_instance& p_camera, const bc_light& p_light, const bc_direct_light& p_direct_light)
		{
			return bc_light_instance
			(
				p_light, 
				core::bc_vector3f(0, 0, p_camera.get_near_clip()), 
				core::bc_vector3f(p_camera.get_screen_width(), p_camera.get_screen_height(), p_camera.get_far_clip())
			);
		}

		bc_light_instance bc_light_manager::_get_light_bounds(const bc_camera_instance& p_camera, const bc_light& p_light, const bc_point_light& p_point_light)
		{
			const auto l_center = p_point_light.get_position(p_light.get_transformation());
			const auto l_right_extend = l_center + (p_camera.get_right() * p_point_light.get_radius());
			const auto l_forward_extend = l_center + (p_camera.get_forward() * p_point_light.get_radius());

			const auto l_view_proj = p_camera.get_view() * p_camera.get_projection();
			const auto l_center_proj = project_point_to_screen_space(p_camera, l_view_proj, l_center);
			const auto l_right_extend_proj = project_point_to_screen_space(p_camera, l_view_proj, l_right_extend);
			const auto l_forward_extend_proj = project_point_to_screen_space(p_camera, l_view_proj, l_forward_extend);
			const auto l_ss_radius = std::abs(l_center_proj.x - l_right_extend_proj.x);
			const auto l_depth_radius = std::abs(l_center_proj.z - l_forward_extend_proj.z);

			return bc_light_instance
			(
				p_light,
				core::bc_vector3f(l_center_proj.x - l_ss_radius, l_center_proj.y - l_ss_radius, l_center_proj.z - l_depth_radius),
				core::bc_vector3f(l_center_proj.x + l_ss_radius, l_center_proj.y + l_ss_radius, l_center_proj.z + l_depth_radius)
			);
		}

		bc_light_instance bc_light_manager::_get_light_bounds(const bc_camera_instance& p_camera, const bc_light& p_light, const bc_spot_light& p_spot_light)
		{
			const auto l_position = p_spot_light.get_position(p_light.get_transformation());
			const auto l_direction = p_spot_light.get_direction(p_light.get_transformation());

			const auto l_right = core::bc_vector3f::cross(l_direction, core::bc_vector3f::up());
			core::bc_matrix3f l_up_rotation;
			l_up_rotation.rotation_euler_lh(l_right, core::bc_to_radian(p_spot_light.get_angle() / 2));
			const auto l_up_direction = l_up_rotation * l_direction;
			
			const auto l_direction_end = l_position + l_direction * p_spot_light.get_length();
			const auto l_up_direction_end = l_position + l_up_direction * core::bc_vector3f::dot(l_direction * p_spot_light.get_length(), l_up_direction);

			const auto l_angle_length = l_up_direction_end.y - l_direction_end.y;

			const auto l_right_direction_end = core::bc_vector3f(l_direction_end.x + l_angle_length, l_direction_end.y, l_direction_end.z);
			const auto l_down_direction_end = core::bc_vector3f(l_direction_end.x, l_direction_end.y - l_angle_length, l_direction_end.z);
			const auto l_left_direction_end = core::bc_vector3f(l_direction_end.x - l_angle_length, l_direction_end.y, l_direction_end.z);

			const auto l_view_proj = p_camera.get_view() * p_camera.get_projection();
			const auto l_position_proj = project_point_to_screen_space(p_camera, l_view_proj, l_position);
			const auto l_up_position_proj = project_point_to_screen_space(p_camera, l_view_proj, l_up_direction_end);
			const auto l_right_position_proj = project_point_to_screen_space(p_camera, l_view_proj, l_right_direction_end);
			const auto l_down_position_proj = project_point_to_screen_space(p_camera, l_view_proj, l_down_direction_end);
			const auto l_left_position_proj = project_point_to_screen_space(p_camera, l_view_proj, l_left_direction_end);

			auto l_min_x = std::min(l_position_proj.x, l_up_position_proj.x);
			l_min_x = std::min(l_min_x, l_right_position_proj.x);
			l_min_x = std::min(l_min_x, l_down_position_proj.x);
			l_min_x = std::min(l_min_x, l_left_position_proj.x);

			auto l_min_y = std::min(l_position_proj.y, l_up_position_proj.y);
			l_min_y = std::min(l_min_y, l_right_position_proj.y);
			l_min_y = std::min(l_min_y, l_down_position_proj.y);
			l_min_y = std::min(l_min_y, l_left_position_proj.y);

			auto l_min_z = std::min(l_position_proj.z, l_up_position_proj.z);
			l_min_z = std::min(l_min_z, l_right_position_proj.z);
			l_min_z = std::min(l_min_z, l_down_position_proj.z);
			l_min_z = std::min(l_min_z, l_left_position_proj.z);

			auto l_max_x = std::max(l_position_proj.x, l_up_position_proj.x);
			l_max_x = std::max(l_max_x, l_right_position_proj.x);
			l_max_x = std::max(l_max_x, l_down_position_proj.x);
			l_max_x = std::max(l_max_x, l_left_position_proj.x);

			auto l_max_y = std::max(l_position_proj.y, l_up_position_proj.y);
			l_max_y = std::max(l_max_y, l_right_position_proj.y);
			l_max_y = std::max(l_max_y, l_down_position_proj.y);
			l_max_y = std::max(l_max_y, l_left_position_proj.y);

			auto l_max_z = std::max(l_position_proj.z, l_up_position_proj.z);
			l_max_z = std::max(l_max_z, l_right_position_proj.z);
			l_max_z = std::max(l_max_z, l_down_position_proj.z);
			l_max_z = std::max(l_max_z, l_left_position_proj.z);

			return bc_light_instance
			(
				p_light,
				core::bc_vector3f(l_min_x, l_min_y, l_min_z),
				core::bc_vector3f(l_max_x, l_max_y, l_max_z)
			);
		}
	}
}