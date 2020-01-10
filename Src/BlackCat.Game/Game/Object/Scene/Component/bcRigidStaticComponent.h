// [01/07/2017 MRB]

#pragma once

#include "PhysicsImp/Body/bcRigidStatic.h"
#include "Game/Object/Scene/Component/bcRigidBodyComponent.h"
#include "PlatformImp/bcIDELogger.h"

namespace black_cat
{
	namespace game
	{
		class bc_physics_system;
		class bc_height_map_component;

		class BC_GAME_DLL bc_rigid_static_component : public bc_rigid_body_component
		{
			BC_COMPONENT(rigid_static)

		public:
			explicit bc_rigid_static_component(bc_actor_component_index p_index) noexcept;

			bc_rigid_static_component(bc_rigid_static_component&&) noexcept;

			~bc_rigid_static_component();

			bc_rigid_static_component& operator=(bc_rigid_static_component&&) noexcept;

			physics::bc_rigid_body get_body() noexcept override;

			physics::bc_rigid_static get_static_body() const noexcept;

			bc_actor get_actor() const noexcept override;

			void initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters) override;

		private:
			void update(const bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock_update_param) override;

			void handle_event(const bc_actor& p_actor, const bc_actor_event& p_event) override;

			void _initialize_from_height_map(bc_physics_system& p_physics_system, bc_actor& p_actor, physics::bc_rigid_static& p_rigid_static, bc_height_map_component& p_component);

			physics::bc_rigid_static_ref m_px_actor_ref;
		};
	}
}