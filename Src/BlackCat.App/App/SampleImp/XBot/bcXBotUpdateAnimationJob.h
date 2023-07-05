// [29/03/2021 MRB]

#pragma once

#include "Game/System/Animation/Job/bcActorUpdateAnimationJob.h"
#include "App/bcExport.h"

namespace black_cat
{
	class bc_xbot_actor_controller;
	
	class BC_DLL bc_xbot_update_animation_job : public game::bc_actor_update_animation_job
	{
	public:
		bc_xbot_update_animation_job(const game::bc_actor& p_xbot_actor,
			bc_xbot_actor_controller& p_xbot_controller,
			game::bc_skinned_mesh_component& p_component,
			const core::bc_shared_ptr<game::bc_model_to_skinning_animation_job>& p_model_to_skinned_job);

		bc_xbot_update_animation_job(bc_xbot_update_animation_job&&) noexcept;

		~bc_xbot_update_animation_job() override;

		bc_xbot_update_animation_job& operator=(bc_xbot_update_animation_job&&) noexcept;

		bool run(const platform::bc_clock::update_param& p_clock) override;
		
	private:
		bc_xbot_actor_controller* m_xbot_controller;
	};
}