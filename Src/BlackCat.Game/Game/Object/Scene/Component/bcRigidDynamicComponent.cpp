// [01/07/2017 MRB]

#include "Game/GamePCH.h"

#include "Core/Container/bcVector.h"
#include "Game/bcExport.h"
#include "Game/Object/Scene/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcRigidDynamicComponent.h"
#include "Game/System/bcGameSystem.h"
#include "PlatformImp/bc_ide_logger.h"

namespace black_cat
{
	namespace game
	{
		bc_rigid_dynamic_component::bc_rigid_dynamic_component(bc_actor_component_index p_index) noexcept
			: bc_rigid_body_component(p_index)
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

		physics::bc_rigid_body bc_rigid_dynamic_component::get_body() noexcept
		{
			return m_px_actor_ref.get();
		}

		physics::bc_rigid_dynamic bc_rigid_dynamic_component::get_dynamic_body() const noexcept
		{
			return m_px_actor_ref.get();
		}

		bc_actor bc_rigid_dynamic_component::get_actor() const noexcept
		{
			return get_manager()->component_get_actor(*this);
		}

		void bc_rigid_dynamic_component::initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters)
		{
			auto* l_mesh_component = p_actor.get_component<bc_mesh_component>();

			if (l_mesh_component)
			{
				auto& l_physics_system = core::bc_get_service< bc_game_system >()->get_physics_system();
				auto& l_physics = l_physics_system.get_physics();

				m_px_actor_ref = l_physics.create_rigid_dynamic(physics::bc_transform::identity());

				initialize_from_mesh(l_physics_system, p_actor, m_px_actor_ref.get(), *l_mesh_component);

				return;
			}

			throw bc_invalid_operation_exception("Rigid dynamic component need mesh component.");
		}

		void bc_rigid_dynamic_component::update(const bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock_update_param)
		{
		}	
	}
}
