// [25/11/2021 MRB]

#pragma once

#include "PhysicsImp/Shape/bcBoundBox.h"
#include "Game/Object/Mesh/bcSubMeshTransform.h"
#include "Game/System/Animation/bcAnimationJob.h"

namespace black_cat
{
	namespace game
	{
		class bci_skinning_transform_animation_job : public bci_animation_job
		{
		public:
			virtual ~bci_skinning_transform_animation_job() override;

			virtual const core::bc_matrix4f& get_world() const noexcept = 0;

			virtual void set_world(const core::bc_matrix4f& p_world) noexcept = 0;
			
			virtual const physics::bc_bound_box& get_bound_box() const noexcept = 0;

			virtual const bc_sub_mesh_mat4_transform& get_skinning_transforms() const noexcept = 0;

		protected:
			explicit bci_skinning_transform_animation_job(bc_animation_skeleton& p_skeleton);

			bci_skinning_transform_animation_job(bci_skinning_transform_animation_job&&) noexcept = default;

			bci_skinning_transform_animation_job& operator=(bci_skinning_transform_animation_job&&) noexcept = default;
		};

		inline bci_skinning_transform_animation_job::~bci_skinning_transform_animation_job() = default;

		inline bci_skinning_transform_animation_job::bci_skinning_transform_animation_job(bc_animation_skeleton& p_skeleton)
			: bci_animation_job(p_skeleton)
		{
		}
	}
}