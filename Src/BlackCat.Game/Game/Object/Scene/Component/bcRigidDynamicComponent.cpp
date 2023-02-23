// [01/07/2017 MRB]

#include "Game/GamePCH.h"

#include "Core/Utility/bcJsonParse.h"
#include "Core/bcUtility.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManagerContainer.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcRigidDynamicComponent.h"
#include "Game/Object/Scene/Component/bcCheckPointComponent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcHierarchyTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcAddedToSceneActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcRemovedFromSceneActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcNetworkReplicateActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcBulletHitActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcExplosionActorEvent.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Physics/bcPhysicsSystem.h"
#include "Game/System/bcGameSystem.h"
#include "Game/bcJsonParse.h"
#include "Game/bcConstant.h"

namespace black_cat
{
	namespace game
	{
		bc_rigid_dynamic_component::bc_rigid_dynamic_component(bc_actor_id p_actor_id, bc_actor_component_id p_id) noexcept
			: bci_actor_component(p_actor_id, p_id),
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
			if (m_px_actor_ref->is_valid())
			{
				{
					bc_rigid_component_lock l_lock(*this);

					core::bc_get_service<bc_game_system>()->get_physics_system().clear_px_shapes_data(*m_px_actor_ref);
					m_px_actor_ref.reset();
				}
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

		void bc_rigid_dynamic_component::initialize_entity(const bc_actor_component_initialize_entity_context& p_context)
		{
			if (const auto* l_mesh_component = p_context.m_actor.get_component<bc_mesh_component>())
			{
				const auto& l_material_manager = p_context.m_game_system.get_render_system().get_material_manager();
				auto& l_physics_system = core::bc_get_service<bc_game_system>()->get_physics_system();
				auto& l_physics = l_physics_system.get_physics();

				m_px_actor_ref = l_physics.create_rigid_dynamic(physics::bc_transform(p_context.m_transform));
				l_physics_system.set_game_actor(*m_px_actor_ref, p_context.m_actor);

				const auto* l_materials = static_cast<core::bc_json_key_value*>(nullptr);
				json_parse::bc_load(p_context.m_parameters, constant::g_param_mesh_collider_materials, l_materials);

				l_physics_system.create_px_shapes_from_mesh(l_material_manager, m_px_actor_ref.get(), l_mesh_component->get_mesh(), l_materials);

				bcFLOAT l_mass_value = 1;
				core::bc_vector3f l_cmass_value;
				json_parse::bc_load(p_context.m_parameters, constant::g_param_rigid_mass, l_mass_value);
				const auto l_has_cmass = json_parse::bc_load(p_context.m_parameters, constant::g_param_rigid_cmass, l_cmass_value);

				m_px_actor_ref->set_mass_update_inertia(l_mass_value, l_has_cmass ? &l_cmass_value : nullptr);
				
				added_to_scene(p_context.m_scene.get_px_scene(), m_px_actor_ref.get());

				bc_actor_mark_for_checkpoint(p_context.m_actor);

				return;
			}

			throw bc_invalid_operation_exception("Rigid dynamic component needs mesh component.");
		}

		void bc_rigid_dynamic_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			if (const auto* l_world_transform_event = core::bci_message::as<bc_world_transform_actor_event>(p_context.m_event))
			{
				const auto l_transform_type = l_world_transform_event->get_transform_type();
				if (l_transform_type == bc_transform_event_type::physics)
				{
					return;
				}
				
				{
					bc_rigid_component_lock l_lock(*this);

					if (l_transform_type == bc_transform_event_type::teleport)
					{
						m_px_actor_ref->set_global_pose(physics::bc_transform(l_world_transform_event->get_transform()));
					}
					else if (l_transform_type == bc_transform_event_type::network)
					{
						const auto l_is_kinematic = core::bc_enum::has(m_px_actor_ref->get_rigid_body_flags(), physics::bc_rigid_body_flag::kinematic);
						if (l_is_kinematic)
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

			if (const auto* l_bullet_hit_event = core::bci_message::as<bc_bullet_hit_actor_event>(p_context.m_event))
			{
				const auto l_force = l_bullet_hit_event->calculate_applied_force();
				
				{
					bc_rigid_component_lock l_lock(*this);

					const auto l_is_kinematic = core::bc_enum::has(m_px_actor_ref->get_rigid_body_flags(), physics::bc_rigid_body_flag::kinematic);
					if(!l_is_kinematic)
					{
						m_px_actor_ref->add_force_at_pose(l_bullet_hit_event->get_bullet_direction() * l_force, l_bullet_hit_event->get_hit_position());
					}
				}
			}

			if (const auto* l_explosion_hit_event = core::bci_message::as<bc_explosion_actor_event>(p_context.m_event))
			{
				{
					bc_rigid_component_lock l_lock(*this);

					const auto l_is_kinematic = core::bc_enum::has(m_px_actor_ref->get_rigid_body_flags(), physics::bc_rigid_body_flag::kinematic);
					if (!l_is_kinematic)
					{
						const auto l_force = l_explosion_hit_event->calculate_applied_force(m_px_actor_ref->get_global_pose().get_position());
						m_px_actor_ref->add_force(l_force.first * l_force.second);
					}
				}
			}
			
			/*const auto* l_scene_add_event = core::bci_message::as<bc_added_to_scene_actor_event>(p_context.m_event);
			if (l_scene_add_event)
			{
				added_to_scene(l_scene_add_event->get_scene().get_px_scene(), m_px_actor_ref.get());
				return;
			}*/

			if (const auto* l_scene_remove_event = core::bci_message::as<bc_removed_from_scene_actor_event>(p_context.m_event))
			{
				remove_from_scene(l_scene_remove_event->get_scene().get_px_scene(), m_px_actor_ref.get());
				return;
			}

			if (const auto* l_network_replicate_event = core::bci_message::as<bc_network_replicate_actor_event>(p_context.m_event))
			{
				const auto l_replication_side = l_network_replicate_event->get_replication_side();

				if (l_replication_side == bc_actor_replication_side::origin)
				{
					{
						bc_rigid_component_lock l_lock(*this);
						m_px_actor_ref->set_actor_flag(physics::bc_actor_flag::send_sleep_wake, true);
					}
				}
				else
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