// [03/15/2021 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Animation/bcAnimationJob.h"
#include "Game/Object/Animation/Job/bcModelToSkinnedAnimationJob.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_skinned_mesh_component;
		
		class BC_GAME_DLL bc_actor_update_animation_job : public bci_animation_job
		{
		public:
			bc_actor_update_animation_job(bc_actor p_actor,
				bc_skinned_mesh_component& p_component,
				core::bc_shared_ptr<bc_model_to_skinned_animation_job> p_model_to_skinned_job);

			bc_actor_update_animation_job(bc_actor_update_animation_job&&) noexcept;

			~bc_actor_update_animation_job() override;

			bc_actor_update_animation_job& operator=(bc_actor_update_animation_job&&) noexcept;

			bool run(const core_platform::bc_clock::update_param& p_clock) override;
			
		private:
			bc_actor m_actor;
			bc_skinned_mesh_component* m_skinned_component;
			core::bc_shared_ptr<bc_model_to_skinned_animation_job> m_model_to_skinned_job;
		};
	}	
}