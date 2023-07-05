// [06/01/2020 MRB]

#pragma once

#include "Core/Messaging/Event/bcEvent.h"
#include "Game/bcExport.h"

namespace black_cat::game
{
	/**
		* \brief Base class for actor events
		*/
	class BC_GAME_DLL bc_actor_event : public core::bc_event
	{
	public:
		~bc_actor_event() override = 0;

		bc_actor_event* get_next() const noexcept;

		void set_next(bc_actor_event* p_event) noexcept;

	protected:
		explicit bc_actor_event(const bcCHAR* p_name) noexcept;

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

	inline bc_actor_event::bc_actor_event(const bcCHAR* p_name) noexcept
		: core::bc_event(p_name),
		  m_next(nullptr)
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
