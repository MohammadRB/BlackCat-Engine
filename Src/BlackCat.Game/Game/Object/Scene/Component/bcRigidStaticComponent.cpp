// [01/07/2017 MRB]

#include "Game/GamePCH.h"

#include "PhysicsImp/Shape/bcShapeHeightField.h"
#include "Game/bcException.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Physics/bcPxWrap.h"
#include "Game/Object/Scene/Component/bcRigidStaticComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"
#include "Game/Object/Scene/Component/Event/bcActorEventWorldTransform.h"
#include "Game/Object/Scene/Component/Event/bcActorEventHierarchyTransform.h"

namespace black_cat
{
	namespace game
	{
		bc_rigid_static_component::bc_rigid_static_component(bc_actor_component_index p_index) noexcept
			: bc_rigid_body_component(p_index)
		{
		}

		bc_rigid_static_component::bc_rigid_static_component(bc_rigid_static_component&& p_other) noexcept
			: bc_rigid_body_component(std::move(p_other)),
			m_px_actor_ref(std::move(p_other.m_px_actor_ref))
		{
		}

		bc_rigid_static_component::~bc_rigid_static_component()
		{
		}

		bc_rigid_static_component& bc_rigid_static_component::operator=(bc_rigid_static_component&& p_other) noexcept
		{
			m_px_actor_ref = std::move(p_other.m_px_actor_ref);
			bc_rigid_body_component::operator=(std::move(p_other));

			return *this;
		}

		bc_actor bc_rigid_static_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		physics::bc_rigid_body& bc_rigid_static_component::get_body() noexcept
		{
			return m_px_actor_ref.get();
		}

		physics::bc_rigid_static bc_rigid_static_component::get_static_body() const noexcept
		{
			return m_px_actor_ref.get();
		}

		void bc_rigid_static_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
			auto& l_physics_system = core::bc_get_service<bc_game_system>()->get_physics_system();
			auto& l_physics = l_physics_system.get_physics();

			auto* l_mesh_component = p_context.m_actor.get_component<bc_mesh_component>();
			if(l_mesh_component)
			{
				m_px_actor_ref = l_physics.create_rigid_static(physics::bc_transform::identity());
				l_physics_system.connect_px_actor_to_game_actor(*m_px_actor_ref, p_context.m_actor);

				create_px_shapes_from_mesh(l_physics_system, m_px_actor_ref.get(), l_mesh_component->get_mesh());

				return;
			}

			auto* l_height_map_component = p_context.m_actor.get_component<bc_height_map_component>();
			if(l_height_map_component)
			{
				m_px_actor_ref = l_physics.create_rigid_static(physics::bc_transform::identity());
				l_physics_system.connect_px_actor_to_game_actor(*m_px_actor_ref, p_context.m_actor);

				create_px_shapes_from_height_map(l_physics_system, m_px_actor_ref.get(), l_height_map_component->get_height_map());

				return;
			}

			throw bc_invalid_operation_exception("Rigid static component needs either mesh or height map component.");
		}

		void bc_rigid_static_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			const auto* l_world_transform_event = core::bci_message::as< bc_actor_event_world_transform >(p_context.m_event);
			if (l_world_transform_event)
			{
				const auto& l_transform = l_world_transform_event->get_transform();
				physics::bc_transform l_px_transform;
				
				auto* l_height_map_component = p_context.m_actor.get_component<bc_height_map_component>();
				if (l_height_map_component) // TODO
				{
					const auto& l_height_map = l_height_map_component->get_height_map();
					const auto l_half_width = (l_height_map.get_width() * l_height_map.get_xz_multiplier()) / 2;
					const auto l_half_height = (l_height_map.get_height() * l_height_map.get_xz_multiplier()) / 2;
					const auto l_position = l_transform.get_translation() + core::bc_vector3f(-l_half_width, 0, l_half_height);

					core::bc_matrix4f l_new_transform = l_transform;
					l_new_transform.set_translation(l_position);

					l_px_transform = physics::bc_transform(l_new_transform);
				}
				else
				{
					l_px_transform = physics::bc_transform(l_transform);
				}
				
				m_px_actor_ref->set_global_pose(l_px_transform);

				return;
			}

			const auto* l_hierarchy_transform_event = core::bci_message::as< bc_actor_event_hierarchy_transform >(p_context.m_event);
			if(l_hierarchy_transform_event && l_hierarchy_transform_event->get_px_transforms())
			{
				update_px_shape_transforms(*m_px_actor_ref, *l_hierarchy_transform_event->get_px_transforms());
				return;
			}
		}
		
		void bc_rigid_static_component::create_px_shapes_from_height_map(bc_physics_system& p_physics_system,
			physics::bc_rigid_static& p_rigid_static,
			const bc_height_map& p_height_map)
		{
			auto& l_physics = p_physics_system.get_physics();
			const auto l_px_height_field = p_height_map.get_px_height_field();

			const auto l_height_field_shape = physics::bc_shape_height_field(l_px_height_field, p_height_map.get_xz_multiplier(), p_physics_system.get_height_field_y_scale());
			auto l_height_field_material = l_physics.create_material(1, 1, 0.1f);

			p_rigid_static.create_shape(l_height_field_shape, l_height_field_material.get());
			p_rigid_static.set_query_group(static_cast<physics::bc_query_group>(bc_query_group::terrain));
		}
	}
}