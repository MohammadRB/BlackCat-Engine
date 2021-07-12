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
		inline bc_actor::bc_actor()
			: m_index(invalid_id)
		{
		}

		inline bc_actor::bc_actor(bc_actor_id p_index)
			: m_index(p_index)
		{
		}

		inline bc_actor::bc_actor(const bc_actor&) noexcept = default;

		inline bc_actor::~bc_actor() = default;

		inline bc_actor& bc_actor::operator=(const bc_actor&) noexcept = default;

		inline bc_actor_id bc_actor::get_id() const noexcept
		{
			return m_index;
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
		TComponent* bc_actor::get_create_component() noexcept
		{
			auto* l_component = get_component<TComponent>();
			if(!l_component)
			{
				create_component<TComponent>();
				l_component = get_component<TComponent>();
			}

			return l_component;
		}

		template<class TComponent>
		const TComponent* bc_actor::get_component() const noexcept
		{
			return const_cast<bc_actor*>(this)->get_component<TComponent>();
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
			
			_get_entity_manager().remove_entity(*this);
			m_index = invalid_id;
		}

		inline bool bc_actor::is_valid() const noexcept
		{
			return m_index != invalid_id;
		}

		inline bool bc_actor::operator==(const bc_actor& p_other) const noexcept
		{
			return m_index == p_other.m_index;
		}

		inline bool bc_actor::operator!=(const bc_actor& p_other) const noexcept
		{
			return m_index != p_other.m_index;
		}

		inline bc_actor_component_manager& bc_actor::_get_manager() noexcept
		{
			static auto* s_manager = core::bc_get_service<bc_actor_component_manager>();
			return *s_manager;
		}

		inline bc_entity_manager& bc_actor::_get_entity_manager() noexcept
		{
			static auto* s_manager = core::bc_get_service<bc_entity_manager>();
			return *s_manager;
		}

		inline void bc_actor_load_instance(core::bc_vector_frame<bci_actor_component*>& p_buffer, const bc_actor_component_load_context& p_context)
		{
			p_buffer.clear();
			p_context.m_actor.get_components(std::back_inserter(p_buffer));

			for(auto* l_component : p_buffer)
			{
				l_component->load_instance(p_context);
			}
		}

		inline void bc_actor_write_instance(core::bc_vector_frame<bci_actor_component*>& p_buffer, const bc_actor_component_write_context& p_context)
		{
			p_buffer.clear();
			p_context.m_actor.get_components(std::back_inserter(p_buffer));

			for (auto* l_component : p_buffer)
			{
				l_component->write_instance(p_context);
			}
		}

		inline void bc_actor_load_network_instance(core::bc_vector_frame<bci_actor_component*>& p_buffer, const bc_actor_component_network_load_context& p_context)
		{
			p_buffer.clear();
			p_context.m_actor.get_components(std::back_inserter(p_buffer));

			for (auto* l_component : p_buffer)
			{
				l_component->load_network_instance(p_context);
			}
		}

		inline void bc_actor_write_network_instance(core::bc_vector_frame<bci_actor_component*>& p_buffer, const bc_actor_component_network_write_context& p_context)
		{
			p_buffer.clear();
			p_context.m_actor.get_components(std::back_inserter(p_buffer));

			for (auto* l_component : p_buffer)
			{
				l_component->write_network_instance(p_context);
			}
		}
	}
}