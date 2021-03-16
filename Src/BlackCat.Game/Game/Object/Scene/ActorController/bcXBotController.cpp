// [01/04/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/ActorController/bcXBotController.h"
#include "Game/Object/Scene/Component/bcSkinnedMeshComponent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcHierarchyTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcBoundBoxChangedActorEvent.h"
#include "Game/Object/Animation/bcAnimationSkeleton.h"
#include "Game/Object/Animation/bcSkeletonAnimation.h"
#include "Game/Object/Animation/bcAnimationJobBuilder.h"
#include "Game/Object/Animation/Job/bcBlendingAnimationJob.h"
#include "Game/Object/Animation/Job/bcPartialBlendingAnimationJob.h"
#include "Game/Object/Animation/Job/bcActorUpdateAnimationJob.h"

namespace black_cat
{
	namespace game
	{
		void bc_xbot_controller::initialize(const bc_actor_component_initialize_context& p_context)
		{
			m_skinned_component = p_context.m_actor.get_component< bc_skinned_mesh_component >();
			m_local_forward = core::bc_vector3f(0, 0, -1);

			if(!m_skinned_component)
			{
				throw bc_invalid_operation_exception("xbot actor must have skinned components");
			}

			_create_idle_animation(p_context.m_actor);
			//_create_running_animation(p_context.m_actor);
		}

		void bc_xbot_controller::added_to_scene(const bc_actor_component_event_context& p_context, bc_scene& p_scene)
		{
			m_active_job = m_idle_job.get();
		}

		void bc_xbot_controller::update(const bc_actor_component_update_content& p_context)
		{
			if(m_active_job)
			{
				m_skinned_component->add_animation_job(*m_active_job);
			}
		}

		void bc_xbot_controller::handle_event(const bc_actor_component_event_context& p_context)
		{
			const auto* l_world_transform_event = core::bci_message::as<bc_world_transform_actor_event>(p_context.m_event);
			if(l_world_transform_event)
			{
				bc_animation_job_builder::set_skinning_world_transform(static_cast<bc_sequence_animation_job&>(*m_idle_job), l_world_transform_event->get_transform());
				//bc_animation_job_builder::set_skinning_world_transform(static_cast<bc_sequence_animation_job&>(*m_running_job), l_world_transform_event->get_transform());
			}
		}
		
		void bc_xbot_controller::_create_idle_animation(bc_actor& p_actor)
		{
			auto& l_skeleton = *m_skinned_component->get_skeleton();
			auto& l_idle_animation = *m_skinned_component->find_animation("NeutralIdle");

			m_idle_sample_job = core::bc_make_shared<bc_sampling_animation_job>(bc_sampling_animation_job(l_skeleton, l_idle_animation));
			auto l_local_to_model_job = core::bc_make_shared<bc_local_to_model_animation_job>
			(
				bc_local_to_model_animation_job(*m_idle_sample_job, m_skinned_component->get_mesh(), m_skinned_component->get_model_transforms())
			);
			auto l_model_to_skinning_job = core::bc_make_shared<bc_model_to_skinned_animation_job>
			(
				bc_model_to_skinned_animation_job(*l_local_to_model_job, m_skinned_component->get_world_transforms())
			);
			auto l_actor_update_job = core::bc_make_shared<bc_actor_update_animation_job>
			(
				bc_actor_update_animation_job(p_actor, *m_skinned_component, l_model_to_skinning_job)
			);

			m_idle_job = bc_animation_job_builder()
			             .start_with(m_idle_sample_job)
			             .then(std::move(l_local_to_model_job))
			             .then(std::move(l_model_to_skinning_job))
			             .end_with(std::move(l_actor_update_job))
			             .build();
		}

		void bc_xbot_controller::_create_running_animation(bc_actor& p_actor)
		{
			auto& l_skeleton = *m_skinned_component->get_skeleton();
			auto& l_running_animation = *m_skinned_component->find_animation("Running");

			m_running_sample_job = core::bc_make_shared<bc_sampling_animation_job>(bc_sampling_animation_job(l_skeleton, l_running_animation));
			auto l_blending_job = core::bc_make_shared<bc_blending_animation_job>(bc_blending_animation_job(l_skeleton, { m_idle_sample_job, m_running_sample_job }));
			auto l_local_to_model_job = core::bc_make_shared<bc_local_to_model_animation_job>
			(
				bc_local_to_model_animation_job(*m_idle_sample_job, m_skinned_component->get_mesh(), m_skinned_component->get_model_transforms())
			);
			auto l_model_to_skinning_job = core::bc_make_shared<bc_model_to_skinned_animation_job>
			(
				bc_model_to_skinned_animation_job(*l_local_to_model_job, m_skinned_component->get_world_transforms())
			);
			auto l_actor_update_job = core::bc_make_shared<bc_actor_update_animation_job>
			(
				bc_actor_update_animation_job(p_actor, *m_skinned_component, l_model_to_skinning_job)
			);

			m_running_job = bc_animation_job_builder()
				.start_with(std::move(l_blending_job))
				.then(std::move(l_local_to_model_job))
				.then(std::move(l_model_to_skinning_job))
				.end_with(std::move(l_actor_update_job))
				.build();
		}
	}
}