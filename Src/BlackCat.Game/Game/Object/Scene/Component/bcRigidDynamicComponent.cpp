// [01/07/2017 MRB]

#include "Game/GamePCH.h"

#include "Core/bcUtility.h"
#include "Core/Utility/bcJsonParse.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcRigidDynamicComponent.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcHierarchyTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcAddedToSceneActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcRemovedFromSceneActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcNetworkReplicateActorEvent.h"
#include "Game/bcConstant.h"

namespace black_cat
{
	namespace game
	{
		bc_rigid_dynamic_component::bc_rigid_dynamic_component(bc_actor_id p_actor_index, bc_actor_component_id p_index) noexcept
			: bci_actor_component(p_actor_index, p_index),
			bc_rigid_body_component()
		{
		}

		bc_rigid_dynamic_component::bc_rigid_dynamic_component(bc_rigid_dynamic_component&& p_other) noexcept
			: bci_actor_component(std::move(p_other)),
			bc_rigid_body_component(std::move(p_other)),
			m_px_actor_ref(std::move(p_other.m_px_actor_ref))
		{
		}

		bc_rigid_dynamic_component::~bc_rigid_dynamic_component()
		{
			auto& l_body = get_body();
			if (l_body.is_valid())
			{
				core::bc_get_service<bc_game_system>()->get_physics_system().clear_px_shapes_data(l_body);
			}
		}

		bc_rigid_dynamic_component& bc_rigid_dynamic_component::operator=(bc_rigid_dynamic_component&& p_other) noexcept
		{
			bci_actor_component::operator=(std::move(p_other));
			bc_rigid_body_component::operator=(std::move(p_other));
			m_px_actor_ref = std::move(p_other.m_px_actor_ref);

			return *this;
		}

		bc_actor bc_rigid_dynamic_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_rigid_dynamic_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
		}

		void bc_rigid_dynamic_component::initialize_entity(const bc_actor_component_initialize_entity_context& p_context)
		{
			auto* l_mesh_component = p_context.m_actor.get_component<bc_mesh_component>();

			if (l_mesh_component)
			{
				auto& l_material_manager = p_context.m_game_system.get_render_system().get_material_manager();
				auto& l_physics_system = core::bc_get_service<bc_game_system>()->get_physics_system();
				auto& l_physics = l_physics_system.get_physics();

				m_px_actor_ref = l_physics.create_rigid_dynamic(physics::bc_transform::identity());
				l_physics_system.set_game_actor(*m_px_actor_ref, p_context.m_actor);

				const auto* l_materials = p_context.m_parameters.get_value<core::bc_json_key_value>(constant::g_param_mesh_collider_materials);
				l_physics_system.create_px_shapes_from_mesh(l_material_manager, m_px_actor_ref.get(), *l_mesh_component, l_materials);

				const auto l_mass_value = bc_null_default(p_context.m_parameters.get_value<bcFLOAT>(constant::g_param_rigid_mass), 1);
				const auto l_cmass_value = p_context.m_parameters.get_value_vector3f(constant::g_param_rigid_cmass);
				m_px_actor_ref->update_mass_inertia(l_mass_value, l_cmass_value.get());
				
				added_to_scene(p_context.m_scene.get_px_scene(), m_px_actor_ref.get());
				
				return;
			}

			throw bc_invalid_operation_exception("Rigid dynamic component needs mesh component.");
		}

		void bc_rigid_dynamic_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			const auto* l_world_transform_event = core::bci_message::as<bc_world_transform_actor_event>(p_context.m_event);
			if(l_world_transform_event)
			{
				const auto l_transform_type = l_world_transform_event->get_transform_type();
				if(l_transform_type == bc_transform_event_type::physics)
				{
					return;
				}
				
				{
					physics::bc_scene_lock l_lock(get_scene());

					if(l_transform_type == bc_transform_event_type::teleport)
					{
						m_px_actor_ref->set_global_pose(physics::bc_transform(l_world_transform_event->get_transform()));
					}
					else if(l_transform_type == bc_transform_event_type::network)
					{
						const auto l_is_kinematic = core::bc_enum::has(m_px_actor_ref->get_rigid_body_flags(), physics::bc_rigid_body_flag::kinematic);
						if(l_is_kinematic)
						{
							m_px_actor_ref->set_kinematic_target(physics::bc_transform(l_world_transform_event->get_transform()));
						}
						else
						{
							m_px_actor_ref->set_global_pose(physics::bc_transform(l_world_transform_event->get_transform()));
						}
					}
				}
				return;
			}

			const auto* l_hierarchy_transform_event = core::bci_message::as<bc_hierarchy_transform_actor_event>(p_context.m_event);
			if (l_hierarchy_transform_event && l_hierarchy_transform_event->get_px_transforms())
			{
				update_px_shape_transforms(*m_px_actor_ref, *l_hierarchy_transform_event->get_px_transforms());
				return;
			}

			/*const auto* l_scene_add_event = core::bci_message::as<bc_added_to_scene_actor_event>(p_context.m_event);
			if (l_scene_add_event)
			{
				added_to_scene(l_scene_add_event->get_scene().get_px_scene(), m_px_actor_ref.get());
				return;
			}*/

			const auto* l_scene_remove_event = core::bci_message::as<bc_removed_from_scene_actor_event>(p_context.m_event);
			if (l_scene_remove_event)
			{
				remove_from_scene(l_scene_remove_event->get_scene().get_px_scene(), m_px_actor_ref.get());
				return;
			}

			const auto* l_network_replicate_event = core::bci_message::as<bc_network_replicate_actor_event>(p_context.m_event);
			if(l_network_replicate_event)
			{
				const auto l_network_type = p_context.m_game_system.get_network_system().get_network_type();
				const auto l_data_dir = l_network_replicate_event->get_data_dir();

				if
				(
					(l_network_type == bc_network_type::server && l_data_dir == bc_actor_network_data_dir::replicate_sync_from_client) ||
					(l_network_type == bc_network_type::client && l_data_dir == bc_actor_network_data_dir::replicate_sync)
				)
				{
					set_kinematic(true);
				}
			}
		}

		void bc_rigid_dynamic_component::debug_draw(const bc_actor_component_debug_draw_context& p_context)
		{
			bc_rigid_body_component::debug_draw(m_px_actor_ref.get(), p_context);
		}
	}
}