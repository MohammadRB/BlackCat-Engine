// [04/20/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Math/bcCoordinate.h"
#include "Core/bcUtility.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcRigidControllerComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcHierarchyTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcAddedToSceneActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcRemovedFromSceneActorEvent.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/bcGameSystem.h"
#include "Game/bcJsonParse.h"
#include "Game/bcConstant.h"

namespace black_cat
{
	namespace game
	{
		bc_rigid_controller_component::bc_rigid_controller_component(bc_actor_id p_actor_id, bc_actor_component_id p_id) noexcept
			: bci_actor_component(p_actor_id, p_id),
			bc_rigid_body_component()
		{
		}

		bc_rigid_controller_component::bc_rigid_controller_component(bc_rigid_controller_component&&) noexcept = default;

		bc_rigid_controller_component::~bc_rigid_controller_component()
		{
			if (m_px_body->is_valid())
			{
				{
					bc_rigid_component_lock l_lock(*this);

					core::bc_get_service<bc_game_system>()->get_physics_system().clear_px_shapes_data(*m_px_body);
					m_px_body.reset();
					m_px_controller.reset();
				}
			}
		}

		bc_rigid_controller_component& bc_rigid_controller_component::operator=(bc_rigid_controller_component&&) noexcept = default;

		bc_actor bc_rigid_controller_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_rigid_controller_component::initialize_entity(const bc_actor_component_initialize_entity_context& p_context)
		{
			const auto& l_material_manager = p_context.m_game_system.get_render_system().get_material_manager();
			auto& l_physics_system = p_context.m_game_system.get_physics_system();
			auto& l_physics = l_physics_system.get_physics();

			if (const auto* l_mesh_component = p_context.m_actor.get_component<bc_mesh_component>())
			{
				m_px_body = l_physics.create_rigid_dynamic(physics::bc_transform(p_context.m_transform));
				l_physics_system.set_game_actor(*m_px_body, p_context.m_actor);

				const auto* l_materials = static_cast<core::bc_json_key_value*>(nullptr);
				json_parse::bc_load(p_context.m_parameters, constant::g_param_mesh_collider_materials, l_materials);
				
				l_physics_system.create_px_shapes_from_mesh(l_material_manager, m_px_body.get(), l_mesh_component->get_mesh(), l_materials);

				core::bc_vector_frame<physics::bc_shape> l_rigid_shapes(m_px_body->get_shape_count());
				m_px_body->get_shapes(l_rigid_shapes.data(), l_rigid_shapes.size());

				for (auto& l_shape : l_rigid_shapes)
				{
					l_shape.set_flag(physics::bc_shape_flag::simulation, false);
				}

				set_kinematic(true);

				bcFLOAT l_mass_value = 1;
				core::bc_vector3f l_cmass_value;
				json_parse::bc_load(p_context.m_parameters, constant::g_param_rigid_mass, l_mass_value);
				const auto l_has_cmass = json_parse::bc_load(p_context.m_parameters, constant::g_param_rigid_cmass, l_cmass_value);
				
				m_px_body->set_mass_update_inertia(l_mass_value, l_has_cmass ? &l_cmass_value : nullptr);

				added_to_scene(p_context.m_scene.get_px_scene(), *m_px_body);

				bc_actor_mark_for_checkpoint(p_context.m_actor);

				return;
			}

			throw bc_invalid_operation_exception("Rigid controller component needs mesh component.");
		}

		void bc_rigid_controller_component::reset_controller(physics::bc_ccontroller_ref p_controller)
		{
			m_px_controller = std::move(p_controller);
		}

		void bc_rigid_controller_component::reset_controller()
		{
			{
				physics::bc_scene_lock l_lock(get_scene());
				m_px_controller.reset();
			}
		}

		void bc_rigid_controller_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			const auto* l_world_transform_event = core::bci_message::as<bc_world_transform_actor_event>(p_context.m_event);
			if (l_world_transform_event)
			{
				const auto& l_transform = l_world_transform_event->get_transform();
				
				{
					physics::bc_scene_lock l_lock(get_scene());
					m_px_body->set_global_pose(physics::bc_transform(l_transform));
				}

				return;
			}

			const auto* l_hierarchy_transform_event = core::bci_message::as<bc_hierarchy_transform_actor_event>(p_context.m_event);
			if (l_hierarchy_transform_event && l_hierarchy_transform_event->get_px_transforms())
			{
				update_px_shape_transforms(*m_px_body, *l_hierarchy_transform_event->get_px_transforms());
				return;
			}

			/*const auto* l_scene_add_event = core::bci_message::as<bc_added_to_scene_actor_event>(p_context.m_event);
			if (l_scene_add_event)
			{
				added_to_scene(l_scene_add_event->get_scene().get_px_scene(), *m_px_body);
				return;
			}*/

			const auto* l_scene_remove_event = core::bci_message::as<bc_removed_from_scene_actor_event>(p_context.m_event);
			if (l_scene_remove_event)
			{
				remove_from_scene(l_scene_remove_event->get_scene().get_px_scene(), *m_px_body);
				return;
			}
		}

		void bc_rigid_controller_component::debug_draw(const bc_actor_component_debug_draw_context& p_context)
		{
			bc_rigid_body_component::debug_draw(*m_px_body, p_context);
		}
	}	
}