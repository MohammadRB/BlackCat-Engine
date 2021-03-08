// [01/04/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/ActorController/bcXBotController.h"
#include "Game/Object/Scene/Component/bcSkinnedMeshComponent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcHierarchyTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcBoundBoxChangedActorEvent.h"
#include "Game/Object/Animation/bcAnimationSkeleton.h"
#include "Game/Object/Animation/bcSkeletonAnimation.h"

namespace black_cat
{
	namespace game
	{
		void bc_xbot_controller::initialize(bc_actor& p_actor)
		{
			m_skinned_component = p_actor.get_component<bc_skinned_mesh_component>();

			if(!m_skinned_component)
			{
				throw bc_invalid_operation_exception("xbot actor must have skinned components");
			}

			auto l_skinned_animations = m_skinned_component->get_animations();
			
			BC_ASSERT(l_skinned_animations.size());
			
			m_all_animations.reserve(l_skinned_animations.size());

			for (bc_skinned_animation_ptr& l_skinned_animation : l_skinned_animations)
			{
				for (bc_skeleton_animation& l_animation : l_skinned_animation->get_animations())
				{
					m_all_animations.push_back(&l_animation);
				}
			}

			bc_animation_skeleton& l_skeleton = (*l_skinned_animations.begin())->get_skeleton();
			//bc_skeleton_animation& l_idle_animation = *_find_animation("Armature|NatualIdle");
			bc_skeleton_animation& l_idle_animation = *m_all_animations[0];

			m_sample_job = core::bc_make_unique< bc_sampling_animation_job >
			(
				bc_sampling_animation_job(l_skeleton, l_idle_animation)
			);
			m_local_to_model_job = core::bc_make_unique< bc_local_to_model_transform_animation_job >
			(
				bc_local_to_model_transform_animation_job
				(
					*m_sample_job,
					m_skinned_component->get_mesh(),
					m_skinned_component->get_model_transforms()
				)
			);
			m_model_to_skinned_job = core::bc_make_unique< bc_model_to_skinned_transform_animation_job >
			(
				bc_model_to_skinned_transform_animation_job
				(
					*m_local_to_model_job,
					m_skinned_component->get_world_transforms()
				)
			);
		}

		void bc_xbot_controller::added_to_scene(const bc_actor_component_event_context& p_context, bc_scene& p_scene)
		{
			m_idle_job = bc_animation_job_builder()
				.start_with(*m_sample_job)
				.then(*m_local_to_model_job)
				.end_with(*m_model_to_skinned_job)
				.afterward([this, l_actor = p_context.m_actor, l_model_to_skinned_job = m_model_to_skinned_job.get()]() mutable
				{
					const auto& l_mesh = m_skinned_component->get_mesh();

					l_mesh.calculate_skinned_mesh_collider_transforms
					(
						m_skinned_component->get_model_transforms(),
						m_skinned_component->get_collider_model_transforms()
					);
					
					l_actor.add_event(bc_bound_box_changed_actor_event(l_model_to_skinned_job->get_bound_box()));
					l_actor.add_event(bc_hierarchy_transform_actor_event(m_skinned_component->get_collider_model_transforms()));
				})
				.build();
		}

		void bc_xbot_controller::update(const bc_actor_component_update_content& p_context)
		{
			if(m_idle_job)
			{
				m_skinned_component->add_animation_job(*m_idle_job);
			}
		}

		void bc_xbot_controller::handle_event(const bc_actor_component_event_context& p_context)
		{
			const auto* l_world_transform_event = core::bci_message::as<bc_world_transform_actor_event>(p_context.m_event);
			if(l_world_transform_event)
			{
				m_model_to_skinned_job->set_world(l_world_transform_event->get_transform());
			}
		}

		bc_skeleton_animation* bc_xbot_controller::_find_animation(const bcCHAR* p_name) const noexcept
		{
			const auto l_ite = std::find_if(std::begin(m_all_animations), std::end(m_all_animations), [=](bc_skeleton_animation* p_animation)
			{
				return std::strcmp(p_animation->get_name(), p_name) == 0;
			});

			if(l_ite != std::end(m_all_animations))
			{
				return *l_ite;
			}

			return nullptr;
		}
	}
}