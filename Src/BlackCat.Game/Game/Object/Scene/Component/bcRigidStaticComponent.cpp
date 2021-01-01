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
		
		physics::bc_rigid_body bc_rigid_static_component::get_body() noexcept
		{
			return m_px_actor_ref.get();
		}

		physics::bc_rigid_static bc_rigid_static_component::get_static_body() const noexcept
		{
			return m_px_actor_ref.get();
		}

		void bc_rigid_static_component::initialize(bc_actor_component_initialize_context& p_context)
		{
			auto& l_physics_system = core::bc_get_service<bc_game_system>()->get_physics_system();
			auto& l_physics = l_physics_system.get_physics();

			auto* l_mesh_component = p_context.m_actor.get_component<bc_mesh_component>();
			if(l_mesh_component)
			{
				m_px_actor_ref = l_physics.create_rigid_static(physics::bc_transform::identity());
				initialize_from_mesh(l_physics_system, p_context.m_actor, m_px_actor_ref.get(), *l_mesh_component);

				return;
			}

			auto* l_height_map_component = p_context.m_actor.get_component<bc_height_map_component>();
			if(l_height_map_component)
			{
				m_px_actor_ref = l_physics.create_rigid_static(physics::bc_transform::identity());
				_initialize_from_height_map(l_physics_system, p_context.m_actor, m_px_actor_ref.get(), *l_height_map_component);

				return;
			}

			throw bc_invalid_operation_exception("Rigid static component needs either mesh or height map component.");
		}

		void bc_rigid_static_component::handle_event(bc_actor& p_actor, const bc_actor_event& p_event)
		{
			auto* l_world_transform_event = core::bc_imessage::as< bc_actor_event_world_transform >(p_event);
			if (l_world_transform_event)
			{
				auto& l_transform = l_world_transform_event->get_transform();
				physics::bc_transform l_px_transform;
				
				auto* l_height_map_component = p_actor.get_component<bc_height_map_component>();
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
			}
		}
		
		void bc_rigid_static_component::_initialize_from_height_map(bc_physics_system& p_physics_system, 
			bc_actor& p_actor, 
			physics::bc_rigid_static& p_rigid_static,
			bc_height_map_component& p_component)
		{
			auto& l_physics = p_physics_system.get_physics();
			auto& l_height_map = p_component.get_height_map();
			auto l_px_height_field = l_height_map.get_px_height_field();

			auto l_height_field_shape = physics::bc_shape_height_field(l_px_height_field, l_height_map.get_xz_multiplier(), p_physics_system.get_height_field_y_scale());
			auto l_height_field_material = l_physics.create_material(1, 1, 0.1);

			p_rigid_static.create_shape(l_height_field_shape, l_height_field_material.get());
			p_rigid_static.set_query_group(static_cast<physics::bc_query_group>(bc_query_group::terrain));

			p_physics_system.connect_px_actor_to_game_actor(p_rigid_static, p_actor);
		}
	}
}