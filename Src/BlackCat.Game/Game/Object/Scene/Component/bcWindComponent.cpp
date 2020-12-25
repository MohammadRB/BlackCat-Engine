// [12/12/2020 MRB]

#include "Game/GamePCH.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcWindComponent.h"
#include "Game/Object/Scene/Component/Event/bcActorEventBoundBoxChanged.h"
#include "Game/Object/Scene/Component/Event/bcActorEventWorldTransform.h"

namespace black_cat
{
	namespace game
	{
		bc_wind::bc_wind(const bc_direct_wind& p_direct)
			: m_direct(p_direct),
			m_type(bc_wind_type::direct)
		{
			_calculate_bound_box();
		}

		bc_wind::bc_wind(const bc_point_wind& p_point)
			: m_point(p_point),
			m_type(bc_wind_type::point)
		{
			_calculate_bound_box();
		}

		bc_wind::bc_wind(const bc_wind& p_other) noexcept
		{
			operator=(p_other);
		}

		bc_wind::~bc_wind() = default;

		bc_wind& bc_wind::operator=(const bc_wind& p_other) noexcept
		{
			switch (p_other.m_type)
			{
			case bc_wind_type::direct:
				m_direct = p_other.m_direct;
				break;
			case bc_wind_type::point:
				m_point = p_other.m_point;
				break;
			default:
				bcAssert(false);
				break;
			}

			m_type = p_other.m_type;
			m_bound_box = p_other.m_bound_box;

			return *this;
		}

		void bc_wind::set_transformation(const core::bc_matrix4f& p_transformation) noexcept
		{
			if(m_type == bc_wind_type::point)
			{
				m_point.m_position = p_transformation.get_translation();
			}

			_calculate_bound_box();
		}

		const physics::bc_bound_box& bc_wind::get_bound_box() const noexcept
		{
			return m_bound_box;
		}

		bc_direct_wind* bc_wind::as_direct_wind() noexcept
		{
			if(m_type == bc_wind_type::direct)
			{
				return &m_direct;
			}

			return nullptr;
		}

		const bc_direct_wind* bc_wind::as_direct_wind() const noexcept
		{
			return const_cast<bc_wind&>(*this).as_direct_wind();
		}

		bc_point_wind* bc_wind::as_point_wind() noexcept
		{
			if (m_type == bc_wind_type::point)
			{
				return &m_point;
			}

			return nullptr;
		}

		const bc_point_wind* bc_wind::as_point_wind() const noexcept
		{
			return const_cast<bc_wind&>(*this).as_point_wind();
		}

		void bc_wind::_calculate_bound_box() noexcept
		{
			switch (m_type)
			{
			case bc_wind_type::direct:
				m_bound_box = physics::bc_bound_box(core::bc_vector3f(0), core::bc_vector3f(.5, .5, .5));
				break;
			case bc_wind_type::point:
				m_bound_box = physics::bc_bound_box
				(
					m_point.m_position,
					core::bc_vector3f(m_point.m_radios)
				);
				break;
			default:
				bcAssert(false);
				break;
			}
		}

		bc_wind_component::bc_wind_component(bc_actor_component_index p_index)
			: bc_iactor_component(p_index)
		{
		}

		bc_actor bc_wind_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		const bc_wind* bc_wind_component::get_wind() const noexcept
		{
			return m_wind.is_set() ? &m_wind.get() : nullptr;
		}

		void bc_wind_component::initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters)
		{
			const auto& l_wind_type = p_parameters.get_value_throw<core::bc_string>("type");

			if (l_wind_type == "direct")
			{
				const auto l_direction_x = p_parameters.get_value_throw< bcFLOAT >("direction_x");
				const auto l_direction_y = p_parameters.get_value_throw< bcFLOAT >("direction_y");
				const auto l_direction_z = p_parameters.get_value_throw< bcFLOAT >("direction_z");
				const auto l_power = p_parameters.get_value_throw< bcFLOAT >("power");

				const bc_direct_wind l_direct_wind({ l_direction_x, l_direction_y, l_direction_z }, l_power);
				m_wind = bc_wind(l_direct_wind);
			}
			else if (l_wind_type == "point")
			{
				const auto l_position_x = p_parameters.get_value_throw< bcFLOAT >("position_x");
				const auto l_position_y = p_parameters.get_value_throw< bcFLOAT >("position_y");
				const auto l_position_z = p_parameters.get_value_throw< bcFLOAT >("position_z");
				const auto l_power = p_parameters.get_value_throw< bcFLOAT >("power");
				const auto l_radius = p_parameters.get_value_throw< bcFLOAT >("radius");

				const bc_point_wind l_wind_point({ l_position_x, l_position_y, l_position_z }, l_power, l_radius);
				m_wind = bc_wind(l_wind_point);
			}
			else
			{
				bcAssert(false);
			}
		}

		void bc_wind_component::handle_event(bc_actor& p_actor, const bc_actor_event& p_event)
		{
			const auto* l_world_transform_event = core::bc_imessage::as<bc_actor_event_world_transform>(p_event);
			if (l_world_transform_event)
			{
				// TODO what if wind is part of a mesh
				m_wind->set_transformation(l_world_transform_event->get_transform());

				const auto& l_bound_box = m_wind->get_bound_box();
				p_actor.add_event(bc_actor_event_bound_box_changed(l_bound_box));
			}
		}

		void bc_wind_component::update(bc_actor& p_actor,const core_platform::bc_clock::update_param& p_clock_update_param)
		{
		}
	}	
}