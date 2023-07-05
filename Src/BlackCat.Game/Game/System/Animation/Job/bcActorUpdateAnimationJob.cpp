// [15/03/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/Component/bcSkinnedMeshComponent.h"
#include "Game/Object/Scene/Component/Event/bcHierarchyTransformActorEvent.h"
#include "Game/Object/Scene/Component/Event/bcBoundBoxChangedActorEvent.h"
#include "Game/System/Animation/Job/bcActorUpdateAnimationJob.h"

namespace black_cat::game
{
	bc_actor_update_animation_job::bc_actor_update_animation_job(bc_actor p_actor,
	                                                             bc_skinned_mesh_component& p_component,
	                                                             core::bc_shared_ptr<bci_skinning_transform_animation_job> p_skinning_job)
		: bci_animation_job(p_skinning_job->get_skeleton()),
		  m_actor(std::move(p_actor)),
		  m_component(&p_component),
		  m_skinning_job(std::move(p_skinning_job))
	{
	}

	bc_actor_update_animation_job::bc_actor_update_animation_job(bc_actor_update_animation_job&&) noexcept = default;

	inline bc_actor_update_animation_job::~bc_actor_update_animation_job() = default;

	inline bc_actor_update_animation_job& bc_actor_update_animation_job::operator=(bc_actor_update_animation_job&&) noexcept = default;

	inline bool bc_actor_update_animation_job::run(const platform::bc_clock::update_param& p_clock)
	{
		if(!m_enabled)
		{
			return true;
		}
			
		const auto& l_mesh = m_component->get_mesh();

		l_mesh.calculate_skinned_mesh_collider_transforms
		(
			m_component->get_model_transforms(),
			m_component->get_collider_model_transforms()
		);

		m_actor.add_event(bc_bound_box_changed_actor_event(m_skinning_job->get_bound_box()));
		m_actor.add_event(bc_hierarchy_transform_actor_event
			(
				m_skinning_job->get_world(),
				&m_component->get_model_transforms(),
				&m_component->get_collider_model_transforms()
			));

		return true;
	}
}
