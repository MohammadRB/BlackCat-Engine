// [12/31/2020 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Game/Object/Mesh/bcSubMesh.h"
#include "Game/Object/Animation/bcAnimationJob.h"
#include "Game/Object/Animation/Job/bcAnimationJobLocalTransform.h"
#include "Game/bcExport.h"

#include "3rdParty/Ozz/Include/ozz/base/maths/simd_math.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_animation_job_local_to_model_transform : public bci_animation_job
		{
		public:
			explicit bc_animation_job_local_to_model_transform(bci_animation_job_local_transform& p_local_transform_job, const bc_sub_mesh& p_sub_mesh, bc_sub_mesh_mat4_transform& p_transforms);

			bc_animation_job_local_to_model_transform(bc_animation_job_local_to_model_transform&&) noexcept = default;

			~bc_animation_job_local_to_model_transform() = default;

			bc_animation_job_local_to_model_transform& operator=(bc_animation_job_local_to_model_transform&&) noexcept = default;

			const bc_sub_mesh& get_mesh() const noexcept;
			
			const bc_sub_mesh_mat4_transform& get_transforms() const noexcept;
			
			bool run(const core_platform::bc_clock::update_param& p_clock) override;
			
		private:
			const bc_sub_mesh* m_mesh;
			bci_animation_job_local_transform* m_local_transform_job;
			core::bc_vector_movable<ozz::math::Float4x4> m_ozz_model_transforms;
			bc_sub_mesh_mat4_transform* m_model_transforms;
		};

		inline const bc_sub_mesh& bc_animation_job_local_to_model_transform::get_mesh() const noexcept
		{
			return *m_mesh;
		}
		
		inline const bc_sub_mesh_mat4_transform& bc_animation_job_local_to_model_transform::get_transforms() const noexcept
		{
			return *m_model_transforms;
		}
	}
}