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
		enum class bc_transform_event_type
		{
			teleport,
			physics,
			network
		};
		
		class BC_GAME_DLL bc_world_transform_actor_event : public bc_actor_event
		{
			BC_EVENT(a_w_t)

		public:
			explicit bc_world_transform_actor_event(const core::bc_vector3f& p_translation, bc_transform_event_type p_transform_type = bc_transform_event_type::teleport);
			
			explicit bc_world_transform_actor_event(const core::bc_matrix4f& p_transform, bc_transform_event_type p_transform_type = bc_transform_event_type::teleport);

			bc_world_transform_actor_event(const bc_world_transform_actor_event& p_other);

			~bc_world_transform_actor_event() override;

			bc_world_transform_actor_event& operator=(const bc_world_transform_actor_event& p_other);

			const core::bc_matrix4f& get_transform() const noexcept;

			bc_transform_event_type get_transform_type() const noexcept;
			
		private:
			core::bc_matrix4f m_transform;
			bc_transform_event_type m_transform_type;
		};

		inline bc_world_transform_actor_event::bc_world_transform_actor_event(const core::bc_vector3f& p_translation, bc_transform_event_type p_transform_type)
			: bc_actor_event(message_name()),
			m_transform(),
			m_transform_type(p_transform_type)
		{
			m_transform.translate(p_translation);
		}

		inline bc_world_transform_actor_event::bc_world_transform_actor_event(const core::bc_matrix4f& p_transform, bc_transform_event_type p_transform_type)
			: bc_actor_event(message_name()),
			m_transform(p_transform),
			m_transform_type(p_transform_type)
		{
		}

		inline bc_world_transform_actor_event::bc_world_transform_actor_event(const bc_world_transform_actor_event& p_other) = default;

		inline bc_world_transform_actor_event::~bc_world_transform_actor_event() = default;

		inline bc_world_transform_actor_event& bc_world_transform_actor_event::operator=(const bc_world_transform_actor_event& p_other) = default;

		inline const core::bc_matrix4f& bc_world_transform_actor_event::get_transform() const noexcept
		{
			return m_transform;
		}

		inline bc_transform_event_type bc_world_transform_actor_event::get_transform_type() const noexcept
		{
			return m_transform_type;
		}
	}
}