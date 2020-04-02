// [01/09/2020 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix4f.h"
#include "Game/Object/Scene/ActorComponent/bcActorEvent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_actor_event_world_transform : public bc_actor_event
		{
			BC_EVENT(a_w_t)

		public:
			explicit bc_actor_event_world_transform(const core::bc_vector3f& p_translation);
			
			explicit bc_actor_event_world_transform(const core::bc_matrix4f& p_transform);

			bc_actor_event_world_transform(const bc_actor_event_world_transform& p_other);

			~bc_actor_event_world_transform();

			bc_actor_event_world_transform& operator=(const bc_actor_event_world_transform& p_other);

			const core::bc_matrix4f& get_transform() const noexcept;

		private:
			core::bc_matrix4f m_transform;
		};

		inline bc_actor_event_world_transform::bc_actor_event_world_transform(const core::bc_vector3f& p_translation)
			: bc_actor_event(event_name()),
			m_transform()
		{
			m_transform.translate(p_translation);
		}

		inline bc_actor_event_world_transform::bc_actor_event_world_transform(const core::bc_matrix4f& p_transform)
			: bc_actor_event(event_name()),
			m_transform(p_transform)
		{
		}

		inline bc_actor_event_world_transform::bc_actor_event_world_transform(const bc_actor_event_world_transform& p_other) = default;

		inline bc_actor_event_world_transform::~bc_actor_event_world_transform() = default;

		inline bc_actor_event_world_transform& bc_actor_event_world_transform::operator=(const bc_actor_event_world_transform& p_other) = default;

		inline const core::bc_matrix4f& bc_actor_event_world_transform::get_transform() const noexcept
		{
			return m_transform;
		}
	}
}