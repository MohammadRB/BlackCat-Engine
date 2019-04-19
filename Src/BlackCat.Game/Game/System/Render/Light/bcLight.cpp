// [02/23/2019 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Render/Light/bcLight.h"
#include "Game/System/Render/Light/bcLightManager.h"

namespace black_cat
{
	namespace game
	{
		_bc_light_ptr_deleter::_bc_light_ptr_deleter()
			: m_manager(nullptr)
		{
		}

		_bc_light_ptr_deleter::_bc_light_ptr_deleter(bc_light_manager* p_manager)
			: m_manager(p_manager)
		{
		}

		void _bc_light_ptr_deleter::operator()(bc_light* p_light)
		{
			bcAssert(m_manager);

			m_manager->destroy_light(p_light);
		}

		bc_light::bc_light(const bc_direct_light& p_light)
			: m_direct_light(p_light),
			m_type(bc_light_type::direct),
			m_transformation(core::bc_matrix4f::identity())
		{
		}

		bc_light::bc_light(const bc_point_light& p_light)
			: m_point_light(p_light),
			m_type(bc_light_type::point),
			m_transformation(core::bc_matrix4f::identity())
		{
		}

		bc_light::bc_light(const bc_spot_light& p_light)
			: m_spot_light(p_light),
			m_type(bc_light_type::spot),
			m_transformation(core::bc_matrix4f::identity())
		{
		}

		bc_light::bc_light(bc_light&& p_other) noexcept
		{
			operator=(std::move(p_other));
		}

		bc_light& bc_light::operator=(bc_light&& p_other) noexcept
		{
			m_type = p_other.m_type;

			switch (m_type)
			{
			case bc_light_type::direct:
				m_direct_light = p_other.m_direct_light;
				break;
			case bc_light_type::point:
				m_point_light = p_other.m_point_light;
				break;
			case bc_light_type::spot:
				m_spot_light = p_other.m_spot_light;
				break;
			}

			m_transformation = p_other.m_transformation;

			return *this;
		}

		physics::bc_bound_box bc_light::get_bound_box() const
		{
			const auto l_type = get_type();
			physics::bc_bound_box l_box;

			switch (l_type)
			{
			case bc_light_type::direct:
				{
					l_box = physics::bc_bound_box(m_transformation.get_translation(), core::bc_vector3f(.5, .5, .5));
					break;
				}
			case bc_light_type::point:
				{
					const auto l_point_light = as_point_light();

					l_box = physics::bc_bound_box
					(
						m_transformation.get_translation() + l_point_light->get_position(),
						core::bc_vector3f(l_point_light->get_radius())
					);
					break;
				}
			case bc_light_type::spot:
				{
					const auto l_spot_light = as_spot_light();
					const auto l_position = m_transformation.get_translation() + l_spot_light->get_position();
					const auto l_center = l_position + (l_spot_light->get_direction() * (l_spot_light->get_length() / 2));

					l_box = physics::bc_bound_box
					(
						l_center,
						core::bc_vector3f(l_spot_light->get_length() / 2)
					);
					break;
				}
			}

			return l_box;
		}

		bc_direct_light* bc_light::as_direct_light()
		{
			if (get_type() != bc_light_type::direct)
			{
				return nullptr;
			}

			return &m_direct_light;
		}

		const bc_direct_light* bc_light::as_direct_light() const
		{
			return const_cast<bc_light&>(*this).as_direct_light();
		}

		bc_point_light* bc_light::as_point_light()
		{
			if (get_type() != bc_light_type::point)
			{
				return nullptr;
			}

			return &m_point_light;
		}

		const bc_point_light* bc_light::as_point_light() const
		{
			return const_cast<bc_light&>(*this).as_point_light();
		}

		bc_spot_light* bc_light::as_spot_light()
		{
			if (get_type() != bc_light_type::spot)
			{
				return nullptr;
			}

			return &m_spot_light;
		}

		const bc_spot_light* bc_light::as_spot_light() const
		{
			return const_cast<bc_light&>(*this).as_spot_light();
		}
	}
}