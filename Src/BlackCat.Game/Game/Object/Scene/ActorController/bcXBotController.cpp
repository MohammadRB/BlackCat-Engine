// [01/04/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/ActorController/bcXBotController.h"
#include "Game/Object/Scene/Component/bcSkinnedMeshComponent.h"
#include "Game/Object/Scene/Component/Event/bcActorEventWorldTransform.h"
#include "Game/Object/Animation/bcAnimationSkeleton.h"
#include "Game/Object/Animation/bcSkeletonAnimation.h"

namespace black_cat
{
	namespace game
	{
		void bc_xbot_controller::initialize(bc_actor& p_actor)
		{
			auto* l_skinned_mesh_component = p_actor.get_component<bc_skinned_mesh_component>();

			if(!l_skinned_mesh_component)
			{
				throw bc_invalid_operation_exception("xbot actor must have skinned components");
			}

			auto l_skinned_animations = l_skinned_mesh_component->get_animations();
			
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

			m_sample_job = core::bc_make_unique< bc_animation_job_sampling >
			(
				bc_animation_job_sampling(l_skeleton, l_idle_animation)
			);
			m_local_to_model_job = core::bc_make_unique< bc_animation_job_local_to_model_transform >
			(
				bc_animation_job_local_to_model_transform
				(
					*m_sample_job,
					l_skinned_mesh_component->get_mesh(),
					l_skinned_mesh_component->get_mesh_transforms()
				)
			);
		}

		void bc_xbot_controller::added_to_scene(bc_actor& p_actor, bc_scene& p_scene)
		{
			m_idle_job = bc_animation_job_builder()
				.start_with(*m_sample_job)
				.end_with(*m_local_to_model_job);
		}

		void bc_xbot_controller::update(bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock)
		{
			if(m_idle_job)
			{
				auto* l_skinned_mesh_component = p_actor.get_component<bc_skinned_mesh_component>();
				l_skinned_mesh_component->add_animation_job(*m_idle_job);
			}
		}

		void bc_xbot_controller::handle_event(bc_actor& p_actor, const bc_actor_event& p_event)
		{
			const auto* l_world_transform_event = core::bc_imessage::as<bc_actor_event_world_transform>(p_event);
			if(l_world_transform_event)
			{
				auto* l_skinned_mesh_component = p_actor.get_component<bc_skinned_mesh_component>();
				const auto l_mesh_scale = l_skinned_mesh_component->get_mesh().get_mesh_scale();
				//m_local_to_model_job->set_world(core::bc_matrix4f::scale_matrix(l_mesh_scale) * l_world_transform_event->get_transform());
				m_local_to_model_job->set_world(l_world_transform_event->get_transform());
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