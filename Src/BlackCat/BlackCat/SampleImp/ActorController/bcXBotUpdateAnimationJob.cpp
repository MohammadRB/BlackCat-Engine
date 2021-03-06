// [03/29/2021 MRB]

#include "BlackCat/BlackCatPCH.h"
#include "BlackCat/SampleImp/ActorController/bcXBotController.h"
#include "BlackCat/SampleImp/ActorController/bcXBotUpdateAnimationJob.h"

namespace black_cat
{
	bc_xbot_update_animation_job::bc_xbot_update_animation_job(const game::bc_actor& p_xbot_actor,
		bc_xbot_controller& p_xbot,
		game::bc_skinned_mesh_component& p_component,
		const core::bc_shared_ptr<game::bc_model_to_skinned_animation_job>& p_model_to_skinned_job)
		: bc_actor_update_animation_job(p_xbot_actor, p_component, p_model_to_skinned_job),
		m_xbot_controller(&p_xbot)
	{
	}

	bc_xbot_update_animation_job::bc_xbot_update_animation_job(bc_xbot_update_animation_job&&) noexcept = default;

	bc_xbot_update_animation_job::~bc_xbot_update_animation_job() = default;

	bc_xbot_update_animation_job& bc_xbot_update_animation_job::operator=(bc_xbot_update_animation_job&&) noexcept = default;

	bool bc_xbot_update_animation_job::run(const core_platform::bc_clock::update_param& p_clock)
	{
		bc_actor_update_animation_job::run(p_clock);
		m_xbot_controller->_update_weapon_transform();

		return true;
	}
}