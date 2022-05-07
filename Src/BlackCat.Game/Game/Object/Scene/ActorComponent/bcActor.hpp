// [05/20/2016 MRB]

// This file is used to break dependencies between bcActor.h and bcActorComponentManager.h

#pragma once

#include "Core/Container/bcVector.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/bcEntityManager.h"

namespace black_cat
{
	namespace game
	{
		inline bc_actor::bc_actor() = default;

		inline bc_actor::bc_actor(bc_actor_id p_index)
			: m_id(p_index)
		{
		}

		inline bc_actor::bc_actor(const bc_actor&) noexcept = default;

		inline bc_actor::bc_actor(bc_actor&&) noexcept = default;

		inline bc_actor::~bc_actor() = default;

		inline bc_actor& bc_actor::operator=(const bc_actor&) noexcept = default;

		inline bc_actor& bc_actor::operator=(bc_actor&&) noexcept = default;

		inline bc_actor_id bc_actor::get_id() const noexcept
		{
			return m_id;
		}

		inline const bc_actor_event* bc_actor::get_events() const noexcept
		{
			return _get_manager().actor_get_events(*this);
		}
		
		template<typename TEvent>
		void bc_actor::add_event(TEvent&& p_event)
		{
			_get_manager().actor_add_event(*this, std::move(p_event));
		}

		template<class TComponent>
		TComponent* bc_actor::get_component() noexcept
		{
			return _get_manager().actor_get_component<TComponent>(*this);
		}

		template<class TComponent>
		const TComponent* bc_actor::get_component() const noexcept
		{
			return const_cast<bc_actor*>(this)->get_component<TComponent>();
		}

		template<class TComponent>
		TComponent* bc_actor::get_create_component() noexcept
		{
			auto* l_component = get_component<TComponent>();
			if (!l_component)
			{
				create_component<TComponent>();
				l_component = get_component<TComponent>();
			}

			return l_component;
		}

		template<class TIterator>
		void bc_actor::get_components(TIterator p_destination) const
		{
			_get_manager().actor_get_components(*this, p_destination);
		}

		template<class TComponent>
		bool bc_actor::has_component() const noexcept
		{
			return _get_manager().actor_has_component<TComponent>(*this);
		}

		template<class TComponent>
		void bc_actor::create_component()
		{
			_get_entity_manager().create_entity_component<TComponent>(*this);
		}

		template<class TComponent>
		void bc_actor::create_component(const core::bc_json_key_value& p_instance_parameters)
		{
			_get_entity_manager().create_entity_component<TComponent>(*this, p_instance_parameters);
		}

		template<class TComponent>
		void bc_actor::remove_component()
		{
			_get_manager().remove_component<TComponent>(*this);
		}

		inline void bc_actor::mark_for_double_update()
		{
			_get_manager().mark_actor_for_double_update(*this);
		}

		inline void bc_actor::draw_debug(bc_shape_drawer& p_shape_drawer) const
		{
			const bc_actor_component_debug_draw_context l_context(p_shape_drawer , *this);
			core::bc_vector_frame<bci_actor_component*> l_components;
			l_components.reserve(10);
			
			get_components(std::back_inserter(l_components));

			for(bci_actor_component* l_component : l_components)
			{
				l_component->debug_draw(l_context);
			}
		}

		inline void bc_actor::destroy()
		{
			BC_ASSERT(is_valid());
			
			_get_manager().remove_actor(*this);
			m_id = bc_actor_id();
		}

		inline bool bc_actor::is_valid() const noexcept
		{
			return m_id.is_valid();
		}

		inline bool bc_actor::is_valid_deep() const noexcept
		{
			return _get_manager().actor_is_valid(*this);
		}

		inline bool bc_actor::operator==(const bc_actor& p_other) const noexcept
		{
			return m_id == p_other.m_id;
		}

		inline bool bc_actor::operator!=(const bc_actor& p_other) const noexcept
		{
			return m_id != p_other.m_id;
		}
		
		inline bc_actor_component_manager_container& bc_actor::_get_manager() const noexcept
		{
			return _get_entity_manager().get_actor_component_container(*this);
		}

		inline bc_entity_manager& bc_actor::_get_entity_manager() const noexcept
		{
			static auto* s_manager = core::bc_get_service<bc_entity_manager>();
			return *s_manager;
		}
	}
}