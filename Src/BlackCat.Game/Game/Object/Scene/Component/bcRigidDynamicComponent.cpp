// [01/07/2017 MRB]

#include "Game/GamePCH.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcRigidDynamicComponent.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/Event/bcActorEventWorldTransform.h"
#include "Game/Object/Scene/Component/Event/bcActorEventHierarchyTransform.h"

namespace black_cat
{
	namespace game
	{
		bc_rigid_dynamic_component::bc_rigid_dynamic_component(bc_actor_index p_actor_index, bc_actor_component_index p_index) noexcept
			: bc_rigid_body_component(p_actor_index, p_index)
		{
		}

		bc_rigid_dynamic_component::bc_rigid_dynamic_component(bc_rigid_dynamic_component&& p_other) noexcept
			: bc_rigid_body_component(std::move(p_other)),
			m_px_actor_ref(std::move(p_other.m_px_actor_ref))
		{
		}

		bc_rigid_dynamic_component::~bc_rigid_dynamic_component()
		{
		}

		bc_rigid_dynamic_component& bc_rigid_dynamic_component::operator=(bc_rigid_dynamic_component&& p_other) noexcept
		{
			bc_rigid_body_component::operator=(std::move(p_other));
			m_px_actor_ref = std::move(p_other.m_px_actor_ref);

			return *this;
		}

		bc_actor bc_rigid_dynamic_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		physics::bc_rigid_body& bc_rigid_dynamic_component::get_body() noexcept
		{
			return m_px_actor_ref.get();
		}

		physics::bc_rigid_dynamic bc_rigid_dynamic_component::get_dynamic_body() const noexcept
		{
			return m_px_actor_ref.get();
		}

		void bc_rigid_dynamic_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
			auto* l_mesh_component = p_context.m_actor.get_component<bc_mesh_component>();

			if (l_mesh_component)
			{
				auto& l_physics_system = core::bc_get_service< bc_game_system >()->get_physics_system();
				auto& l_physics = l_physics_system.get_physics();

				m_px_actor_ref = l_physics.create_rigid_dynamic(physics::bc_transform::identity());
				l_physics_system.set_game_actor(*m_px_actor_ref, p_context.m_actor);
				
				l_physics_system.create_px_shapes_from_mesh(m_px_actor_ref.get(), p_context.m_actor, *l_mesh_component);

				return;
			}

			throw bc_invalid_operation_exception("Rigid dynamic component needs mesh component.");
		}

		void bc_rigid_dynamic_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			const auto* l_world_transform_event = core::bci_message::as< bc_actor_event_world_transform >(p_context.m_event);
			if(l_world_transform_event && !l_world_transform_event->is_px_simulation_transform())
			{
				m_px_actor_ref->set_global_pose(physics::bc_transform(l_world_transform_event->get_transform()));
				return;
			}

			const auto* l_hierarchy_transform_event = core::bci_message::as< bc_actor_event_hierarchy_transform >(p_context.m_event);
			if (l_hierarchy_transform_event && l_hierarchy_transform_event->get_px_transforms())
			{
				update_px_shape_transforms(*m_px_actor_ref, *l_hierarchy_transform_event->get_px_transforms());
				return;
			}
		}

		void bc_rigid_dynamic_component::debug_draw(const bc_actor_component_debug_draw_context& p_context)
		{
			bc_rigid_body_component::debug_draw(m_px_actor_ref.get(), p_context);
		}
	}
}