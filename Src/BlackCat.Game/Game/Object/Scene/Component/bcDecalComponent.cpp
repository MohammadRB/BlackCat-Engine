// [25/02/2021 MRB]

#include "Game/GamePCH.h"

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManagerContainer.h"
#include "Game/Object/Scene/Component/bcDecalComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcHierarchyTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcRemoveDecalActorEvent.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/System/Render/Decal/bcDecalManager.h"
#include "Game/System/bcGameSystem.h"
#include "Game/bcUtility.h"
#include "Game/bcConstant.h"

namespace black_cat::game
{
	BC_JSON_STRUCTURE(_bc_decal_parameter_json)
	{
		BC_JSON_VALUE(core::bc_string_frame, nm);
		BC_JSON_VALUE(core::bc_vector3f, pos_ls);
		BC_JSON_VALUE(core::bc_vector3f, rot_ls);
		BC_JSON_VALUE(bcINT32, rg);
		BC_JSON_VALUE(bc_mesh_node::node_index_t, ani);
	};
		
	BC_JSON_STRUCTURE(_bc_decal_array_json)
	{
		BC_JSON_ARRAY(_bc_decal_parameter_json, decals);
	};
		
	bc_decal_component::bc_decal_component(bc_actor_id p_actor_id, bc_actor_component_id p_id) noexcept
		: bci_actor_component(p_actor_id, p_id),
		  bc_render_component(),
		  bc_const_iterator_adapter(m_persistent_decals),
		  m_decal_manager(nullptr),
		  m_mesh_scale(0),
		  m_use_hierarchy_transforms(false),
		  m_temporary_decals_slot(0)
	{
	}

	bc_decal_component::bc_decal_component(bc_decal_component&& p_other) noexcept
		: bci_actor_component(std::move(p_other)),
		  bc_render_component(std::move(p_other)),
		  bc_const_iterator_adapter(m_persistent_decals),
		  m_decal_manager(p_other.m_decal_manager),
		  m_mesh_scale(p_other.m_mesh_scale),
		  m_use_hierarchy_transforms(p_other.m_use_hierarchy_transforms),
		  m_temporary_decals_slot(p_other.m_temporary_decals_slot),
		  m_temporary_decals(std::move(p_other.m_temporary_decals))
	{
	}

	bc_decal_component::~bc_decal_component() = default;

	bc_decal_component& bc_decal_component::operator=(bc_decal_component&& p_other) noexcept
	{
		bci_actor_component::operator=(std::move(p_other));
		bc_render_component::operator=(std::move(p_other));
		m_decal_manager = p_other.m_decal_manager;
		m_mesh_scale = p_other.m_mesh_scale;
		m_use_hierarchy_transforms = p_other.m_use_hierarchy_transforms;
		m_temporary_decals_slot = p_other.m_temporary_decals_slot;
		m_temporary_decals = std::move(p_other.m_temporary_decals);
			
		return *this;
	}
		
	bc_actor bc_decal_component::get_actor() const noexcept
	{
		return get_manager().component_get_actor(*this);
	}

	void bc_decal_component::add_decal(core::bc_string_view p_name,
	                                   const core::bc_vector3f& p_local_pos,
	                                   const core::bc_matrix3f& p_local_rotation,
	                                   const core::bc_matrix4f& p_initial_world_transform)
	{
		const auto l_is_auto_remove = m_decal_manager->is_decal_auto_remove(p_name);
			
		if(!l_is_auto_remove)
		{
			auto l_decal = m_decal_manager->create_decal(p_name, get_actor(), p_local_pos, p_local_rotation);
			if(!l_decal)
			{
				return;
			}

			l_decal->set_world_transform(p_initial_world_transform);
			_add_decal_if_needed(std::move(l_decal), false);
		}
		else
		{
			auto* l_decal = m_decal_manager->create_decal(p_name, p_local_pos, p_local_rotation);
			if (!l_decal)
			{
				return;
			}

			l_decal->set_world_transform(p_initial_world_transform);
		}
	}

	void bc_decal_component::add_decal(core::bc_string_view p_name,
	                                   const core::bc_vector3f& p_local_pos,
	                                   const core::bc_matrix3f& p_local_rotation,
	                                   bc_actor_render_group p_render_group,
	                                   const core::bc_matrix4f& p_initial_world_transform)
	{
		const auto l_is_auto_remove = m_decal_manager->is_decal_auto_remove(p_name);

		if (!l_is_auto_remove)
		{
			auto l_decal = m_decal_manager->create_decal(p_name, get_actor(), p_local_pos, p_local_rotation, p_render_group);
			if (!l_decal)
			{
				return;
			}

			l_decal->set_world_transform(p_initial_world_transform);
			_add_decal_if_needed(std::move(l_decal), false);
		}
		else
		{
			auto* l_decal = m_decal_manager->create_decal(p_name, p_local_pos, p_local_rotation, p_render_group);
			if (!l_decal)
			{
				return;
			}

			l_decal->set_world_transform(p_initial_world_transform);
		}
	}

	void bc_decal_component::add_decal(core::bc_string_view p_name,
	                                   const core::bc_vector3f& p_local_pos,
	                                   const core::bc_matrix3f& p_local_rotation,
	                                   const core::bc_matrix4f& p_initial_world_transform,
	                                   bc_mesh_node::node_index_t p_attached_node)
	{
		if(p_attached_node == bc_mesh_node::s_invalid_index)
		{
			add_decal(p_name, p_local_pos, p_local_rotation, p_initial_world_transform);
		}
		else
		{
			auto l_decal = m_decal_manager->create_decal(p_name, get_actor(), p_local_pos, p_local_rotation, p_attached_node);
			if (!l_decal)
			{
				return;
			}
				
			l_decal->set_world_transform(p_initial_world_transform);
			_add_decal_if_needed(std::move(l_decal), true);
		}
	}

	void bc_decal_component::add_decal(core::bc_string_view p_name,
	                                   const core::bc_vector3f& p_local_pos,
	                                   const core::bc_matrix3f& p_local_rotation,
	                                   bc_actor_render_group p_render_group,
	                                   const core::bc_matrix4f& p_initial_world_transform,
	                                   bc_mesh_node::node_index_t p_attached_node)
	{
		if (p_attached_node == bc_mesh_node::s_invalid_index)
		{
			add_decal(p_name, p_local_pos, p_local_rotation, p_render_group, p_initial_world_transform);
		}
		else
		{
			auto l_decal = m_decal_manager->create_decal(p_name, get_actor(), p_local_pos, p_local_rotation, p_render_group, p_attached_node);
			if (!l_decal)
			{
				return;
			}
				
			l_decal->set_world_transform(p_initial_world_transform);
			_add_decal_if_needed(std::move(l_decal), true);
		}
	}

	void bc_decal_component::initialize_entity(const bc_actor_component_initialize_entity_context& p_context)
	{
		m_decal_manager = &p_context.m_scene.get_decal_manager();
			
		auto* l_mesh_component = p_context.m_actor.get_component<bc_mesh_component>();
		if(l_mesh_component)
		{
			m_mesh_scale = l_mesh_component->get_mesh().get_mesh_scale();
			m_use_hierarchy_transforms = l_mesh_component->get_mesh().get_skinned();
		}
		else
		{
			m_mesh_scale = 1;
			m_use_hierarchy_transforms = false;
		}
	}

	void bc_decal_component::load_instance(const bc_actor_component_load_context& p_context)
	{
		const auto l_ite = p_context.m_parameters.find(constant::g_param_decal_parameters);
		if(l_ite == std::end(p_context.m_parameters))
		{
			return;
		}

		const auto& l_json_str = l_ite->second.as_throw<core::bc_string>();
		if(l_json_str.empty())
		{
			return;
		}

		core::bc_json_document<_bc_decal_array_json> l_json;
		l_json.load(l_json_str.c_str());

		for (auto& l_json_entry : l_json->m_decals)
		{
			add_decal
			(
				*l_json_entry->m_nm,
				*l_json_entry->m_pos_ls,
				bc_matrix3f_rotation_zyx(*l_json_entry->m_rot_ls),
				static_cast<bc_actor_render_group>(*l_json_entry->m_rg),
				core::bc_matrix4f::identity(),
				*l_json_entry->m_ani
			);
		}
	}

	void bc_decal_component::write_instance(const bc_actor_component_write_context& p_context)
	{
		if(m_persistent_decals.empty())
		{
			return;
		}

		core::bc_json_document<_bc_decal_array_json> l_json;
		l_json.set_max_decimal_places(2);
			
		for(const auto& l_decal : m_persistent_decals)
		{
			auto& l_json_entry = l_json->m_decals.new_entry();
			*l_json_entry->m_nm = l_decal->get_decal()->get_name();
			*l_json_entry->m_pos_ls = l_decal->get_local_position();
			*l_json_entry->m_rot_ls = bc_matrix3f_decompose_to_angles_zyx(l_decal->get_local_rotation());
			*l_json_entry->m_rg = static_cast<bcINT32>(l_decal->get_render_group());
			*l_json_entry->m_ani = l_decal->get_attached_node_index();
		}

		core::bc_string l_json_str = l_json.write();
		p_context.m_parameters.add_or_update(constant::g_param_decal_parameters, core::bc_any(std::move(l_json_str)));
	}

	void bc_decal_component::handle_event(const bc_actor_component_event_context& p_context)
	{
		const auto* l_world_transform_event = core::bci_message::as<bc_world_transform_actor_event>(p_context.m_event);
		if(l_world_transform_event && !m_use_hierarchy_transforms)
		{
			const auto l_update_transform = [&, this](const core::bc_span<bc_decal_instance_ptr>& p_decals)
			{
				for (auto& l_decal : p_decals)
				{
					if (!l_decal)
					{
						continue;
					}
						
					l_decal->set_world_transform(l_world_transform_event->get_transform());
				}
			};

			{
				platform::bc_lock_guard l_decals_lock(*this);
				l_update_transform(core::bc_make_span(m_temporary_decals));
			}

			l_update_transform(core::bc_make_span(m_persistent_decals));
				
			return;
		}

		const auto* l_hierarchy_event = core::bci_message::as<bc_hierarchy_transform_actor_event>(p_context.m_event);
		if(l_hierarchy_event && m_use_hierarchy_transforms)
		{
			const auto* l_mesh_component = p_context.m_actor.get_component<bc_mesh_component>();
			const auto& l_mesh = l_mesh_component->get_mesh();

			const auto l_update_transform = [&, this](const core::bc_span<bc_decal_instance_ptr>& p_decals)
			{
				for (auto& l_decal : p_decals)
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
			};

			{
				platform::bc_lock_guard l_decals_lock(*this);
				l_update_transform(core::bc_make_span(m_temporary_decals));
			}

			l_update_transform(core::bc_make_span(m_persistent_decals));
				
			return;
		}
			
		const auto* l_remove_event = core::bci_message::as<bc_remove_decal_actor_event>(p_context.m_event);
		if(l_remove_event)
		{
			{
				platform::bc_lock_guard l_decals_lock(*this);

				const auto l_temp_ite = std::find_if
				(
					std::begin(m_temporary_decals),
					std::end(m_temporary_decals),
					[&](const bc_decal_instance_ptr& p_decal)
					{
						return p_decal.get() == &l_remove_event->get_decal_instance();
					}
				);
				if (l_temp_ite != std::end(m_temporary_decals))
				{
					*l_temp_ite = nullptr;
					return;
				}
			}

			const auto l_persistent_ite = std::find_if
			(
				std::begin(m_persistent_decals),
				std::end(m_persistent_decals),
				[&](const bc_decal_instance_ptr& p_decal)
				{
					return p_decal.get() == &l_remove_event->get_decal_instance();
				}
			);
			if (l_persistent_ite != std::end(m_persistent_decals))
			{
				m_persistent_decals.erase(l_persistent_ite);
				return;
			}
		}
	}

	void bc_decal_component::render(const bc_actor_component_render_context& p_context) const
	{
		/*const auto& l_global_config = bc_get_global_config();
			
			for(const auto& l_decal_instance : m_temporary_decals)
			{
				if(!l_decal_instance)
				{
					continue;
				}
				
				auto* l_decal = l_decal_instance->get_decal();
				const auto l_lod_factor = std::max(l_decal->get_width(), l_decal->get_height()) * l_decal->get_view_distance() * l_global_config.get_lod_global_scale();
				const auto l_camera_distance = (l_decal_instance->get_world_transform().get_translation() - p_context.m_camera.m_main_camera.get_position()).magnitude();
				const auto l_culling_index = static_cast<bcUINT32>(l_camera_distance / l_lod_factor);
				
				if(l_culling_index <= l_global_config.get_lod_culling_index())
				{
					p_context.m_buffer.add_decal_instance
					(
						l_decal_instance->get_decal_ptr(),
						bc_render_instance(l_decal_instance->get_world_transform(), bc_render_group::unknown)
					);
				}
			}*/
	}

	void bc_decal_component::_add_decal_if_needed(bc_decal_instance_ptr p_decal, bool p_need_update) noexcept
	{
		const auto l_is_temporary = p_decal->get_decal()->get_auto_remove();

		if(!l_is_temporary)
		{
			m_persistent_decals.push_back(std::move(p_decal));
			return;
		}

		if(p_need_update)
		{
			{
				platform::bc_lock_guard l_decals_lock(*this);

				m_temporary_decals_slot = (m_temporary_decals_slot + 1) % m_temporary_decals.size();
				m_temporary_decals[m_temporary_decals_slot] = std::move(p_decal);
			}
		}
	}

	void bc_decal_component::lock()
	{
		while (m_decals_lock.test_and_set())
		{
		}
	}

	void bc_decal_component::unlock()
	{
		m_decals_lock.clear();
	}
}
