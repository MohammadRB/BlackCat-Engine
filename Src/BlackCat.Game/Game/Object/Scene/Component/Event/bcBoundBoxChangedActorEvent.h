// [15/01/2020 MRB]

#pragma once

#include "PhysicsImp/Shape/bcBoundBox.h"
#include "Game/Object/Scene/ActorComponent/bcActorEvent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_bound_box_changed_actor_event : public bc_actor_event
		{
			BC_EVENT(a_bb_c)
			
		public:
			explicit bc_bound_box_changed_actor_event(const physics::bc_bound_box& p_bound_box);

			bc_bound_box_changed_actor_event(const bc_bound_box_changed_actor_event&);

			~bc_bound_box_changed_actor_event() override;

			bc_bound_box_changed_actor_event& operator=(const bc_bound_box_changed_actor_event&);

			const physics::bc_bound_box& get_bound_box() const noexcept;

		private:
			physics::bc_bound_box m_bound_box;
		};

		inline bc_bound_box_changed_actor_event::bc_bound_box_changed_actor_event(const physics::bc_bound_box& p_bound_box)
			: bc_actor_event(message_name()),
			m_bound_box(p_bound_box)
		{
		}

		inline bc_bound_box_changed_actor_event::bc_bound_box_changed_actor_event(const bc_bound_box_changed_actor_event&) = default;

		inline bc_bound_box_changed_actor_event::~bc_bound_box_changed_actor_event() = default;

		inline bc_bound_box_changed_actor_event& bc_bound_box_changed_actor_event::operator=(const bc_bound_box_changed_actor_event&) = default;

		inline const physics::bc_bound_box& bc_bound_box_changed_actor_event::get_bound_box() const noexcept
		{
			return m_bound_box;
		}
	}
}