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
#include "Game/Object/Scene/Component/bcDecalComponent.h"
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
				const auto l_px_actor_type = l_rigid_body_component->get_body_type();
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

			/*const auto* l_bound_box_event = core::bci_message::as<bc_bound_box_changed_actor_event>(p_context.m_event);
			if(l_bound_box_event)
			{
				bc_mesh_component::set_lod_factor(l_bound_box_event->get_bound_box());
				return;
			}*/

			const auto* l_bullet_hit_event = core::bci_message::as<bc_bullet_hit_actor_event>(p_context.m_event);
			if(l_bullet_hit_event)
			{
				bc_mesh_component::process_bullet_hit
				(
					p_context.m_game_system.get_scene()->get_particle_manager(), 
					*l_bullet_hit_event,
					m_render_group == bc_render_group::all_dynamic
				);
				return;
			}
		}

		void bc_simple_mesh_component::add_decal(const bcCHAR* p_decal_name,
			const core::bc_vector3f& p_world_position,
			const core::bc_vector3f& p_world_direction,
			bc_mesh_node::node_index_t p_attached_node_index)
		{
			core::bc_vector3f l_local_position;
			core::bc_vector3f l_local_direction;
			core::bc_matrix3f l_local_rotation;
			core::bc_matrix4f l_world_transform;

			if (p_attached_node_index == bc_mesh_node::s_invalid_index)
			{
				bc_mesh_utility::calculate_mesh_decal
				(
					p_world_position,
					p_world_direction,
					l_local_position,
					l_local_direction,
					l_local_rotation,
					l_world_transform
				);
			}
			else
			{
				bc_mesh_utility::calculate_mesh_decal
				(
					p_world_position,
					p_world_direction,
					get_world_transforms()[p_attached_node_index],
					l_local_position,
					l_local_direction,
					l_local_rotation,
					l_world_transform
				);
			}

			auto* l_decal_component = get_actor().get_create_component<bc_decal_component>();
			l_decal_component->add_decal(p_decal_name, l_local_position, l_local_rotation, m_render_group, l_world_transform, p_attached_node_index);
		}
	}
}