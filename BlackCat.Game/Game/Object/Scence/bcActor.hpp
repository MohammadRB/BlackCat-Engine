// [05/20/2016 MRB]

// This file is used to break dependencies between bcActor.h and bcActorComponentManager.h

#pragma once

#include "Game/Object/Scence/bcActor.h"
#include "Game/Object/Scence/bcActorComponentManager.h"

namespace black_cat
{
	namespace game
	{
		inline bc_actor::bc_actor()
			: m_manager(nullptr),
			m_index(g_actor_invalid_index)
		{
		}

		inline bc_actor::bc_actor(const bc_actor&) noexcept = default;

		inline bc_actor::~bc_actor() = default;

		inline bc_actor& bc_actor::operator=(const bc_actor&) noexcept = default;

		template< class TComponent >
		void bc_actor::create_component()
		{
			m_manager->create_component< TComponent >(*this);
		}

		template< class TComponent >
		void bc_actor::remove_component()
		{
			m_manager->remove_component< TComponent >(*this);
		}

		template< class TComponent >
		bool bc_actor::has_component() const
		{
			return m_manager->actor_has_component< TComponent >(*this);
		}

		template< class TComponent >
		TComponent* bc_actor::get_component()
		{
			return m_manager->actor_get_component< TComponent >(*this);
		}

		template< class TComponent >
		const TComponent* bc_actor::get_component() const
		{
			return const_cast<bc_actor*>(this)->get_component<TComponent>();
		}

		inline void bc_actor::destroy()
		{
			m_manager->remove_actor(*this);
			m_index = g_actor_invalid_index;
		}

		inline bool bc_actor::operator==(const bc_actor& p_other) const noexcept
		{
			return m_index == p_other.m_index;
		}

		inline bool bc_actor::operator!=(const bc_actor& p_other) const noexcept
		{
			return m_index != p_other.m_index;
		}

		inline bc_actor::bc_actor(bc_actor_component_manager* p_manager, bc_actor_index p_index)
			: m_manager(p_manager),
			m_index(p_index)
		{
		}

		inline bc_actor_index bc_actor::get_index() const noexcept
		{
			return m_index;
		}
	}
}