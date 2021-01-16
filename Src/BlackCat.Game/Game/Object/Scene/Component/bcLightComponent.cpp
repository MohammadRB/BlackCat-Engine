// [03/01/2019 MRB]

#include "Game/GamePCH.h"

#include "Core/bcUtility.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Render/Light/bcLightManager.h"
#include "Game/Object/Scene/Component/bcLightComponent.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/Event/bcActorEventWorldTransform.h"
#include "Game/Object/Scene/Component/Event/bcActorEventBoundBoxChanged.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"

namespace black_cat
{
	namespace game
	{
		bc_light_component::bc_light_component(bc_actor_component_index p_index)
			: bci_actor_component(p_index)
		{
		}

		bc_actor bc_light_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		core::bc_vector3f bc_light_component::get_world_position() const noexcept
		{
			return m_light->get_transformation().get_translation();
		}

		const core::bc_matrix4f& bc_light_component::get_world_transform() const noexcept
		{
			return m_light->get_transformation();
		}

		bc_light* bc_light_component::get_light() noexcept
		{
			return m_light.get();
		}

		void bc_light_component::initialize(bc_actor_component_initialize_context& p_context)
		{
			auto& l_game_system = *core::bc_get_service<bc_game_system>();
			auto& l_light_manager = l_game_system.get_render_system().get_light_manager();
			auto& l_light_type = p_context.m_parameters.get_value_throw<core::bc_string>("type");

			if(l_light_type == "direct")
			{
				const auto l_direction_x = p_context.m_parameters.get_value_throw< bcFLOAT >("direction_x");
				const auto l_direction_y = p_context.m_parameters.get_value_throw< bcFLOAT >("direction_y");
				const auto l_direction_z = p_context.m_parameters.get_value_throw< bcFLOAT >("direction_z");
				const auto l_color_r = p_context.m_parameters.get_value_throw< bcFLOAT >("color_r");
				const auto l_color_g = p_context.m_parameters.get_value_throw< bcFLOAT >("color_g");
				const auto l_color_b = p_context.m_parameters.get_value_throw< bcFLOAT >("color_b");
				const auto l_intensity = p_context.m_parameters.get_value_throw< bcFLOAT >("intensity");
				const auto l_ambient_color_r = p_context.m_parameters.get_value_throw< bcFLOAT >("ambient_color_r");
				const auto l_ambient_color_g = p_context.m_parameters.get_value_throw< bcFLOAT >("ambient_color_g");
				const auto l_ambient_color_b = p_context.m_parameters.get_value_throw< bcFLOAT >("ambient_color_b");
				const auto l_ambient_intensity = p_context.m_parameters.get_value_throw< bcFLOAT >("ambient_intensity");

				m_light = l_light_manager.add_light(bc_direct_light
				(
					core::bc_vector3f::normalize(core::bc_vector3f(l_direction_x, l_direction_y, l_direction_z)),
					core::bc_vector3f(l_color_r, l_color_g, l_color_b),
					l_intensity, 
					core::bc_vector3f(l_ambient_color_r, l_ambient_color_g, l_ambient_color_b),
					l_ambient_intensity
				));
			}
			else if(l_light_type == "point")
			{
				const auto l_position_x = p_context.m_parameters.get_value_throw< bcFLOAT >("position_x");
				const auto l_position_y = p_context.m_parameters.get_value_throw< bcFLOAT >("position_y");
				const auto l_position_z = p_context.m_parameters.get_value_throw< bcFLOAT >("position_z");
				const auto l_radius = p_context.m_parameters.get_value_throw< bcFLOAT >("radius");
				const auto l_color_r = p_context.m_parameters.get_value_throw< bcFLOAT >("color_r");
				const auto l_color_g = p_context.m_parameters.get_value_throw< bcFLOAT >("color_g");
				const auto l_color_b = p_context.m_parameters.get_value_throw< bcFLOAT >("color_b");
				const auto l_intensity = p_context.m_parameters.get_value_throw< bcFLOAT >("intensity");
				const auto l_particle_cast = bc_null_default(p_context.m_parameters.get_value<bool>("particle_cast"), false);
				const auto l_particle_intensity = bc_null_default(p_context.m_parameters.get_value<bcFLOAT>("particle_intensity"), l_intensity);

				m_light = l_light_manager.add_light(bc_point_light
				(
					core::bc_vector3f(l_position_x, l_position_y, l_position_z),
					l_radius, 
					core::bc_vector3f(l_color_r, l_color_g, l_color_b),
					l_intensity,
					l_particle_cast,
					l_particle_intensity
				));
			}
			else if(l_light_type == "spot")
			{
				const auto l_position_x = p_context.m_parameters.get_value_throw< bcFLOAT >("position_x");
				const auto l_position_y = p_context.m_parameters.get_value_throw< bcFLOAT >("position_y");
				const auto l_position_z = p_context.m_parameters.get_value_throw< bcFLOAT >("position_z");
				const auto l_direction_x = p_context.m_parameters.get_value_throw< bcFLOAT >("direction_x");
				const auto l_direction_y = p_context.m_parameters.get_value_throw< bcFLOAT >("direction_y");
				const auto l_direction_z = p_context.m_parameters.get_value_throw< bcFLOAT >("direction_z");
				const auto l_length = p_context.m_parameters.get_value_throw< bcFLOAT >("length");
				const auto l_angle = p_context.m_parameters.get_value_throw< bcFLOAT >("angle");
				const auto l_color_r = p_context.m_parameters.get_value_throw< bcFLOAT >("color_r");
				const auto l_color_g = p_context.m_parameters.get_value_throw< bcFLOAT >("color_g");
				const auto l_color_b = p_context.m_parameters.get_value_throw< bcFLOAT >("color_b");
				const auto l_intensity = p_context.m_parameters.get_value_throw< bcFLOAT >("intensity");

				m_light = l_light_manager.add_light(bc_spot_light
				(
					core::bc_vector3f(l_position_x, l_position_y, l_position_z),
					core::bc_vector3f::normalize(core::bc_vector3f(l_direction_x, l_direction_y, l_direction_z)),
					l_length, 
					l_angle, 
					core::bc_vector3f(l_color_r, l_color_g, l_color_b),
					l_intensity
				));
			}
			else
			{
				BC_ASSERT(false);
			}
		}

		void bc_light_component::handle_event(bc_actor& p_actor, const bc_actor_event& p_event)
		{
			const auto* l_world_transform_event = core::bci_message::as<bc_actor_event_world_transform>(p_event);
			if(l_world_transform_event)
			{
				// TODO what if light is part of a mesh
				m_light->set_transformation(l_world_transform_event->get_transform());

				const auto& l_bound_box = m_light->get_bound_box();
				p_actor.add_event(bc_actor_event_bound_box_changed(l_bound_box));
			}
		}
	}
}