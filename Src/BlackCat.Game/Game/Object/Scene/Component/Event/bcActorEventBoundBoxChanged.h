// [01/15/2020 MRB]

#pragma once

#include "PhysicsImp/Shape/bcBoundBox.h"
#include "Game/Object/Scene/ActorComponent/bcActorEvent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_actor_event_bound_box_changed : public bc_actor_event
		{
			BC_EVENT(a_bb_c)
			
		public:
			explicit bc_actor_event_bound_box_changed(const physics::bc_bound_box& p_bound_box);

			bc_actor_event_bound_box_changed(const bc_actor_event_bound_box_changed&);

			~bc_actor_event_bound_box_changed();

			bc_actor_event_bound_box_changed& operator=(const bc_actor_event_bound_box_changed&);

			const physics::bc_bound_box& get_bound_box() const noexcept;

		private:
			physics::bc_bound_box m_bound_box;
		};

		inline bc_actor_event_bound_box_changed::bc_actor_event_bound_box_changed(const physics::bc_bound_box& p_bound_box)
			: bc_actor_event(message_name()),
			m_bound_box(p_bound_box)
		{
		}

		inline bc_actor_event_bound_box_changed::bc_actor_event_bound_box_changed(const bc_actor_event_bound_box_changed&) = default;

		inline bc_actor_event_bound_box_changed::~bc_actor_event_bound_box_changed() = default;

		inline bc_actor_event_bound_box_changed& bc_actor_event_bound_box_changed::operator=(const bc_actor_event_bound_box_changed&) = default;

		inline const physics::bc_bound_box& bc_actor_event_bound_box_changed::get_bound_box() const noexcept
		{
			return m_bound_box;
		}
	}
}