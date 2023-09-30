// [14/09/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/Content/bcLazyContent.h"
#include "Core/Content/bcContentManager.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/System/Render/bcRenderInstance.h"
#include "Game/System/Render/bcRenderStateBuffer.h"
#include "Game/System/Render/Particle/bcParticleManager.h"
#include "Game/System/Physics/bcPhysicsShapeUtility.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManagerContainer.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"
#include "Game/Object/Scene/Component/bcDecalComponent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcBoundBoxChangedActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcBulletHitActorEvent.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/bcJsonParse.h"
#include "Game/bcUtility.h"
#include "Game/bcConstant.h"

namespace black_cat::game
{
	bc_height_map_component::bc_height_map_component(bc_actor_id p_actor_id, bc_actor_component_id p_id)
		: bci_actor_component(p_actor_id, p_id),
		  bc_render_component(),
		  bc_decal_resolver_component(),
		  m_transform()
	{
	}

	bc_height_map_component::bc_height_map_component(bc_height_map_component&& p_other) noexcept
		: bci_actor_component(std::move(p_other)),
		  bc_render_component(std::move(p_other)),
		  bc_decal_resolver_component(std::move(p_other)),
		  m_height_map(std::move(p_other.m_height_map)),
		  m_transform(p_other.m_transform)
	{
	}

	bc_height_map_component::~bc_height_map_component()
	{
	}

	bc_height_map_component& bc_height_map_component::operator=(bc_height_map_component&& p_other) noexcept
	{
		bci_actor_component::operator=(std::move(p_other));
		bc_render_component::operator=(std::move(p_other));
		bc_decal_resolver_component::operator=(std::move(p_other));
		m_height_map = std::move(p_other.m_height_map);
		m_transform = p_other.m_transform;

		return *this;
	}

	bc_actor bc_height_map_component::get_actor() const noexcept
	{
		return get_manager().component_get_actor(*this);
	}

	void bc_height_map_component::initialize(const bc_actor_component_initialize_context& p_context)
	{
		const auto* l_height_map_name = static_cast<core::bc_string*>(nullptr);
		json_parse::bc_load_throw(p_context.m_parameters, constant::g_param_heightmap, l_height_map_name);

		m_height_map = p_context.m_stream_manager.find_content_throw<bc_height_map>(l_height_map_name->c_str());
	}

	void bc_height_map_component::load_instance(const bc_actor_component_load_context& p_context)
	{
		bc_decal_resolver_component::load_instance(p_context);
	}

	void bc_height_map_component::write_instance(const bc_actor_component_write_context& p_context)
	{
		bc_decal_resolver_component::write_instance(p_context);
			
		auto* l_content_manager = core::bc_get_service<core::bc_content_manager>();
		l_content_manager->save(*m_height_map);
	}

	void bc_height_map_component::handle_event(const bc_actor_component_event_context& p_context)
	{
		if(const auto* l_world_transform_event = core::bci_message::as<bc_world_transform_actor_event>(p_context.m_event))
		{
			m_transform.translate(l_world_transform_event->get_transform().get_translation());

			const auto l_height_map_width = m_height_map->get_width() * m_height_map->get_xz_multiplier();
			const auto l_height_map_height = m_height_map->get_height() * m_height_map->get_xz_multiplier();
			p_context.m_actor.add_event(bc_bound_box_changed_actor_event
			(
				physics::bc_bound_box
				(
					m_transform.get_translation(),
					core::bc_vector3f
					(
						static_cast<bcFLOAT>(l_height_map_width) / 2,
						m_height_map->get_y_multiplier() * 0.8f, // TODO Get actual terrain height
						static_cast<bcFLOAT>(l_height_map_height) / 2
					)
				)
			));
		}

		if(const auto* l_bullet_hit_event = core::bci_message::as<bc_bullet_hit_actor_event>(p_context.m_event))
		{
			const auto l_material_index = m_height_map->get_px_height_field().get_triangle_material(l_bullet_hit_event->get_hit_face_index());
			const auto& l_material = m_height_map->get_material(l_material_index);

			if(l_material.m_collider_material.m_collision_particle)
			{
				const auto l_particle_color = l_material.m_mesh_material->get_diffuse().xyz();
				p_context.m_game_system.get_scene()->get_particle_manager().spawn_emitter
				(
					l_material.m_collider_material.m_collision_particle,
					l_bullet_hit_event->get_hit_position(),
					l_bullet_hit_event->get_hit_normal(),
					&l_particle_color,
					l_bullet_hit_event->get_bullet_mass() / bc_get_global_config().get_scene_bullet_reference_mass()
				);
			}
				
			if(l_material.m_collider_material.m_collision_decal)
			{
				add_decal
				(
					l_material.m_collider_material.m_collision_decal, 
					l_bullet_hit_event->get_hit_position(), 
					l_bullet_hit_event->get_hit_normal()
				);
			}
		}
	}

	void bc_height_map_component::render(const bc_actor_component_render_context& p_context) const
	{
		const bc_render_instance l_instance(m_transform, bc_actor_render_group::terrain);
		p_context.m_buffer.add_render_instance(m_height_map->get_render_state_ptr(), l_instance);
	}

	void bc_height_map_component::add_decal(const bcCHAR* p_decal_name, 
	                                        const core::bc_vector3f& p_world_position, 
	                                        const core::bc_vector3f& p_dir, 
	                                        bc_mesh_node::node_index_t p_attached_node_index)
	{
		auto l_actor = get_actor();
		const auto l_local_pos = p_world_position - m_transform.get_translation();
		const auto l_world_pos = p_world_position - l_local_pos;
		const auto l_local_rotation = bc_matrix3f_rotation_between_two_vector(core::bc_vector3f::up(), p_dir);
			
		auto* l_decal_component = l_actor.get_create_component<bc_decal_component>();
		l_decal_component->add_decal
		(
			p_decal_name,
			l_local_pos,
			l_local_rotation,
			bc_actor_render_group::terrain,
			core::bc_matrix4f::translation_matrix(l_world_pos)
		);
	}

	bc_pickup_proxy_result bc_height_map_component::ray_pickup(const physics::bc_ray& p_ray) const
	{
		auto l_ray_hit = physics::bc_ray_hit();
		const auto l_px_height_field_shape = physics::bc_shape_height_field
		(
			get_height_map().get_px_height_field(),
			get_height_map().get_xz_multiplier(),
			get_height_map().get_y_multiplier()
		);
		const bool l_intersects = physics::bc_shape_query::ray_cast
		(
			p_ray,
			l_px_height_field_shape,
			bc_convert_to_height_map_transform(get_height_map(), physics::bc_transform(get_world_position())),
			physics::bc_hit_flag::distance,
			&l_ray_hit,
			1
		);

		return { l_intersects ? bc_pickup_proxy_state::pickup : bc_pickup_proxy_state::no_pickup, l_ray_hit };
	}
}
