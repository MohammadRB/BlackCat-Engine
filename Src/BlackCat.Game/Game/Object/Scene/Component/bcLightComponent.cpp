// [01/03/2019 MRB]

#include "Game/GamePCH.h"

#include "Core/bcUtility.h"
#include "Core/Utility/bcNullable.h"
#include "Game/System/Render/Light/bcLightManager.h"
#include "Game/System/Render/Material/bcMaterialManager.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Scene/Component/bcLightComponent.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManagerContainer.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcBoundBoxChangedActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcAddedToSceneActorEvent.h"
#include "Game/Object/Scene/Component/bcIconComponent.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/bcJsonParse.h"
#include "Game/bcException.h"
#include "Game/bcConstant.h"

namespace black_cat::game
{
	bc_light_component::bc_light_component(bc_actor_id p_actor_id, bc_actor_component_id p_index)
		: bci_actor_component(p_actor_id, p_index)
	{
	}

	bc_light_component::~bc_light_component() = default;

	bc_actor bc_light_component::get_actor() const noexcept
	{
		return get_manager().component_get_actor(*this);
	}

	void bc_light_component::initialize(const bc_actor_component_initialize_context& p_context)
	{
		auto& l_material_manager = p_context.m_game_system.get_render_system().get_material_manager();
		auto& l_light_manager = p_context.m_scene.get_light_manager();
		const auto* l_light_type = static_cast<core::bc_string*>(nullptr);
		const bcCHAR* l_light_icon_type = nullptr;

		json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_light_type, l_light_type);
			
		if(*l_light_type == "direct")
		{
			core::bc_vector3f l_direction;
			core::bc_vector4f l_color;
			core::bc_vector4f l_ambient_color;

			json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_light_direction, l_direction);
			json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_light_color, l_color);
			json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_light_ambient_color, l_ambient_color);

			m_light = l_light_manager.add_light(bc_direct_light
				(
					core::bc_vector3f::normalize(l_direction),
					l_color.xyz(),
					l_color.w,
					l_ambient_color.xyz(),
					l_ambient_color.w
				));
			l_light_icon_type = bc_icon_type::sun;
		}
		else if(*l_light_type == "point")
		{
			core::bc_vector3f l_position;
			core::bc_vector4f l_color;
			bcFLOAT l_radius;

			json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_light_position, l_position);
			json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_light_color, l_color);
			json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_light_radius, l_radius);

			bool l_particle_cast = false;
			bcFLOAT l_particle_intensity = l_color.w;
			core::bc_vector3f l_flare_surface;
			const core::bc_string* l_flare_mask_material = nullptr;

			json_parse::bc_load(p_context.m_parameters, constant::g_param_light_particle_cast, l_particle_cast);
			json_parse::bc_load(p_context.m_parameters, constant::g_param_light_particle_intensity, l_particle_intensity);
			json_parse::bc_load(p_context.m_parameters, constant::g_param_light_flare_surface, l_flare_surface);
			json_parse::bc_load(p_context.m_parameters, constant::g_param_light_flare_mask_material, l_flare_mask_material);
				
			core::bc_nullable<bc_light_flare> l_flare;
			if(l_flare_mask_material)
			{
				const auto l_material = l_material_manager.load_mesh_material_throw(l_flare_mask_material->c_str());

				bcFLOAT l_flare_intensity = l_color.w;
				bcFLOAT l_flare_size = l_radius;
				bcFLOAT l_flare_u0 = 0;
				bcFLOAT l_flare_v0 = 0;
				bcFLOAT l_flare_u1 = l_material->get_diffuse_map().get_width();
				bcFLOAT l_flare_v1 = l_material->get_diffuse_map().get_height();

				json_parse::bc_load(p_context.m_parameters, constant::g_param_light_flare_intensity, l_flare_intensity);
				json_parse::bc_load(p_context.m_parameters, constant::g_param_light_flare_size, l_flare_size);
				json_parse::bc_load(p_context.m_parameters, constant::g_param_light_flare_mask_u0, l_flare_u0);
				json_parse::bc_load(p_context.m_parameters, constant::g_param_light_flare_mask_v0, l_flare_v0);
				json_parse::bc_load(p_context.m_parameters, constant::g_param_light_flare_mask_u1, l_flare_u1);
				json_parse::bc_load(p_context.m_parameters, constant::g_param_light_flare_mask_v1, l_flare_v1);

				l_flare.reset(bc_light_flare
					(
						l_flare_surface.x,
						l_flare_surface.y,
						l_flare_surface.z,
						l_flare_intensity,
						l_flare_size,
						l_material,
						l_flare_u0,
						l_flare_v0,
						l_flare_u1,
						l_flare_v1
					));
			}

			m_light = l_light_manager.add_light(bc_point_light
				(
					l_position,
					l_radius,
					l_color.xyz(),
					l_color.w,
					l_particle_cast,
					l_particle_intensity,
					l_flare.get()
				));
			l_light_icon_type = bc_icon_type::light;
		}
		else if(*l_light_type == "spot")
		{
			core::bc_vector3f l_position;
			core::bc_vector3f l_direction;
			core::bc_vector4f l_color;
			bcFLOAT l_length;
			bcFLOAT l_angle;
			core::bc_vector3f l_flare_surface;
			const core::bc_string* l_flare_mask_material = nullptr;

			json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_light_position, l_position);
			json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_light_direction, l_direction);
			json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_light_color, l_color);
			json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_light_length, l_length);
			json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_light_angle, l_angle);
			json_parse::bc_load(p_context.m_parameters, constant::g_param_light_flare_surface, l_flare_surface);
			json_parse::bc_load(p_context.m_parameters, constant::g_param_light_flare_mask_material, l_flare_mask_material);

			core::bc_nullable<bc_light_flare> l_flare;
			if (l_flare_mask_material)
			{
				const auto l_material = l_material_manager.load_mesh_material_throw(l_flare_mask_material->c_str());

				bcFLOAT l_flare_intensity = l_color.w;
				bcFLOAT l_flare_size = l_length / 2;
				bcFLOAT l_flare_u0 = 0;
				bcFLOAT l_flare_v0 = 0;
				bcFLOAT l_flare_u1 = l_material->get_diffuse_map().get_width();
				bcFLOAT l_flare_v1 = l_material->get_diffuse_map().get_height();

				json_parse::bc_load(p_context.m_parameters, constant::g_param_light_flare_intensity, l_flare_intensity);
				json_parse::bc_load(p_context.m_parameters, constant::g_param_light_flare_size, l_flare_size);
				json_parse::bc_load(p_context.m_parameters, constant::g_param_light_flare_mask_u0, l_flare_u0);
				json_parse::bc_load(p_context.m_parameters, constant::g_param_light_flare_mask_v0, l_flare_v0);
				json_parse::bc_load(p_context.m_parameters, constant::g_param_light_flare_mask_u1, l_flare_u1);
				json_parse::bc_load(p_context.m_parameters, constant::g_param_light_flare_mask_v1, l_flare_v1);

				l_flare.reset(bc_light_flare
					(
						l_flare_surface.x,
						l_flare_surface.y,
						l_flare_surface.z,
						l_flare_intensity,
						l_flare_size,
						l_material,
						l_flare_u0,
						l_flare_v0,
						l_flare_u1,
						l_flare_v1
					));
			}

			m_light = l_light_manager.add_light(bc_spot_light
				(
					l_position,
					core::bc_vector3f::normalize(l_direction),
					l_length,
					l_angle,
					l_color.xyz(),
					l_color.w,
					l_flare.get()
				));
			l_light_icon_type = bc_icon_type::light;
		}
		else
		{
			throw bc_invalid_argument_exception("Invalid light type");
		}

		auto* l_icon_component = p_context.m_actor.get_create_component<bc_icon_component>();
		l_icon_component->set_icon(l_light_icon_type);
	}

	void bc_light_component::handle_event(const bc_actor_component_event_context& p_context)
	{
		if(const auto* l_world_transform_event = core::bci_message::as<bc_world_transform_actor_event>(p_context.m_event))
		{
			// TODO what if light is part of a mesh
			const auto& l_transform = l_world_transform_event->get_transform();
			m_light->set_transformation(l_transform);

			//const auto& l_bound_box = physics::bc_bound_box(l_transform.get_translation(), core::bc_vector3f(.5f));
			//p_context.m_actor.add_event(bc_bound_box_changed_actor_event(l_bound_box));

			return;
		}
	}
}
