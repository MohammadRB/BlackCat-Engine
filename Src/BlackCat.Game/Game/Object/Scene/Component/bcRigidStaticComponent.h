// [01/07/2017 MRB]

#pragma once

#include "PhysicsImp/Body/bcRigidStatic.h"
#include "Game/Object/Scene/Component/bcRigidBodyComponent.h"

namespace black_cat
{
	namespace game
	{
		class bc_height_map;
		class bc_physics_system;

		class BC_GAME_DLL bc_rigid_static_component : public bc_rigid_body_component
		{
			BC_COMPONENT(rgd_stc, true, false)

		public:
			explicit bc_rigid_static_component(bc_actor_component_index p_index) noexcept;

			bc_rigid_static_component(bc_rigid_static_component&&) noexcept;

			~bc_rigid_static_component();

			bc_rigid_static_component& operator=(bc_rigid_static_component&&) noexcept;

			bc_actor get_actor() const noexcept override;

			physics::bc_rigid_body& get_body() noexcept override;

			physics::bc_rigid_static get_static_body() const noexcept;

			void initialize(bc_actor_component_initialize_context& p_context) override;

			void handle_event(bc_actor_component_event_context& p_context) override;

		private:
			void create_px_shapes_from_height_map(bc_physics_system& p_physics_system,
				physics::bc_rigid_static& p_rigid_static, 
				const bc_height_map& p_height_map);

			physics::bc_rigid_static_ref m_px_actor_ref;
		};
	}
}