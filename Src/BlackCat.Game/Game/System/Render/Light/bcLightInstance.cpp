// [11/03/2020 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Render/Light/bcLightInstance.h"

namespace black_cat
{
	namespace game
	{
		bc_light_instance::bc_light_instance(const bc_light& p_light) noexcept
			: m_id(p_light.get_id()),
			m_type(p_light.get_type()),
			m_light(bc_direct_light({0,0,0}, {0,0,0}, 0, {0,0,0}, 0)),
			m_bound_box(p_light.get_bound_box())
		{
			switch (m_type)
			{
			case bc_light_type::direct:
			{
				m_light = bc_direct_light(*p_light.as_direct_light());
				break;
			}
			case bc_light_type::point:
			{
				m_light = bc_point_light(*p_light.as_point_light());
				auto& l_point_light = *as_point_light();
				l_point_light.set_position(l_point_light.get_position(p_light.get_transformation()));
				break;
			}
			case bc_light_type::spot:
			{
				m_light = bc_spot_light(*p_light.as_spot_light());
				auto& l_spot_light = *as_spot_light();
				l_spot_light.set_position(l_spot_light.get_position(p_light.get_transformation()));
				l_spot_light.set_direction(l_spot_light.get_direction(p_light.get_transformation()));
				break;
			}
			}
		}
		
		core::bc_vector3f bc_light_instance::get_min_bound() const noexcept
		{
			const auto l_center = m_bound_box.get_center();
			const auto l_half_extend = m_bound_box.get_half_extends();
			return { l_center.x - l_half_extend.x, l_center.y - l_half_extend.y, l_center.z - l_half_extend.z };
		}

		core::bc_vector3f bc_light_instance::get_max_bound() const noexcept
		{
			const auto l_center = m_bound_box.get_center();
			const auto l_half_extend = m_bound_box.get_half_extends();
			return { l_center.x + l_half_extend.x, l_center.y + l_half_extend.y, l_center.z + l_half_extend.z };
		}

		const bc_light_flare* bc_light_instance::get_flare() const noexcept
		{
			return as_point_light() ? as_point_light()->get_flare() : as_spot_light() ? as_spot_light()->get_flare() : nullptr;
		}

		bc_direct_light* bc_light_instance::as_direct_light() noexcept
		{
			if (get_type() != bc_light_type::direct)
			{
				return nullptr;
			}

			return &std::get<bc_direct_light>(m_light);
		}

		const bc_direct_light* bc_light_instance::as_direct_light() const noexcept
		{
			return const_cast<bc_light_instance&>(*this).as_direct_light();
		}

		bc_point_light* bc_light_instance::as_point_light() noexcept
		{
			if (get_type() != bc_light_type::point)
			{
				return nullptr;
			}

			return &std::get<bc_point_light>(m_light);
		}

		const bc_point_light* bc_light_instance::as_point_light() const noexcept
		{
			return const_cast<bc_light_instance&>(*this).as_point_light();
		}

		bc_spot_light* bc_light_instance::as_spot_light() noexcept
		{
			if (get_type() != bc_light_type::spot)
			{
				return nullptr;
			}

			return &std::get<bc_spot_light>(m_light);
		}

		const bc_spot_light* bc_light_instance::as_spot_light() const noexcept
		{
			return const_cast<bc_light_instance&>(*this).as_spot_light();
		}
	}
}