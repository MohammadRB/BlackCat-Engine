// [01/07/2017 MRB]

#include "Game/GamePCH.h"

#include "PhysicsImp/Shape/bcShapeHeightField.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Physics/bcPxWrap.h"
#include "Game/bcConstant.h"
#include "Game/bcException.h"
#include "Game/Object/Scene/Component/bcRigidStaticComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcHierarchyTransformActorEvent.h"

namespace black_cat
{
	namespace game
	{
		bc_rigid_static_component::bc_rigid_static_component(bc_actor_index p_actor_index, bc_actor_component_index p_index) noexcept
			: bci_actor_component(p_actor_index, p_index),
			bc_rigid_body_component(p_actor_index, p_index)
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
			auto& l_body = get_body();
			if(l_body.is_valid())
			{
				core::bc_get_service<bc_game_system>()->get_physics_system().clear_px_shapes_data(l_body);
			}
		}

		bc_rigid_static_component& bc_rigid_static_component::operator=(bc_rigid_static_component&& p_other) noexcept
		{
			bci_actor_component::operator=(std::move(p_other));
			bc_rigid_body_component::operator=(std::move(p_other));
			m_px_actor_ref = std::move(p_other.m_px_actor_ref);

			return *this;
		}

		bc_actor bc_rigid_static_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_rigid_static_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
			auto& l_material_manager = p_context.m_game_system.get_render_system().get_material_manager();
			auto& l_physics_system = p_context.m_game_system.get_physics_system();
			auto& l_physics = l_physics_system.get_physics();

			auto* l_mesh_component = p_context.m_actor.get_component<bc_mesh_component>();
			if(l_mesh_component)
			{
				m_px_actor_ref = l_physics.create_rigid_static(physics::bc_transform::identity());
				l_physics_system.set_game_actor(*m_px_actor_ref, p_context.m_actor);

				const auto* l_materials = p_context.m_parameters.get_value<core::bc_json_key_value>(constant::g_param_mesh_collider_materials);
				l_physics_system.create_px_shapes_from_mesh(l_material_manager, m_px_actor_ref.get(), *l_mesh_component, l_materials);

				return;
			}

			auto* l_height_map_component = p_context.m_actor.get_component<bc_height_map_component>();
			if(l_height_map_component)
			{
				m_px_actor_ref = l_physics.create_rigid_static(physics::bc_transform::identity());
				l_physics_system.set_game_actor(*m_px_actor_ref, p_context.m_actor);

				l_physics_system.create_px_shapes_from_height_map(l_material_manager, m_px_actor_ref.get(), *l_height_map_component);

				return;
			}

			throw bc_invalid_operation_exception("Rigid static component needs either mesh or height map component.");
		}

		void bc_rigid_static_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			const auto* l_world_transform_event = core::bci_message::as< bc_world_transform_actor_event >(p_context.m_event);
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

			const auto* l_hierarchy_transform_event = core::bci_message::as< bc_hierarchy_transform_actor_event >(p_context.m_event);
			if(l_hierarchy_transform_event && l_hierarchy_transform_event->get_px_transforms())
			{
				update_px_shape_transforms(*m_px_actor_ref, *l_hierarchy_transform_event->get_px_transforms());
				return;
			}
		}

		void bc_rigid_static_component::debug_draw(const bc_actor_component_debug_draw_context& p_context)
		{
			bc_rigid_body_component::debug_draw(m_px_actor_ref.get(), p_context);
		}
	}
}