// [01/15/2021 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Core/Math/bcMatrix4f.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "Game/Object/Mesh/bcSubMeshTransform.h"
#include "Game/Object/Animation/bcAnimationJob.h"
#include "Game/Object/Animation/Job/bcLocalToModelAnimationJob.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_model_to_skinned_animation_job : public bci_animation_job
		{
		public:
			bc_model_to_skinned_animation_job(core::bc_shared_ptr<bc_local_to_model_animation_job> p_model_job, bc_sub_mesh_mat4_transform& p_skinned_world_transforms);

			bc_model_to_skinned_animation_job(bc_model_to_skinned_animation_job&&) noexcept = default;

			~bc_model_to_skinned_animation_job() = default;

			bc_model_to_skinned_animation_job& operator=(bc_model_to_skinned_animation_job&&) noexcept = default;

			const physics::bc_bound_box& get_bound_box() const noexcept;

			const bc_sub_mesh_mat4_transform& get_world_transforms() const noexcept;
			
			const core::bc_matrix4f& get_world() const noexcept;
			
			void set_world(const core::bc_matrix4f& p_world) noexcept;
			
			bool run(const core_platform::bc_clock::update_param& p_clock) override;

		private:
			core::bc_shared_ptr<bc_local_to_model_animation_job> m_model_job;
			bc_sub_mesh_mat4_transform* m_transforms;
			core::bc_matrix4f m_world;
			physics::bc_bound_box m_bound_box;
		};

		inline const physics::bc_bound_box& bc_model_to_skinned_animation_job::get_bound_box() const noexcept
		{
			return m_bound_box;
		}

		inline const bc_sub_mesh_mat4_transform& bc_model_to_skinned_animation_job::get_world_transforms() const noexcept
		{
			return *m_transforms;
		}
		
		inline const core::bc_matrix4f& bc_model_to_skinned_animation_job::get_world() const noexcept
		{
			return m_world;
		}
		
		inline void bc_model_to_skinned_animation_job::set_world(const core::bc_matrix4f& p_world) noexcept
		{
			m_world = p_world;
		}
	}	
}