// [03/29/2021 MRB]

#pragma once

#include "Game/Object/Animation/Job/bcActorUpdateAnimationJob.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_xbot_controller;
		
		class BC_GAME_DLL bc_xbot_update_animation_job : public bc_actor_update_animation_job
		{
		public:
			bc_xbot_update_animation_job(const bc_actor& p_actor,
				bc_xbot_controller& p_xbot,
				bc_skinned_mesh_component& p_component,
				const core::bc_shared_ptr<bc_model_to_skinned_animation_job>& p_model_to_skinned_job);

			bc_xbot_update_animation_job(bc_xbot_update_animation_job&&) noexcept;

			~bc_xbot_update_animation_job() override;

			bc_xbot_update_animation_job& operator=(bc_xbot_update_animation_job&&) noexcept;

			bool run(const core_platform::bc_clock::update_param& p_clock) override;
			
		private:
			bc_xbot_controller* m_xbot_controller;
		};
	}	
}