// [09/14/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/Content/bcLazyContent.h"
#include "Core/Content/bcContentManager.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/System/Render/bcRenderInstance.h"
#include "Game/System/Render/bcRenderStateBuffer.h"
#include "Game/System/Render/Particle/bcParticleManager.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"
#include "Game/Object/Scene/Component/bcDecalComponent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcBoundBoxChangedActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcBulletHitActorEvent.h"
#include "Game/bcConstant.h"

namespace black_cat
{
	namespace game
	{
		bc_height_map_component::bc_height_map_component(bc_actor_index p_actor_index, bc_actor_component_index p_index)
			: bci_actor_component(p_actor_index, p_index),
			bc_render_component(),
			bc_decal_resolver_component()
		{
		}

		bc_height_map_component::bc_height_map_component(bc_height_map_component&& p_other) noexcept
			: bci_actor_component(std::move(p_other)),
			bc_render_component(std::move(p_other)),
			bc_decal_resolver_component(std::move(p_other)),
			m_height_map(std::move(p_other.m_height_map))
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

			return *this;
		}

		bc_actor bc_height_map_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_height_map_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
			const auto& l_height_map_name = p_context.m_parameters.get_value_throw< core::bc_string >(constant::g_param_heightmap);
			m_height_map = p_context.m_stream_manager.find_content_throw<bc_height_map>(l_height_map_name.c_str());
		}

		void bc_height_map_component::write_instance(const bc_actor_component_write_context& p_context)
		{
			auto* l_content_manager = core::bc_get_service< core::bc_content_manager >();
			l_content_manager->save(*m_height_map);
		}

		void bc_height_map_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			const auto* l_world_transform_event = core::bci_message::as<bc_world_transform_actor_event>(p_context.m_event);
			if(l_world_transform_event)
			{
				m_transform.translate(l_world_transform_event->get_transform().get_translation());
				
				p_context.m_actor.add_event(bc_bound_box_changed_actor_event
				(
					physics::bc_bound_box
					(
						m_transform.get_translation(),
						core::bc_vector3f
						(
							(m_height_map->get_width() * m_height_map->get_xz_multiplier()) / 2,
							1024.0 / 2, // TODO Get actual terrain height
							(m_height_map->get_height() * m_height_map->get_xz_multiplier()) / 2
						)
					)
				));
			}

			const auto* l_bullet_hit_event = core::bci_message::as<bc_bullet_hit_actor_event>(p_context.m_event);
			if(l_bullet_hit_event)
			{
				const auto l_material_index = m_height_map->get_px_height_field().get_triangle_material(l_bullet_hit_event->get_hit_face_index());
				const auto& l_material = m_height_map->get_material(l_material_index);

				if(l_material.m_collider_material.m_collision_particle)
				{
					auto l_particle_color = l_material.m_mesh_material->get_diffuse().xyz();
					p_context.m_game_system.get_scene()->get_particle_manager().spawn_emitter
					(
						l_material.m_collider_material.m_collision_particle,
						l_bullet_hit_event->get_hit_position(),
						l_bullet_hit_event->get_hit_normal(),
						&l_particle_color,
						l_bullet_hit_event->get_bullet_mass() / get_global_config().get_bullet_reference_mass()
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
			const bc_render_instance l_instance(m_transform, bc_render_group::terrain);
			p_context.m_buffer.add_render_instance(m_height_map->get_render_state_ptr(), l_instance);
		}

		void bc_height_map_component::add_decal(const bcCHAR* p_decal_name, 
			const core::bc_vector3f& p_world_position, 
			const core::bc_vector3f& p_dir, 
			bc_mesh_node::node_index_t p_attached_node_index)
		{
			auto l_actor = get_actor();
			auto* l_decal_component = l_actor.get_create_component<bc_decal_component>();

			const auto l_local_pos = p_world_position - m_transform.get_translation();
			const auto l_world_pos = p_world_position - l_local_pos;
			core::bc_matrix3f l_local_rotation;

			if(graphic::bc_render_api_info::use_left_handed())
			{
				l_local_rotation.rotation_between_two_vector_lh(core::bc_vector3f::up(), p_dir);
			}
			else
			{
				l_local_rotation.rotation_between_two_vector_rh(core::bc_vector3f::up(), p_dir);
			}

			l_decal_component->add_decal
			(
				p_decal_name,
				l_local_pos,
				l_local_rotation,
				bc_render_group::terrain,
				core::bc_matrix4f::translation_matrix(l_world_pos)
			);
		}
	}
}