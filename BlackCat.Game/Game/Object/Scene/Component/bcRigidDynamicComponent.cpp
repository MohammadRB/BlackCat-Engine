// [01/07/2017 MRB]

#include "Game/GamePCH.h"
#include "Game/bcExport.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcRigidDynamicComponent.h"

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
			m_px_actor(std::move(p_other.m_px_actor))
		{
		}

		bc_rigid_dynamic_component::~bc_rigid_dynamic_component()
		{
		}

		bc_rigid_dynamic_component& bc_rigid_dynamic_component::operator=(bc_rigid_dynamic_component&& p_other) noexcept
		{
			bc_rigid_body_component::operator=(std::move(p_other));
			m_px_actor = std::move(p_other.m_px_actor);

			return *this;
		}

		physics::bc_rigid_body& bc_rigid_dynamic_component::get_body() noexcept
		{
			return m_px_actor.get();
		}

		physics::bc_rigid_dynamic bc_rigid_dynamic_component::get_dynamic_body() const noexcept
		{
			return m_px_actor.get();
		}

		void bc_rigid_dynamic_component::initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters)
		{
			auto* l_mesh_component = p_actor.get_component<bc_mesh_component>();

			if (l_mesh_component)
			{
				// TODO do whatever related to mesh

				return;
			}

			throw bc_invalid_operation_exception("Rigid dynamic component need mesh component.");
		}

		void bc_rigid_dynamic_component::update(const bc_actor& p_actor, core_platform::bc_clock::update_param p_clock_update_param)
		{
		}
	}
}