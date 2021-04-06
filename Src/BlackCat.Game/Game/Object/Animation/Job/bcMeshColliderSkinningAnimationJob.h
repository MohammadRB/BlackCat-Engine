// [01/29/2021 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Container/bcVector.h"
#include "Game/Object/Animation/bcAnimationJob.h"
#include "Game/Object/Mesh/bcSubMesh.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_mesh_collider_skinning_animation_job : public bci_animation_job
		{
		public:
			bc_mesh_collider_skinning_animation_job(bc_animation_skeleton& p_skeleton, const bc_sub_mesh& p_sub_mesh, const bc_sub_mesh_mat4_transform& p_model_transforms);

			bc_mesh_collider_skinning_animation_job(bc_mesh_collider_skinning_animation_job&& p_other) noexcept;

			~bc_mesh_collider_skinning_animation_job();

			bc_mesh_collider_skinning_animation_job& operator=(bc_mesh_collider_skinning_animation_job&& p_other) noexcept;

			bcSIZE get_vertices_count() const noexcept;
			
			const core::bc_vector3f* get_vertices() const noexcept;

			bcSIZE get_faces_count() const noexcept;

			const bcUINT16* get_16bit_indices() const noexcept;
			
			const bcUINT32* get_32bit_indices() const noexcept;
						
			bool run(const core_platform::bc_clock::update_param& p_clock) override;
			
		private:
			const bc_sub_mesh* m_sub_mesh;
			const bc_sub_mesh_mat4_transform* m_model_transforms;
			bc_sub_mesh_mat4_transform m_inverse_model_transforms;
			core::bc_vector_movable< core::bc_vector3f > m_skinned_vertices;
		};

		inline bcSIZE bc_mesh_collider_skinning_animation_job::get_vertices_count() const noexcept
		{
			return m_sub_mesh->get_mesh_collider().get_skinned_collider().get_vertices_count();
		}

		inline const core::bc_vector3f* bc_mesh_collider_skinning_animation_job::get_vertices() const noexcept
		{
			return m_skinned_vertices.data();
		}

		inline bcSIZE bc_mesh_collider_skinning_animation_job::get_faces_count() const noexcept
		{
			return m_sub_mesh->get_mesh_collider().get_skinned_collider().get_faces_count();
		}

		inline const bcUINT16* bc_mesh_collider_skinning_animation_job::get_16bit_indices() const noexcept
		{
			return m_sub_mesh->get_mesh_collider().get_skinned_collider().get_16bit_indices();
		}

		inline const bcUINT32* bc_mesh_collider_skinning_animation_job::get_32bit_indices() const noexcept
		{
			return m_sub_mesh->get_mesh_collider().get_skinned_collider().get_32bit_indices();
		}
	}	
}