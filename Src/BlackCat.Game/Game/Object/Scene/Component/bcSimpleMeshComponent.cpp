// [09/09/2019 MRB]

#include "Game/GamePCH.h"

#include "PhysicsImp/Shape/bcShape.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/System/Physics/bcPxShapeData.h"
#include "Game/System/Render/Particle/bcParticleManager.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/Component/bcSimpleMeshComponent.h"
#include "Game/Object/Scene/Component/bcRigidBodyComponent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcBoundBoxChangedActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcBulletHitActorEvent.h"

namespace black_cat
{
	namespace game
	{
		bc_simple_mesh_component::bc_simple_mesh_component(bc_actor_index p_actor_index, bc_actor_component_index p_index)
			: bci_actor_component(p_actor_index, p_index),
			bc_mesh_component(),
			m_render_group(bc_render_group::unknown)
		{
		}

		bc_simple_mesh_component::bc_simple_mesh_component(bc_simple_mesh_component&& p_other) noexcept
			: bci_actor_component(std::move(p_other)),
			bc_mesh_component(std::move(p_other)),
			m_render_group(p_other.m_render_group)
		{
		}

		bc_simple_mesh_component::~bc_simple_mesh_component()
		{
		}

		bc_simple_mesh_component& bc_simple_mesh_component::operator=(bc_simple_mesh_component&& p_other) noexcept
		{
			bci_actor_component::operator=(std::move(p_other));
			bc_mesh_component::operator=(std::move(p_other));
			m_render_group = p_other.m_render_group;
			
			return *this;
		}

		bc_actor bc_simple_mesh_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_simple_mesh_component::initialize_entity(const bc_actor_component_initialize_entity_context& p_context)
		{
			auto* l_rigid_body_component = get_actor().get_component<bc_rigid_body_component>();
			if (l_rigid_body_component)
			{
				const auto l_px_actor_type = l_rigid_body_component->get_body().get_type();
				if (l_px_actor_type == physics::bc_actor_type::rigid_static)
				{
					m_render_group = bc_render_group::static_mesh;
				}
				else
				{
					m_render_group = bc_render_group::dynamic_mesh;
				}
			}
			else
			{
				m_render_group = bc_render_group::static_mesh;
			}
		}

		void bc_simple_mesh_component::render(const bc_actor_component_render_context& p_context) const
		{
			const auto l_mesh_lod = get_mesh().get_mesh_level_of_detail();
			const auto l_lod = l_mesh_lod.get_lod_culling
			(
				p_context.m_camera.m_main_camera.get_position(),
				p_context.m_camera.m_render_camera.get_position(),
				get_world_position(),
				get_lod_factor()
			);
			if (!l_lod.second)
			{
				return;
			}
			
			const auto& l_render_states = get_render_states();
			const auto& l_world_transforms = get_world_transforms();
			bc_mesh_utility::render_mesh(p_context.m_buffer, l_render_states, l_world_transforms, l_lod.first, m_render_group);
		}
		
		void bc_simple_mesh_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			const auto* l_world_transform_event = core::bci_message::as<bc_world_transform_actor_event>(p_context.m_event);
			if(l_world_transform_event)
			{
				bc_mesh_component::set_world_transform(p_context.m_actor, l_world_transform_event->get_transform());
				return;
			}

			const auto* l_bound_box_event = core::bci_message::as<bc_bound_box_changed_actor_event>(p_context.m_event);
			if(l_bound_box_event)
			{
				bc_mesh_component::update_lod_factor(l_bound_box_event->get_bound_box());
				return;
			}

			const auto* l_bullet_hit_event = core::bci_message::as<bc_bullet_hit_actor_event>(p_context.m_event);
			if(l_bullet_hit_event)
			{
				auto* l_hit_shape_data = static_cast<bc_px_shape_data*>(l_bullet_hit_event->get_hit_shape().get_data());
				BC_ASSERT(l_hit_shape_data);
				
				if(l_hit_shape_data->m_collision_particle)
				{
					p_context.m_game_system.get_render_system().get_particle_manager().spawn_emitter
					(
						l_hit_shape_data->m_collision_particle,
						l_bullet_hit_event->get_hit_position(),
						l_bullet_hit_event->get_hit_normal(),
						nullptr,
						l_bullet_hit_event->get_bullet_mass() / get_global_config().get_bullet_reference_mass()
					);
				}
				
				if (l_hit_shape_data->m_collision_decal)
				{
					add_decal
					(
						l_hit_shape_data->m_collision_decal,
						l_bullet_hit_event->get_hit_position(),
						l_bullet_hit_event->get_hit_normal(),
						l_hit_shape_data->m_collider_entry->m_attached_node_transform_index
					);
				}
				
				return;
			}
		}
	}
}