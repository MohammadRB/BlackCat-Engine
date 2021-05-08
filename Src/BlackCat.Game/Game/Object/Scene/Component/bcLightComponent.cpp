// [03/01/2019 MRB]

#include "Game/GamePCH.h"

#include "Core/bcUtility.h"
#include "Core/Utility/bcNullable.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Render/Light/bcLightManager.h"
#include "Game/System/Render/Material/bcMaterialManager.h"
#include "Game/Object/Scene/Component/bcLightComponent.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcBoundBoxChangedActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcAddedToSceneActorEvent.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/bcException.h"

namespace black_cat
{
	namespace game
	{
		bc_light_component::bc_light_component(bc_actor_index p_actor_index, bc_actor_component_index p_index)
			: bci_actor_component(p_actor_index, p_index),
			m_light_holder(nullptr)
		{
		}

		bc_light_component::~bc_light_component()
		{
			if(m_light_holder)
			{
				BC_DELETE(m_light_holder);
			}
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

		void bc_light_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
			auto& l_material_manager = p_context.m_game_system.get_render_system().get_material_manager();
			const auto& l_light_type = p_context.m_parameters.get_value_throw<core::bc_string>(constant::g_param_light_type);
			
			if(l_light_type == "direct")
			{
				const auto l_direction = p_context.m_parameters.get_value_vector3f_throw(constant::g_param_light_direction);
				const auto l_color = p_context.m_parameters.get_value_vector4f_throw(constant::g_param_light_color);
				const auto l_ambient_color = p_context.m_parameters.get_value_vector4f_throw(constant::g_param_light_ambient_color);

				m_light_holder = BC_NEW
				(
					_bc_light_component_light_holder(bc_direct_light
					(
						core::bc_vector3f::normalize(l_direction),
						l_color.xyz(),
						l_color.w,
						l_ambient_color.xyz(),
						l_ambient_color.w
					)),
					core::bc_alloc_type::unknown
				);
			}
			else if(l_light_type == "point")
			{
				const auto l_position = p_context.m_parameters.get_value_vector3f_throw(constant::g_param_light_position);
				const auto l_color = p_context.m_parameters.get_value_vector4f_throw(constant::g_param_light_color);
				const auto l_radius = p_context.m_parameters.get_value_throw<bcFLOAT>(constant::g_param_light_radius);
				const auto l_particle_cast = bc_null_default(p_context.m_parameters.get_value<bool>(constant::g_param_light_particle_cast), false);
				const auto l_particle_intensity = bc_null_default(p_context.m_parameters.get_value<bcFLOAT>(constant::g_param_light_particle_intensity), l_color.w);
				const auto l_flare_surface = p_context.m_parameters.get_value_vector3f(constant::g_param_light_flare_surface);
				const auto* l_flare_mask_material = p_context.m_parameters.get_value<core::bc_string>(constant::g_param_light_flare_mask_material);

				core::bc_nullable<bc_light_flare> l_flare;
				if(l_flare_surface != nullptr && l_flare_mask_material)
				{
					const auto l_material = l_material_manager.load_mesh_material_throw(l_flare_mask_material->c_str());
					l_flare.reset(bc_light_flare
					(
						l_flare_surface->x,
						l_flare_surface->y,
						l_flare_surface->z,
						bc_null_default(p_context.m_parameters.get_value<bcFLOAT>(constant::g_param_light_flare_intensity), l_color.w),
						l_material_manager.load_mesh_material_throw(l_flare_mask_material->c_str()),
						bc_null_default(p_context.m_parameters.get_value<bcFLOAT>(constant::g_param_light_flare_mask_u0), 0),
						bc_null_default(p_context.m_parameters.get_value<bcFLOAT>(constant::g_param_light_flare_mask_v0), 0),
						bc_null_default(p_context.m_parameters.get_value<bcFLOAT>(constant::g_param_light_flare_mask_u1), l_material->get_diffuse_map().get_width()),
						bc_null_default(p_context.m_parameters.get_value<bcFLOAT>(constant::g_param_light_flare_mask_v1), l_material->get_diffuse_map().get_height())
					));
				}

				m_light_holder = BC_NEW
				(
					_bc_light_component_light_holder(bc_point_light
					(
						l_position,
						l_radius,
						l_color.xyz(),
						l_color.w,
						l_particle_cast,
						l_particle_intensity,
						l_flare.get()
					)),
					core::bc_alloc_type::unknown
				);
			}
			else if(l_light_type == "spot")
			{
				const auto l_position = p_context.m_parameters.get_value_vector3f_throw(constant::g_param_light_position);
				const auto l_direction = p_context.m_parameters.get_value_vector3f_throw(constant::g_param_light_direction);
				const auto l_color = p_context.m_parameters.get_value_vector4f_throw(constant::g_param_light_color);
				const auto l_length = p_context.m_parameters.get_value_throw<bcFLOAT>(constant::g_param_light_length);
				const auto l_angle = p_context.m_parameters.get_value_throw<bcFLOAT>(constant::g_param_light_angle);
				const auto l_flare_surface = p_context.m_parameters.get_value_vector3f(constant::g_param_light_flare_surface);
				const auto* l_flare_mask_material = p_context.m_parameters.get_value<core::bc_string>(constant::g_param_light_flare_mask_material);

				core::bc_nullable<bc_light_flare> l_flare;
				if (l_flare_surface != nullptr && l_flare_mask_material)
				{
					const auto l_material = l_material_manager.load_mesh_material_throw(l_flare_mask_material->c_str());
					l_flare.reset(bc_light_flare
					(
						l_flare_surface->x,
						l_flare_surface->y,
						l_flare_surface->z,
						bc_null_default(p_context.m_parameters.get_value<bcFLOAT>(constant::g_param_light_flare_intensity), l_color.w),
						l_material,
						bc_null_default(p_context.m_parameters.get_value<bcFLOAT>(constant::g_param_light_flare_mask_u0), 0),
						bc_null_default(p_context.m_parameters.get_value<bcFLOAT>(constant::g_param_light_flare_mask_v0), 0),
						bc_null_default(p_context.m_parameters.get_value<bcFLOAT>(constant::g_param_light_flare_mask_u1), l_material->get_diffuse_map().get_width()),
						bc_null_default(p_context.m_parameters.get_value<bcFLOAT>(constant::g_param_light_flare_mask_v1), l_material->get_diffuse_map().get_height())
					));
				}

				m_light_holder = BC_NEW
				(
					_bc_light_component_light_holder(bc_spot_light
					(
						l_position,
						core::bc_vector3f::normalize(l_direction),
						l_length,
						l_angle,
						l_color.xyz(),
						l_color.w,
						l_flare.get()
					)),
					core::bc_alloc_type::unknown
				);
			}
			else
			{
				BC_ASSERT(false);
				throw bc_invalid_argument_exception("Invalid light type");
			}
		}

		void bc_light_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			const auto* l_added_to_scene_event = core::bci_message::as<bc_added_to_scene_actor_event>(p_context.m_event);
			if (l_added_to_scene_event)
			{
				switch (m_light_holder->m_type)
				{
				case bc_light_type::direct:
					m_light = l_added_to_scene_event->get_scene().get_light_manager().add_light(m_light_holder->m_direct_light);
					break;
				case bc_light_type::point:
					m_light = l_added_to_scene_event->get_scene().get_light_manager().add_light(m_light_holder->m_point_light);
					break;
				case bc_light_type::spot:
					m_light = l_added_to_scene_event->get_scene().get_light_manager().add_light(m_light_holder->m_spot_light);
					break;
				}

				BC_DELETE(m_light_holder);

				return;
			}
			
			const auto* l_world_transform_event = core::bci_message::as<bc_world_transform_actor_event>(p_context.m_event);
			if(l_world_transform_event)
			{
				// TODO what if light is part of a mesh
				m_light->set_transformation(l_world_transform_event->get_transform());

				const auto& l_bound_box = m_light->get_bound_box();
				p_context.m_actor.add_event(bc_bound_box_changed_actor_event(l_bound_box));

				return;
			}
		}
	}
}