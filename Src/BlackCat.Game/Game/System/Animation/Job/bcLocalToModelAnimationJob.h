// [31/12/2020 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcSpan.h"
#include "Game/Object/Mesh/bcSubMeshTransform.h"
#include "Game/System/Animation/bcAnimationJob.h"
#include "Game/System/Animation/Job/bcLocalTransformAnimationJob.h"
#include "Game/bcExport.h"
#include "3rdParty/Ozz/Include/ozz/base/maths/simd_math.h"

namespace black_cat::game
{
	class bc_sub_mesh;
		
	class BC_GAME_DLL bc_local_to_model_animation_job : public bci_animation_job
	{
	public:
		bc_local_to_model_animation_job(core::bc_shared_ptr< bci_local_transform_animation_job> p_local_transform_job,
		                                const bc_sub_mesh& p_sub_mesh, 
		                                bc_sub_mesh_mat4_transform& p_model_transforms);

		bc_local_to_model_animation_job(bc_local_to_model_animation_job&&) noexcept = default;

		~bc_local_to_model_animation_job() override = default;

		bc_local_to_model_animation_job& operator=(bc_local_to_model_animation_job&&) noexcept = default;

		const bc_sub_mesh& get_mesh() const noexcept;
			
		const bc_sub_mesh_mat4_transform& get_transforms() const noexcept;

		core::bc_span<ozz::math::Float4x4> get_native_transforms() noexcept;
			
		bool run(const platform::bc_clock::update_param& p_clock) override;
			
		bool run(const platform::bc_clock::update_param& p_clock, bcINT32 p_from);
			
	private:
		const bc_sub_mesh* m_mesh;
		core::bc_shared_ptr<bci_local_transform_animation_job> m_local_transform_job;
		core::bc_vector_movable_aligned<ozz::math::Float4x4, 16> m_ozz_model_transforms;
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

	inline core::bc_span<ozz::math::Float4x4> bc_local_to_model_animation_job::get_native_transforms() noexcept
	{
		return core::bc_make_span(m_ozz_model_transforms);
	}
}
