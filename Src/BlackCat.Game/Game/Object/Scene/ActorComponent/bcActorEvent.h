// [01/06/2020 MRB]

#pragma once

#include "Core/Messaging/Event/bcEvent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		/**
		* \brief Base class for actor events
		*/
		class BC_GAME_DLL bc_actor_event : public core::bc_event
		{
		public:
			virtual ~bc_actor_event() = 0;

			bc_actor_event* get_next() const noexcept;

			void set_next(bc_actor_event* p_event) noexcept;

		protected:
			bc_actor_event(const bcCHAR* p_name, bc_actor_event* p_next = nullptr) noexcept;

			bc_actor_event(const bc_actor_event& p_other) noexcept;

			bc_actor_event& operator=(const bc_actor_event& p_other) noexcept;

		private:
			bc_actor_event* m_next;
		};

		inline bc_actor_event::~bc_actor_event() = default;

		inline bc_actor_event* bc_actor_event::get_next() const noexcept
		{
			return m_next;
		}

		inline void bc_actor_event::set_next(bc_actor_event* p_event) noexcept
		{
			m_next = p_event;
		}

		inline bc_actor_event::bc_actor_event(const bcCHAR* p_name, bc_actor_event* p_next) noexcept
			: core::bc_event(p_name),
			m_next(p_next)
		{
		}

		inline bc_actor_event::bc_actor_event(const bc_actor_event& p_other) noexcept
			: bc_event(p_other),
			m_next(p_other.m_next)
		{
		}

		inline bc_actor_event& bc_actor_event::operator=(const bc_actor_event& p_other) noexcept
		{
			bc_event::operator=(p_other);
			m_next = p_other.m_next;
			return *this;
		}
	}
}