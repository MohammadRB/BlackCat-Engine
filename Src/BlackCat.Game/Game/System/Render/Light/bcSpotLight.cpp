// [04/03/2022 MRB]

#include "Game/GamePCH.h"

#include "Core/Math/bcCoordinate.h"
#include "Game/System/Render/Light/bcSpotLight.h"

namespace black_cat
{
	namespace game
	{
		bcFLOAT bc_spot_light::calculate_intensity(const core::bc_vector3f& p_view_position, bcFLOAT p_fade_point) const noexcept
		{
			const auto l_camera_direction = core::bc_vector3f::normalize(p_view_position - m_position);
			const auto l_camera_angle = core::bc_to_degree(std::acosf(m_direction.dot(l_camera_direction)));
			auto l_camera_angle_ratio = 1 - std::min(1.0f, l_camera_angle / (m_angle / 2));
			l_camera_angle_ratio = std::min(1.f, l_camera_angle_ratio + p_fade_point);
			l_camera_angle_ratio = (l_camera_angle_ratio - p_fade_point) * (1.f / (1 - p_fade_point));

			return l_camera_angle_ratio;
		}
	}
}
