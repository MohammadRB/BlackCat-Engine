// [10/02/2021 MRB]

#pragma once

#include "Game/Object/Scene/ActorComponent/bcActorEvent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_rigid_dynamic_sleep_actor_event : public bc_actor_event
		{
			BC_EVENT(r_d_s)
			
		public:
			explicit bc_rigid_dynamic_sleep_actor_event(bool p_is_sleep) noexcept;

			bc_rigid_dynamic_sleep_actor_event(bc_rigid_dynamic_sleep_actor_event&&) noexcept;

			~bc_rigid_dynamic_sleep_actor_event() override;

			bc_rigid_dynamic_sleep_actor_event& operator=(bc_rigid_dynamic_sleep_actor_event&&) noexcept;

			bool get_is_sleep() const noexcept;

		private:
			bool m_is_sleep;
		};

		inline bc_rigid_dynamic_sleep_actor_event::bc_rigid_dynamic_sleep_actor_event(bool p_is_sleep) noexcept
			: bc_actor_event(message_name()),
			m_is_sleep(p_is_sleep)
		{
		}

		inline bc_rigid_dynamic_sleep_actor_event::bc_rigid_dynamic_sleep_actor_event(bc_rigid_dynamic_sleep_actor_event&&) noexcept = default;

		inline bc_rigid_dynamic_sleep_actor_event::~bc_rigid_dynamic_sleep_actor_event() = default;

		inline bc_rigid_dynamic_sleep_actor_event& bc_rigid_dynamic_sleep_actor_event::operator=(bc_rigid_dynamic_sleep_actor_event&&) noexcept = default;

		inline bool bc_rigid_dynamic_sleep_actor_event::get_is_sleep() const noexcept
		{
			return m_is_sleep;
		}
	}	
}