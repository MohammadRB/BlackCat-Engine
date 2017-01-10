// [01/07/2017 MRB]

#include "Game/GamePCH.h"

#include "PhysicsImp/Shape/bcShapeHeightField.h"
#include "Game/bcExport.h"
#include "Game/bcException.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Scene/Component/bcRigidStaticComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"

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
			m_px_actor(std::move(p_other.m_px_actor))
		{
		}

		bc_rigid_static_component::~bc_rigid_static_component()
		{
		}

		bc_rigid_static_component& bc_rigid_static_component::operator=(bc_rigid_static_component&& p_other) noexcept
		{
			bc_rigid_body_component::operator=(std::move(p_other));
			m_px_actor = std::move(p_other.m_px_actor);

			return *this;
		}

		physics::bc_rigid_body& bc_rigid_static_component::get_body() noexcept
		{
			return m_px_actor.get();
		}

		physics::bc_rigid_static bc_rigid_static_component::get_static_body() const noexcept
		{
			return m_px_actor.get();
		}

		void bc_rigid_static_component::initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters)
		{
			auto* l_mesh_component = p_actor.get_component<bc_mesh_component>();

			if(l_mesh_component)
			{
				// TODO do whatever related to mesh

				return;
			}

			auto* l_height_map_component = p_actor.get_component<bc_height_map_component>();

			if(l_height_map_component)
			{
				auto* l_height_map = l_height_map_component->get_height_map();
				auto l_px_height_field = l_height_map->get_height_field();

				auto& l_physics_system = core::bc_get_service< bc_game_system >()->get_physics_system();
				auto& l_physics = l_physics_system.get_physics();

				m_px_actor = l_physics.create_rigid_static(physics::bc_transform::identity());
				auto l_height_field_shape = physics::bc_shape_height_field(l_px_height_field, 1, l_physics_system.get_height_field_y_scale());
				auto l_height_field_material = l_physics.create_material(1, 1, 1);

				m_px_actor->create_shape(l_height_field_shape, l_height_field_material.get());
				// TODO check for material reference count

				return;
			}

			throw bc_invalid_operation_exception("Rigid static component need mesh or height map component.");
		}

		void bc_rigid_static_component::update(const bc_actor& p_actor, core_platform::bc_clock::update_param p_clock_update_param)
		{
		}
	}
}