// [03/27/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Math/bcMatrix3f.h"
#include "Game/Object/Scene/Component/bcWeaponComponent.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/bcSoundComponent.h"
#include "Game/Object/Scene/Component/Event/bcAddedToSceneActorEvent.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManagerContainer.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Render/Particle/bcParticleManager.h"
#include "Game/System/Render/Light/bcLightManager.h"
#include "Game/System/Render/Material/bcMaterialManager.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Sound/bcSoundManager.h"
#include "Game/bcJsonParse.h"
#include "Game/bcUtility.h"
#include "Game/bcConstant.h"

namespace black_cat
{
	namespace game
	{
		bc_weapon_component::bc_weapon_component(bc_actor_id p_actor_id, bc_actor_component_id p_id)
			: bci_actor_component(p_actor_id, p_id),
			m_class(bc_weapon_class::rifle),
			m_fire_light_radius(0),
			m_fire_light_intensity(0),
			m_rate_of_fire_seconds(0),
			m_bullet_speed(0),
			m_bullet_mass(0),
			m_scene(nullptr),
			m_sound_component(nullptr),
			m_fire_light_age(0)
		{
		}

		bc_actor bc_weapon_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		bc_bullet bc_weapon_component::shoot(const core::bc_vector3f& p_aim_direction_ws, bc_network_client_id p_player_id)
		{
			const auto& l_transform = get_actor().get_component<bc_mediate_component>()->get_world_transform();
			const auto l_position = (l_transform * core::bc_vector4f(m_fire_offset_ls, 1)).xyz();
			const auto l_direction = core::bc_vector3f::normalize(l_transform.get_rotation() * m_forward_ls);

			m_scene->get_particle_manager().spawn_emitter(m_fire_particle_name.data(), l_position, l_direction);
			m_fire_light = m_scene->get_light_manager().add_light(bc_point_light
			(
				l_position,
				m_fire_light_radius,
				m_fire_light_color,
				m_fire_light_intensity,
				m_fire_light_flare.get()
			));
			m_fire_light_age = 0;

			m_sound_component->play_sounds();

			return bc_bullet(l_position, p_aim_direction_ws, m_bullet_speed, m_bullet_mass, p_player_id);
		}
		
		void bc_weapon_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
			const core::bc_string* l_class_value = nullptr;
			const core::bc_string* l_fire_particle_name;

			json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_weapon_class, l_class_value);

			if (*l_class_value == "rifle")
			{
				m_class = bc_weapon_class::rifle;
			}
			else
			{
				throw bc_invalid_argument_exception("Weapon class value is invalid");
			}

			json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_weapon_up_ls, m_up_ls);
			json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_weapon_forward_ls, m_forward_ls);
			json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_weapon_right_ls, m_right_ls);
			json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_weapon_main_hand_offset_ls, m_main_hand_offset_ls);
			json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_weapon_second_hand_offset_ls, m_second_hand_offset_ls);
			json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_weapon_fire_offset_ls, m_fire_offset_ls);
			json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_weapon_fire_particle, l_fire_particle_name);
			json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_weapon_fire_light_color, m_fire_light_color);
			json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_weapon_fire_light_radius, m_fire_light_radius);
			json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_weapon_fire_light_intensity, m_fire_light_intensity);
			json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_weapon_rate_of_fire_seconds, m_rate_of_fire_seconds);
			json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_weapon_bullet_speed, m_bullet_speed);
			json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_weapon_bullet_mass, m_bullet_mass);

			m_fire_particle_name = *l_fire_particle_name;
			m_scene = &p_context.m_scene;
			m_sound_component = p_context.m_actor.get_component<bc_sound_component>();

			core::bc_vector3f l_flare_surface;
			const core::bc_string* l_flare_mask_material = nullptr;
			json_parse::bc_load(p_context.m_parameters, constant::g_param_weapon_fire_light_flare_surface, l_flare_surface);
			json_parse::bc_load(p_context.m_parameters, constant::g_param_weapon_fire_light_flare_mask_material, l_flare_mask_material);
			
			if (l_flare_mask_material)
			{
				auto& l_material_manager = p_context.m_game_system.get_render_system().get_material_manager();
				const auto l_material = l_material_manager.load_mesh_material_throw(l_flare_mask_material->c_str());

				bcFLOAT l_fire_light_intensity = m_fire_light_intensity;
				bcFLOAT l_fire_light_radius = m_fire_light_radius;
				json_parse::bc_load(p_context.m_parameters, constant::g_param_weapon_fire_light_flare_intensity, l_fire_light_intensity);
				json_parse::bc_load(p_context.m_parameters, constant::g_param_weapon_fire_light_flare_size, l_fire_light_radius);

				m_fire_light_flare.reset(bc_light_flare
				(
					l_flare_surface.x,
					l_flare_surface.y,
					l_flare_surface.z,
					l_fire_light_intensity,
					l_fire_light_radius,
					l_material,
					0,
					0,
					l_material->get_diffuse_map().get_width(),
					l_material->get_diffuse_map().get_height()
				));
			}
		}

		void bc_weapon_component::update(const bc_actor_component_update_content& p_context)
		{
			if(!m_fire_light)
			{
				return;
			}

			constexpr bcFLOAT l_light_lifetime = 0.2f;
			m_fire_light_age += p_context.m_clock.m_elapsed_second;

			if(m_fire_light_age > l_light_lifetime)
			{
				m_fire_light.reset();
				return;
			}

			auto* l_point_light = m_fire_light->as_point_light();
			const auto l_normal_age = pow(1 - (m_fire_light_age / l_light_lifetime), 2);
			l_point_light->set_intensity(m_fire_light_intensity * l_normal_age);
		}

		void bc_weapon_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			/*const auto* l_scene_add_event = core::bci_message::as<bc_added_to_scene_actor_event>(p_context.m_event);
			if(l_scene_add_event)
			{
				m_scene = &l_scene_add_event->get_scene();
			}*/
		}
	}	
}
