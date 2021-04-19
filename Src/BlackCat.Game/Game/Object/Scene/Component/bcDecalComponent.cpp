// [02/25/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcDecalComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcHierarchyTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcRemoveDecalActorEvent.h"
#include "Game/System/Render/Decal/bcDecalManager.h"
#include "Game/System/bcGameSystem.h"

namespace black_cat
{
	namespace game
	{
		bc_decal_component::bc_decal_component(bc_actor_index p_actor_index, bc_actor_component_index p_index) noexcept
			: bci_actor_component(p_actor_index, p_index),
			bc_render_component(),
			bc_const_iterator_adapter(m_decals),
			m_decal_manager(nullptr),
			m_decals_slot(0),
			m_mesh_scale(0),
			m_use_hierarchy_transforms(false)
		{
		}

		bc_decal_component::bc_decal_component(bc_decal_component&& p_other) noexcept
			: bci_actor_component(std::move(p_other)),
			bc_render_component(std::move(p_other)),
			bc_const_iterator_adapter(m_decals),
			m_decal_manager(p_other.m_decal_manager),
			m_decals_slot(p_other.m_decals_slot.load(core_platform::bc_memory_order::relaxed)),
			m_decals(std::move(p_other.m_decals)),
			m_mesh_scale(p_other.m_mesh_scale),
			m_use_hierarchy_transforms(p_other.m_use_hierarchy_transforms)
		{
		}

		bc_decal_component::~bc_decal_component() = default;

		bc_decal_component& bc_decal_component::operator=(bc_decal_component&& p_other) noexcept
		{
			bci_actor_component::operator=(std::move(p_other));
			bc_render_component::operator=(std::move(p_other));
			m_decal_manager = p_other.m_decal_manager;
			m_decals_slot.store(p_other.m_decals_slot.load(core_platform::bc_memory_order::relaxed), core_platform::bc_memory_order::relaxed);
			m_decals = std::move(p_other.m_decals);
			m_mesh_scale = p_other.m_mesh_scale;
			m_use_hierarchy_transforms = p_other.m_use_hierarchy_transforms;
			
			return *this;
		}
		
		bc_actor bc_decal_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_decal_component::add_decal(const bcCHAR* p_decal_name,
			const core::bc_vector3f& p_local_pos,
			const core::bc_matrix3f& p_local_rotation,
			const core::bc_matrix4f& p_initial_world_transform)
		{
			auto* l_decal = m_decal_manager->create_decal(p_decal_name, p_local_pos, p_local_rotation);
			l_decal->set_world_transform(p_initial_world_transform);
		}

		void bc_decal_component::add_decal(const bcCHAR* p_decal_name,
			const core::bc_vector3f& p_local_pos,
			const core::bc_matrix3f& p_local_rotation,
			bc_render_group p_render_group,
			const core::bc_matrix4f& p_initial_world_transform)
		{
			auto* l_decal = m_decal_manager->create_decal(p_decal_name, p_local_pos, p_local_rotation, p_render_group);
			l_decal->set_world_transform(p_initial_world_transform);
		}

		void bc_decal_component::add_decal(const bcCHAR* p_decal_name,
			const core::bc_vector3f& p_local_pos,
			const core::bc_matrix3f& p_local_rotation,
			const core::bc_matrix4f& p_initial_world_transform,
			bc_mesh_node::node_index_t p_attached_node)
		{
			if(p_attached_node == bc_mesh_node::s_invalid_index)
			{
				add_decal(p_decal_name, p_local_pos, p_local_rotation, p_initial_world_transform);
			}
			else
			{
				auto l_decal = m_decal_manager->create_decal(p_decal_name, get_actor(), p_local_pos, p_local_rotation, p_attached_node);
				l_decal->set_world_transform(p_initial_world_transform);

				_add_decal(std::move(l_decal));
			}
		}

		void bc_decal_component::add_decal(const bcCHAR* p_decal_name,
			const core::bc_vector3f& p_local_pos,
			const core::bc_matrix3f& p_local_rotation,
			bc_render_group p_render_group,
			const core::bc_matrix4f& p_initial_world_transform,
			bc_mesh_node::node_index_t p_attached_node)
		{
			if (p_attached_node == bc_mesh_node::s_invalid_index)
			{
				add_decal(p_decal_name, p_local_pos, p_local_rotation, p_initial_world_transform);
			}
			else
			{
				auto l_decal = m_decal_manager->create_decal(p_decal_name, get_actor(), p_local_pos, p_local_rotation, p_render_group, p_attached_node);
				l_decal->set_world_transform(p_initial_world_transform);

				_add_decal(std::move(l_decal));
			}
		}

		void bc_decal_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
			m_decal_manager = &p_context.m_game_system.get_render_system().get_decal_manager();
			m_decals.resize(m_decals_count);
		}

		void bc_decal_component::initialize_entity(const bc_actor_component_initialize_entity_context& p_context)
		{
			auto* l_mesh_component = p_context.m_actor.get_component<bc_mesh_component>();
			if(l_mesh_component)
			{
				m_mesh_scale = l_mesh_component->get_mesh().get_mesh_scale();
				m_use_hierarchy_transforms = l_mesh_component->get_mesh().get_skinned();
			}
		}

		void bc_decal_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			const auto* l_world_transform_event = core::bci_message::as<bc_world_transform_actor_event>(p_context.m_event);
			if(l_world_transform_event && !m_use_hierarchy_transforms)
			{
				for (bc_decal_instance_ptr& l_decal : m_decals)
				{
					if(!l_decal)
					{
						continue;
					}
					
					if (m_mesh_scale != 1)
					{
						l_decal->set_world_transform(core::bc_matrix4f::scale_matrix(m_mesh_scale) * l_world_transform_event->get_transform());
					}
					else
					{
						l_decal->set_world_transform(l_world_transform_event->get_transform());
					}
				}
				
				return;
			}

			const auto* l_hierarchy_event = core::bci_message::as<bc_hierarchy_transform_actor_event>(p_context.m_event);
			if(l_hierarchy_event && m_use_hierarchy_transforms)
			{
				const auto* l_mesh_component = p_context.m_actor.get_component<bc_mesh_component>();
				const auto& l_mesh = l_mesh_component->get_mesh();

				for (bc_decal_instance_ptr& l_decal : m_decals)
				{
					if (!l_decal)
					{
						continue;
					}
					
					const auto l_attached_node_index = l_decal->get_attached_node_index();
					if (l_attached_node_index == bc_mesh_node::s_invalid_index)
					{
						continue;
					}

					const auto& l_mesh_node = *l_mesh.find_node(l_attached_node_index);
					const auto& l_model_transform = l_hierarchy_event->get_mat4_transforms()->get_node_transform(l_mesh_node);
					const auto& l_inv_bind_pose_transform = l_mesh.get_node_inverse_bind_pose_transform(l_mesh_node);

					l_decal->set_world_transform(l_inv_bind_pose_transform * (l_model_transform * l_hierarchy_event->get_world_transform()));
				}
				
				return;
			}
			
			const auto* l_remove_event = core::bci_message::as<bc_remove_decal_actor_event>(p_context.m_event);
			if(l_remove_event)
			{
				const auto l_ite = std::find_if(std::cbegin(m_decals), std::cend(m_decals), [&](const bc_decal_instance_ptr& p_decal)
				{
					return p_decal.get() == &l_remove_event->get_decal_instance();
				});
				
				BC_ASSERT(l_ite != std::cend(m_decals));

				m_decals.erase(l_ite);
				return;
			}
		}

		void bc_decal_component::render(const bc_actor_component_render_context& p_context) const
		{
			for(const bc_decal_instance_ptr& l_decal_instance : m_decals)
			{
				auto* l_decal = l_decal_instance->get_decal();
				const auto l_lod_factor = std::max(l_decal->get_width(), l_decal->get_height()) * l_decal->get_lod_scale() * get_global_config().get_lod_global_scale();
				const auto l_camera_distance = (l_decal_instance->get_world_transform().get_translation() - p_context.m_camera.m_main_camera.get_position()).magnitude();
				const auto l_culling_index = static_cast<bcUINT32>(l_camera_distance / l_lod_factor);
				
				if(l_culling_index <= get_global_config().get_lod_culling_index())
				{
					p_context.m_buffer.add_decal_instance
					(
						l_decal_instance->get_decal_ptr(),
						bc_render_instance(l_decal_instance->get_world_transform(), bc_render_group::unknown)
					);
				}
			}
		}

		void bc_decal_component::_add_decal(bc_decal_instance_ptr p_decal) noexcept
		{
			while (true)
			{
				auto l_current_slot = m_decals_slot.load(core_platform::bc_memory_order::relaxed);
				const auto l_new_slot = (l_current_slot + 1) % m_decals_count;
				const auto l_compare_result = m_decals_slot.compare_exchange_strong
				(
					&l_current_slot, 
					l_new_slot,
					core_platform::bc_memory_order::relaxed
				);

				if(l_compare_result)
				{
					m_decals[l_current_slot] = std::move(p_decal);
					break;
				}
			}
		}
	}
}