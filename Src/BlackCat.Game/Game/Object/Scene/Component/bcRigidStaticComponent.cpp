// [07/01/2017 MRB]

#include "Game/GamePCH.h"

#include "Core/Utility/bcJsonParse.h"
#include "Core/bcUtility.h"
#include "PhysicsImp/Fundation/bcScene.h"
#include "PhysicsImp/Shape/bcShapeHeightField.h"
#include "Game/Object/Scene/Component/bcRigidStaticComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcHierarchyTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcAddedToSceneActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcRemovedFromSceneActorEvent.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/System/Physics/bcPxWrap.h"
#include "Game/System/Physics/bcPhysicsShapeUtility.h"
#include "Game/System/Physics/bcPhysicsSystem.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/bcGameSystem.h"
#include "Game/bcJsonParse.h"
#include "Game/bcConstant.h"
#include "Game/bcException.h"

namespace black_cat::game
{
	bc_rigid_static_component::bc_rigid_static_component(bc_actor_id p_actor_id, bc_actor_component_id p_id) noexcept
		: bci_actor_component(p_actor_id, p_id),
		  bc_rigid_body_component()
	{
	}

	bc_rigid_static_component::bc_rigid_static_component(bc_rigid_static_component&& p_other) noexcept
		: bci_actor_component(std::move(p_other)),
		  bc_rigid_body_component(std::move(p_other)),
		  m_px_actor_ref(std::move(p_other.m_px_actor_ref))
	{
	}

	bc_rigid_static_component::~bc_rigid_static_component()
	{
		if(m_px_actor_ref->is_valid())
		{
			{
				bc_rigid_component_lock l_lock(*this);

				core::bc_get_service<bc_game_system>()->get_physics_system().clear_px_shapes_data(*m_px_actor_ref);
				m_px_actor_ref.reset();
			}
		}
	}

	bc_rigid_static_component& bc_rigid_static_component::operator=(bc_rigid_static_component&& p_other) noexcept
	{
		bci_actor_component::operator=(std::move(p_other));
		bc_rigid_body_component::operator=(std::move(p_other));
		m_px_actor_ref = std::move(p_other.m_px_actor_ref);

		return *this;
	}

	void bc_rigid_static_component::initialize_entity(const bc_actor_component_initialize_entity_context& p_context)
	{
		const auto& l_material_manager = p_context.m_game_system.get_render_system().get_material_manager();
		auto& l_physics_system = p_context.m_game_system.get_physics_system();
		auto& l_physics = l_physics_system.get_physics();

		if (const auto* l_mesh_component = p_context.m_actor.get_component<bc_mesh_component>())
		{
			m_px_actor_ref = l_physics.create_rigid_static(physics::bc_transform(p_context.m_transform));
			l_physics_system.set_game_actor(*m_px_actor_ref, p_context.m_actor);

			const auto* l_materials = static_cast<core::bc_json_key_value*>(nullptr);
			json_parse::bc_load(p_context.m_parameters, constant::g_param_mesh_collider_materials, l_materials);

			l_physics_system.create_px_shapes_from_mesh(l_material_manager, m_px_actor_ref.get(), l_mesh_component->get_mesh(), l_materials);

			added_to_scene(p_context.m_scene.get_px_scene(), m_px_actor_ref.get());
				
			return;
		}

		if (const auto* l_height_map_component = p_context.m_actor.get_component<bc_height_map_component>())
		{
			m_px_actor_ref = l_physics.create_rigid_static(physics::bc_transform(p_context.m_transform));
			l_physics_system.set_game_actor(*m_px_actor_ref, p_context.m_actor);
			l_physics_system.create_px_shapes_from_height_map(l_material_manager, m_px_actor_ref.get(), l_height_map_component->get_height_map());

			added_to_scene(p_context.m_scene.get_px_scene(), m_px_actor_ref.get());
				
			return;
		}

		throw bc_invalid_operation_exception("Rigid static component needs either mesh or height map component.");
	}

	void bc_rigid_static_component::handle_event(const bc_actor_component_event_context& p_context)
	{
		const auto* l_world_transform_event = core::bci_message::as<bc_world_transform_actor_event>(p_context.m_event);
		if (l_world_transform_event)
		{
			const auto& l_transform = l_world_transform_event->get_transform();
			auto l_px_transform = physics::bc_transform(l_transform);
				
			auto* l_height_map_component = p_context.m_actor.get_component<bc_height_map_component>();
			if (l_height_map_component)
			{
				l_px_transform = bc_convert_to_height_map_transform(l_height_map_component->get_height_map(), l_px_transform);
			}

			{
				physics::bc_scene_lock l_lock(get_scene());
				m_px_actor_ref->set_global_pose(l_px_transform);
			}

			return;
		}

		const auto* l_hierarchy_transform_event = core::bci_message::as<bc_hierarchy_transform_actor_event>(p_context.m_event);
		if(l_hierarchy_transform_event && l_hierarchy_transform_event->get_px_transforms())
		{
			update_px_shape_transforms(*m_px_actor_ref, *l_hierarchy_transform_event->get_px_transforms());
			return;
		}

		/*const auto* l_scene_add_event = core::bci_message::as<bc_added_to_scene_actor_event>(p_context.m_event);
			if(l_scene_add_event)
			{
				added_to_scene(l_scene_add_event->get_scene().get_px_scene(), m_px_actor_ref.get());
				return;
			}*/

		const auto* l_scene_remove_event = core::bci_message::as<bc_removed_from_scene_actor_event>(p_context.m_event);
		if(l_scene_remove_event)
		{
			remove_from_scene(l_scene_remove_event->get_scene().get_px_scene(), m_px_actor_ref.get());
			return;
		}
	}

	void bc_rigid_static_component::debug_draw(const bc_actor_component_debug_draw_context& p_context)
	{
		bc_rigid_body_component::debug_draw(m_px_actor_ref.get(), p_context);
	}
}
