// [12/31/2020 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Game/Object/Mesh/bcSubMeshTransform.h"
#include "Game/Object/Animation/bcAnimationJob.h"
#include "Game/Object/Animation/Job/bcLocalTransformAnimationJob.h"
#include "Game/bcExport.h"
#include "3rdParty/Ozz/Include/ozz/base/maths/simd_math.h"

namespace black_cat
{
	namespace game
	{
		class bc_sub_mesh;
		
		class BC_GAME_DLL bc_local_to_model_animation_job : public bci_animation_job
		{
		public:
			bc_local_to_model_animation_job(bci_local_transform_animation_job& p_local_transform_job, 
				const bc_sub_mesh& p_sub_mesh, 
				bc_sub_mesh_mat4_transform& p_transforms);

			bc_local_to_model_animation_job(bc_local_to_model_animation_job&&) noexcept = default;

			~bc_local_to_model_animation_job() = default;

			bc_local_to_model_animation_job& operator=(bc_local_to_model_animation_job&&) noexcept = default;

			const bc_sub_mesh& get_mesh() const noexcept;
			
			const bc_sub_mesh_mat4_transform& get_transforms() const noexcept;
			
			bool run(const core_platform::bc_clock::update_param& p_clock) override;
			
		private:
			const bc_sub_mesh* m_mesh;
			bci_local_transform_animation_job* m_local_transform_job;
			core::bc_vector_movable<ozz::math::Float4x4> m_ozz_model_transforms;
			bc_sub_mesh_mat4_transform* m_model_transforms;
		};

		inline const bc_sub_mesh& bc_local_to_model_animation_job::get_mesh() const noexcept
		{
			return *m_mesh;
		}
		
		inline const bc_sub_mesh_mat4_transform& bc_local_to_model_animation_job::get_transforms() const noexcept
		{
			return *m_model_transforms;
		}
	}
}