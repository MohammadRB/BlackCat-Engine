// [02/25/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcDecalComponent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcHierarchyTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcRemoveDecalActorEvent.h"
#include "Game/System/Render/Decal/bcDecalManager.h"
#include "Game/System/bcGameSystem.h"

namespace black_cat
{
	namespace game
	{
		bc_decal_component::bc_decal_component(bc_actor_index p_actor_index, bc_actor_component_index p_index)
			: bci_actor_component(p_actor_index, p_index),
			bc_render_component(),
			bc_const_iterator_adapter(m_decals),
			m_decal_name(nullptr)
		{
		}

		bc_decal_component::bc_decal_component(bc_decal_component&& p_other) noexcept
			: bci_actor_component(std::move(p_other)),
			bc_render_component(std::move(p_other)),
			bc_const_iterator_adapter(m_decals),
			m_decal_name(p_other.m_decal_name),
			m_decals(std::move(p_other.m_decals))
		{
		}

		bc_decal_component::~bc_decal_component() = default;

		bc_decal_component& bc_decal_component::operator=(bc_decal_component&& p_other) noexcept
		{
			bci_actor_component::operator=(std::move(p_other));
			bc_render_component::operator=(std::move(p_other));
			m_decal_name = p_other.m_decal_name;
			m_decals = std::move(p_other.m_decals);
			return *this;
		}
		
		bc_actor bc_decal_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_decal_component::add_decal(const core::bc_vector3f& p_local_pos,
			const core::bc_matrix3f& p_local_rotation,
			const core::bc_matrix4f& p_initial_world_transform,
			bc_mesh_node::node_index_t p_attached_node)
		{
			if(!m_decal_name)
			{
				return;
			}
			
			add_decal
			(
				m_decal_name,
				p_local_pos,
				p_local_rotation,
				p_initial_world_transform,
				p_attached_node
			);
		}

		void bc_decal_component::add_decal(const bcCHAR* p_decal_name,
			const core::bc_vector3f& p_local_pos,
			const core::bc_matrix3f& p_local_rotation,
			const core::bc_matrix4f& p_initial_world_transform,
			bc_mesh_node::node_index_t p_attached_node)
		{
			auto& l_decal_manager = core::bc_get_service<bc_game_system>()->get_render_system().get_decal_manager();
			const auto l_actor = p_attached_node != bc_mesh_node::s_invalid_index ? get_actor() : bc_actor();
			auto l_decal = l_decal_manager.create_decal(p_decal_name, l_actor, p_local_pos, p_local_rotation, p_attached_node);

			l_decal->set_world_transform(p_initial_world_transform);
			
			m_decals.push_back(std::move(l_decal));
		}

		void bc_decal_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
			const auto* l_decal_name = p_context.m_parameters.get_value<core::bc_string>(constant::g_param_decal_name);
			if (l_decal_name != nullptr)
			{
				m_decal_name = l_decal_name->c_str();
			}
		}

		void bc_decal_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			const auto* l_world_transform_event = core::bci_message::as<bc_world_transform_actor_event>(p_context.m_event);
			if(l_world_transform_event)
			{
				for(bc_decal_instance_ptr& l_decal : m_decals)
				{
					l_decal->set_world_transform(l_world_transform_event->get_transform());
				}
				return;
			}

			const auto* l_hierarchy_event = core::bci_message::as<bc_hierarchy_transform_actor_event>(p_context.m_event);
			if(l_hierarchy_event)
			{
				for (bc_decal_instance_ptr& l_decal : m_decals)
				{
					const auto l_attached_node_index = l_decal->get_attached_node_index();
					if(l_attached_node_index == bc_mesh_node::s_invalid_index)
					{
						continue;
					}
					
					if(l_hierarchy_event->get_px_transforms())
					{
						auto l_transform = (*l_hierarchy_event->get_px_transforms())[l_attached_node_index].get_matrix4();
						l_decal->set_world_transform(l_transform);
					}
					else
					{
						const auto& l_transform = (*l_hierarchy_event->get_mat4_transforms())[l_attached_node_index];
						l_decal->set_world_transform(l_transform);
					}
				}
				return;
			}
			
			const auto* l_remove_event = core::bci_message::as<bc_remove_decal_actor_event>(p_context.m_event);
			if(l_remove_event)
			{
				for (auto l_begin = std::cbegin(m_decals); l_begin != std::cend(m_decals); ++l_begin)
				{
					auto& l_decal = *l_begin;
					if(l_decal.get() == &l_remove_event->get_decal_instance())
					{
						m_decals.erase(l_begin);
					}
				}
				return;
			}
		}

		void bc_decal_component::render(const bc_actor_component_render_context& p_context) const
		{
			for(const bc_decal_instance_ptr& l_decal : m_decals)
			{
				const auto l_lod_factor = std::max(l_decal->get_decal()->get_width(), l_decal->get_decal()->get_height()) * 
					l_decal->get_decal()->get_lod_scale() * 
					get_global_config().get_lod_global_scale();
				const auto l_camera_distance = (l_decal->get_world_transform().get_translation() - p_context.m_camera.m_main_camera.get_position()).magnitude();
				const auto l_culling_index = static_cast<bcUINT32>(l_camera_distance / l_lod_factor);
				
				if(l_culling_index <= get_global_config().get_lod_culling_index())
				{
					p_context.m_buffer.add_decal_instance
					(
						l_decal->get_decal_ptr(),
						bc_render_instance(l_decal->get_world_transform(), bc_render_group::unknown)
					);
				}
			}
		}
	}
}