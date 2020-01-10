// [01/09/2020 MRB]

#pragma once

#include "Core/Math/bcMatrix4f.h"
#include "Game/bcExport.h"
#include "Game/Object/Scene/ActorComponent/bcActorEvent.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_actor_event_world_transform : public bc_actor_event
		{
			BC_EVENT("actor_event_world_transform")

		public:
			bc_actor_event_world_transform(const core::bc_matrix4f& p_transform);

			bc_actor_event_world_transform(const bc_actor_event_world_transform& p_other) = default;

			~bc_actor_event_world_transform() = default;

			bc_actor_event_world_transform& operator=(const bc_actor_event_world_transform& p_other) = default;

		protected:

		private:
			core::bc_matrix4f m_transform;
		};

		bc_actor_event_world_transform::bc_actor_event_world_transform(const core::bc_matrix4f& p_transform)
			: bc_actor_event(event_name()),
			m_transform(p_transform)
		{
		}
	}
}