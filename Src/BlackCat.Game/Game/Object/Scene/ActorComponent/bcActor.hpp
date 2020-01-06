// [05/20/2016 MRB]

// This file is used to break dependencies between bcActor.h and bcActorComponentManager.h

#pragma once

#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"

namespace black_cat
{
	namespace game
	{
		inline bc_actor::bc_actor()
			: m_index(invalid_index)
		{
		}

		inline bc_actor::bc_actor(bc_actor_index p_index)
			: m_index(p_index)
		{
		}

		inline bc_actor::bc_actor(const bc_actor&) noexcept = default;

		inline bc_actor::~bc_actor() = default;

		inline bc_actor& bc_actor::operator=(const bc_actor&) noexcept = default;

		inline bc_actor_index bc_actor::get_index() const noexcept
		{
			return m_index;
		}

		template< class TComponent >
		void bc_actor::create_component()
		{
			_get_manager()->create_component< TComponent >(*this);
		}

		template< class TComponent >
		void bc_actor::remove_component()
		{
			_get_manager()->remove_component< TComponent >(*this);
		}

		template< class TComponent >
		bool bc_actor::has_component() const
		{
			return _get_manager()->actor_has_component< TComponent >(*this);
		}

		template< class TComponent >
		TComponent* bc_actor::get_component()
		{
			return _get_manager()->actor_get_component< TComponent >(*this);
		}

		template< class TComponent >
		const TComponent* bc_actor::get_component() const
		{
			return const_cast<bc_actor*>(this)->get_component<TComponent>();
		}

		template< class TIterator >
		void bc_actor::get_components(TIterator p_destination) const
		{
			_get_manager()->actor_get_components(*this, p_destination);
		}

		inline void bc_actor::destroy()
		{
			_get_manager()->remove_actor(*this);
			m_index = invalid_index;
		}

		inline bool bc_actor::operator==(const bc_actor& p_other) const noexcept
		{
			return m_index == p_other.m_index;
		}

		inline bool bc_actor::operator!=(const bc_actor& p_other) const noexcept
		{
			return m_index != p_other.m_index;
		}

		inline bc_actor_component_manager* bc_actor::_get_manager() noexcept
		{
			static bc_actor_component_manager* s_manager = core::bc_get_service< bc_actor_component_manager >();

			return s_manager;
		}
	}
}