// [01/29/2021 MRB]

#pragma once

#include "Game/Object/Animation/bcAnimationJob.h"
#include "Game/Object/Mesh/bcSubMesh.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_animation_job_mesh_skinning : public bci_animation_job
		{
		public:
			explicit bc_animation_job_mesh_skinning(bc_animation_skeleton& p_skeleton, const bc_sub_mesh& p_sub_mesh);

			bc_animation_job_mesh_skinning(bc_animation_job_mesh_skinning&& p_other) noexcept;

			~bc_animation_job_mesh_skinning();

			bc_animation_job_mesh_skinning& operator=(bc_animation_job_mesh_skinning&& p_other) noexcept;
			
		private:
			
		};
	}	
}