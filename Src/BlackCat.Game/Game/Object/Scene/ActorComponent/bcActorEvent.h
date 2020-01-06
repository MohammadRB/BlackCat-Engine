// [01/06/2020 MRB]

#pragma once

#include "Core/Event/bcEvent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_actor_event : public core::bc_event
		{
		public:
			virtual ~bc_actor_event() = 0;

			bc_actor_event* get_next() const;

		protected:
			bc_actor_event(const bcCHAR* p_name, bc_actor_event* p_next);

			bc_actor_event(const bc_actor_event& p_other) = default;

			bc_actor_event& operator=(const bc_actor_event& p_other) = default;

		private:
			bc_actor_event* p_next;
		};

		inline bc_actor_event::bc_actor_event(const bcCHAR* p_name, bc_actor_event* p_next)
			: core::bc_event(p_name),
			p_next(p_next)
		{
		}

		inline bc_actor_event::~bc_actor_event()
		{
		}

		inline bc_actor_event* bc_actor_event::get_next() const
		{
			return p_next;
		}
	}
}