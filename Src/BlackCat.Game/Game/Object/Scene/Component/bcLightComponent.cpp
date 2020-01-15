// [03/01/2019 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/Component/bcLightComponent.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/System/Render/Light/bcLightManager.h"
#include "Game/System/bcGameSystem.h"

namespace black_cat
{
	namespace game
	{
		bc_light_component::bc_light_component(bc_actor_component_index p_index)
			: bc_iactor_component(p_index)
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

		void bc_light_component::set_world_transform(const core::bc_matrix4f& p_transform) noexcept
		{
			// TODO what if light is part of a mesh
			m_light->set_transformation(p_transform);

			const auto l_bound_box = m_light->get_bound_box();
			get_actor().get_component<bc_mediate_component>()->set_bound_box(l_bound_box);
		}

		const bc_light* bc_light_component::get_light() const noexcept
		{
			return m_light.get();
		}

		void bc_light_component::initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters)
		{
			auto& l_game_system = *core::bc_get_service<bc_game_system>();
			auto& l_light_manager = l_game_system.get_render_system().get_light_manager();
			auto& l_light_type = p_parameters.get_value_throw<core::bc_string>("type");

			if(l_light_type == "direct")
			{
				const auto l_direction_x = p_parameters.get_value_throw< bcFLOAT >("direction_x");
				const auto l_direction_y = p_parameters.get_value_throw< bcFLOAT >("direction_y");
				const auto l_direction_z = p_parameters.get_value_throw< bcFLOAT >("direction_z");
				const auto l_color_r = p_parameters.get_value_throw< bcFLOAT >("color_r");
				const auto l_color_g = p_parameters.get_value_throw< bcFLOAT >("color_g");
				const auto l_color_b = p_parameters.get_value_throw< bcFLOAT >("color_b");
				const auto l_intensity = p_parameters.get_value_throw< bcFLOAT >("intensity");
				const auto l_ambient_color_r = p_parameters.get_value_throw< bcFLOAT >("ambient_color_r");
				const auto l_ambient_color_g = p_parameters.get_value_throw< bcFLOAT >("ambient_color_g");
				const auto l_ambient_color_b = p_parameters.get_value_throw< bcFLOAT >("ambient_color_b");
				const auto l_ambient_intensity = p_parameters.get_value_throw< bcFLOAT >("ambient_intensity");

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
				const auto l_position_x = p_parameters.get_value_throw< bcFLOAT >("position_x");
				const auto l_position_y = p_parameters.get_value_throw< bcFLOAT >("position_y");
				const auto l_position_z = p_parameters.get_value_throw< bcFLOAT >("position_z");
				const auto l_radius = p_parameters.get_value_throw< bcFLOAT >("radius");
				const auto l_color_r = p_parameters.get_value_throw< bcFLOAT >("color_r");
				const auto l_color_g = p_parameters.get_value_throw< bcFLOAT >("color_g");
				const auto l_color_b = p_parameters.get_value_throw< bcFLOAT >("color_b");
				const auto l_intensity = p_parameters.get_value_throw< bcFLOAT >("intensity");

				m_light = l_light_manager.add_light(bc_point_light
				(
					core::bc_vector3f(l_position_x, l_position_y, l_position_z),
					l_radius, 
					core::bc_vector3f(l_color_r, l_color_g, l_color_b),
					l_intensity
				));
			}
			else if(l_light_type == "spot")
			{
				const auto l_position_x = p_parameters.get_value_throw< bcFLOAT >("position_x");
				const auto l_position_y = p_parameters.get_value_throw< bcFLOAT >("position_y");
				const auto l_position_z = p_parameters.get_value_throw< bcFLOAT >("position_z");
				const auto l_direction_x = p_parameters.get_value_throw< bcFLOAT >("direction_x");
				const auto l_direction_y = p_parameters.get_value_throw< bcFLOAT >("direction_y");
				const auto l_direction_z = p_parameters.get_value_throw< bcFLOAT >("direction_z");
				const auto l_length = p_parameters.get_value_throw< bcFLOAT >("length");
				const auto l_angle = p_parameters.get_value_throw< bcFLOAT >("angle");
				const auto l_color_r = p_parameters.get_value_throw< bcFLOAT >("color_r");
				const auto l_color_g = p_parameters.get_value_throw< bcFLOAT >("color_g");
				const auto l_color_b = p_parameters.get_value_throw< bcFLOAT >("color_b");
				const auto l_intensity = p_parameters.get_value_throw< bcFLOAT >("intensity");

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
				bcAssert(false);
			}
			
			set_world_transform(core::bc_matrix4f::identity());
		}

		void bc_light_component::handle_event(const bc_actor& p_actor, const bc_actor_event& p_event)
		{
		}
		
		void bc_light_component::update(const bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock_update_param)
		{
		}
	}
}