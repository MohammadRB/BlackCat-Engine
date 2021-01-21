// [01/17/2021 MRB]

#pragma once

#include "Game/Object/Mesh/bcSubMeshTransform.h"
#include "Game/Object/Scene/ActorComponent/bcActorEvent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_actor_event_hierarchy_transform : public bc_actor_event
		{
			BC_EVENT(a_h_t)

		public:
			explicit bc_actor_event_hierarchy_transform(const bc_sub_mesh_transform& p_transforms);

			bc_actor_event_hierarchy_transform(const bc_actor_event_hierarchy_transform& p_other);

			~bc_actor_event_hierarchy_transform();

			bc_actor_event_hierarchy_transform& operator=(const bc_actor_event_hierarchy_transform& p_other);

			const bc_sub_mesh_transform& get_transforms() const noexcept;

		private:
			const bc_sub_mesh_transform* m_transforms;
		};

		inline bc_actor_event_hierarchy_transform::bc_actor_event_hierarchy_transform(const bc_sub_mesh_transform& p_transforms)
			: bc_actor_event(message_name()),
			m_transforms(&p_transforms)
		{
		}
		
		inline bc_actor_event_hierarchy_transform::bc_actor_event_hierarchy_transform(const bc_actor_event_hierarchy_transform& p_other) = default;

		inline bc_actor_event_hierarchy_transform::~bc_actor_event_hierarchy_transform() = default;

		inline bc_actor_event_hierarchy_transform& bc_actor_event_hierarchy_transform::operator=(const bc_actor_event_hierarchy_transform& p_other) = default;

		inline const bc_sub_mesh_transform& bc_actor_event_hierarchy_transform::get_transforms() const noexcept
		{
			return *m_transforms;
		}
	}
}