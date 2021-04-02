// [03/15/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/Component/bcSkinnedMeshComponent.h"
#include "Game/Object/Scene/Component/Event/bcHierarchyTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcBoundBoxChangedActorEvent.h"
#include "Game/Object/Animation/Job/bcActorUpdateAnimationJob.h"

namespace black_cat
{
	namespace game
	{
		bc_actor_update_animation_job::bc_actor_update_animation_job(bc_actor p_actor,
			bc_skinned_mesh_component& p_component,
			core::bc_shared_ptr< bc_model_to_skinned_animation_job > p_model_to_skinned_job)
			: bci_animation_job(p_model_to_skinned_job->get_skeleton()),
			m_actor(p_actor),
			m_skinned_component(&p_component),
			m_model_to_skinned_job(std::move(p_model_to_skinned_job))
		{
		}

		bc_actor_update_animation_job::bc_actor_update_animation_job(bc_actor_update_animation_job&&) noexcept = default;

		inline bc_actor_update_animation_job::~bc_actor_update_animation_job() = default;

		inline bc_actor_update_animation_job& bc_actor_update_animation_job::operator=(bc_actor_update_animation_job&&) noexcept = default;

		inline bool bc_actor_update_animation_job::run(const core_platform::bc_clock::update_param& p_clock)
		{
			if(!m_enabled)
			{
				return true;
			}
			
			const auto& l_mesh = m_skinned_component->get_mesh();

			l_mesh.calculate_skinned_mesh_collider_transforms
			(
				m_skinned_component->get_model_transforms(),
				m_skinned_component->get_collider_model_transforms()
			);

			m_actor.add_event(bc_bound_box_changed_actor_event(m_model_to_skinned_job->get_bound_box()));
			m_actor.add_event(bc_hierarchy_transform_actor_event(m_skinned_component->get_collider_model_transforms()));

			return true;
		}
	}	
}