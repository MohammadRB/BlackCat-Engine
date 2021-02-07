// [01/04/2021 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcVector.h"
#include "Game/Object/Scene/ActorComponent/bcActorController.h"
#include "Game/Object/Animation/bcAnimationJobBuilder.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_skeleton_animation;
		class bc_skinned_mesh_component;

		class BC_GAME_DLL bc_xbot_controller : public bci_actor_controller
		{
		public:
			void initialize(bc_actor& p_actor) override;
			
			void added_to_scene(bc_actor& p_actor, bc_scene& p_scene) override;
			
			void update(bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock) override;
			
			void handle_event(bc_actor& p_actor, const bc_actor_event& p_event) override;

		private:
			bc_skeleton_animation* _find_animation(const bcCHAR* p_name) const noexcept;

			bc_skinned_mesh_component* m_skinned_component;
			core::bc_vector_movable<bc_skeleton_animation*> m_all_animations;
			core::bc_unique_ptr<bc_sampling_animation_job> m_sample_job;
			core::bc_unique_ptr<bc_animation_job_local_to_model_transform> m_local_to_model_job;
			core::bc_unique_ptr<bc_model_to_skinned_transform_animation_job> m_model_to_skinned_job;

			core::bc_unique_ptr<bci_animation_job> m_idle_job;
		};
	}
}