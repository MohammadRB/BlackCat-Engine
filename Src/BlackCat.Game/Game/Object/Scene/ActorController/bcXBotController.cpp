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

namespace black_cat
{
	namespace game
	{
		void bc_xbot_controller::initialize(const bc_actor_component_initialize_context& p_context)
		{
			m_skinned_component = p_context.m_actor.get_component<bc_skinned_mesh_component>();
			m_local_forward = core::bc_vector3f(0, 0, -1);

			if(!m_skinned_component)
			{
				throw bc_invalid_operation_exception("xbot actor must have skinned components");
			}

			auto& l_skeleton = *m_skinned_component->get_skeleton();
			auto& l_idle_animation = *m_skinned_component->find_animation("NatualIdle");

			auto l_sample_job = core::bc_make_unique<bc_sampling_animation_job>(bc_sampling_animation_job(l_skeleton, l_idle_animation));
			auto l_local_to_model_job = core::bc_make_unique<bc_local_to_model_transform_animation_job>(bc_local_to_model_transform_animation_job
			(
				*l_sample_job,
				m_skinned_component->get_mesh(),
				m_skinned_component->get_model_transforms()
			));
			auto l_model_to_skinned_job = core::bc_make_unique<bc_model_to_skinned_transform_animation_job>(bc_model_to_skinned_transform_animation_job
			(
				*l_local_to_model_job,
				m_skinned_component->get_world_transforms()
			));
			m_idle_model_to_skinned_job = l_model_to_skinned_job.get();
			
			m_idle_job = bc_animation_job_builder()
	             .start_with(std::move(l_sample_job))
	             .then(std::move(l_local_to_model_job))
	             .end_with(std::move(l_model_to_skinned_job))
	             .afterward
	             (
		             [this, l_actor = p_context.m_actor]() mutable
		             {
			             const auto& l_mesh = m_skinned_component->get_mesh();

			             l_mesh.calculate_skinned_mesh_collider_transforms
			             (
				             m_skinned_component->get_model_transforms(),
				             m_skinned_component->get_collider_model_transforms()
			             );

			             l_actor.add_event(bc_bound_box_changed_actor_event(m_idle_model_to_skinned_job->get_bound_box()));
			             l_actor.add_event(bc_hierarchy_transform_actor_event(m_skinned_component->get_collider_model_transforms()));
		             }
	             )
	             .build();
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
				m_idle_model_to_skinned_job->set_world(l_world_transform_event->get_transform());
			}
		}
	}
}