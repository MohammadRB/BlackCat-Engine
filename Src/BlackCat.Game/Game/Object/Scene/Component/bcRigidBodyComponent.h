// [01/07/2017 MRB]

#pragma once

#include "PhysicsImp/Body/bcRigidBody.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"

namespace black_cat
{
	namespace game
	{
		class bc_physics_system;
		class bc_sub_mesh;
		class bc_sub_mesh_transform;

		class bc_rigid_body_component : public bci_actor_component
		{
			BC_ABSTRACT_COMPONENT(rgd_bdy)

		public:
			virtual ~bc_rigid_body_component();

			virtual physics::bc_rigid_body& get_body() noexcept = 0;

		protected:
			explicit bc_rigid_body_component(bc_actor_component_index p_index) noexcept;

			bc_rigid_body_component(bc_rigid_body_component&&) noexcept;

			bc_rigid_body_component& operator=(bc_rigid_body_component&&) noexcept;

			void create_px_shapes_from_mesh(bc_physics_system& p_physics_system,
				physics::bc_rigid_body& p_px_actor,
				const bc_sub_mesh& p_mesh);

			void update_px_shape_transforms(physics::bc_rigid_body& p_px_actor,
				const bc_sub_mesh_transform& p_model_space_transforms);
		};

		inline bc_rigid_body_component::~bc_rigid_body_component() = default;

		inline bc_rigid_body_component::bc_rigid_body_component(bc_actor_component_index p_index) noexcept
			: bci_actor_component(p_index)
		{
		}

		inline bc_rigid_body_component::bc_rigid_body_component(bc_rigid_body_component&&) noexcept = default;

		inline bc_rigid_body_component& bc_rigid_body_component::operator=(bc_rigid_body_component&&) noexcept = default;
	}
}