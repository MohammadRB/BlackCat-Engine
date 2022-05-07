// [12/12/2020 MRB]

#include "Game/GamePCH.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManagerContainer.h"
#include "Game/Object/Scene/Component/bcWindComponent.h"
#include "Game/Object/Scene/Component/bcIconComponent.h"
#include "Game/Object/Scene/Component/Event/bcBoundBoxChangedActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/bcJsonParse.h"
#include "Game/bcConstant.h"

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
				BC_ASSERT(false);
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
				BC_ASSERT(false);
				break;
			}
		}

		bc_wind_component::bc_wind_component(bc_actor_id p_actor_id, bc_actor_component_id p_id)
			: bci_actor_component(p_actor_id, p_id)
		{
		}

		bc_actor bc_wind_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		const bc_wind* bc_wind_component::get_wind() const noexcept
		{
			return m_wind.has_value() ? m_wind.get() : nullptr;
		}

		void bc_wind_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
			const core::bc_string* l_wind_type = nullptr;
			json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_wind_type, l_wind_type);

			if (*l_wind_type == "direct")
			{
				core::bc_vector3f l_direction;
				bcFLOAT l_power;

				json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_wind_direction, l_direction);
				json_parse::bc_load_throw(p_context.m_parameters, "power", l_power);

				const bc_direct_wind l_direct_wind(l_direction, l_power);
				m_wind = bc_wind(l_direct_wind);
			}
			else if (*l_wind_type == "point")
			{
				core::bc_vector3f l_position;
				bcFLOAT l_power;
				bcFLOAT l_radius;

				json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_wind_position, l_position);
				json_parse::bc_load_throw(p_context.m_parameters, "power", l_power);
				json_parse::bc_load_throw(p_context.m_parameters, "radius", l_radius);

				const bc_point_wind l_wind_point(l_position, l_power, l_radius);
				m_wind = bc_wind(l_wind_point);
			}
			else
			{
				BC_ASSERT(false);
				throw bc_invalid_argument_exception("Invalid wind type");
			}

			auto* l_icon_component = p_context.m_actor.get_create_component<bc_icon_component>();
			l_icon_component->set_icon(bc_icon_type::wind);
		}

		void bc_wind_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			if (const auto* l_world_transform_event = core::bci_message::as<bc_world_transform_actor_event>(p_context.m_event))
			{
				// TODO what if wind is part of a mesh
				const auto& l_transform = l_world_transform_event->get_transform();
				m_wind->set_transformation(l_transform);

				//const auto& l_bound_box = physics::bc_bound_box(l_transform.get_translation(), core::bc_vector3f(.5f));
				//p_context.m_actor.add_event(bc_bound_box_changed_actor_event(l_bound_box));

				return;
			}
		}
	}	
}