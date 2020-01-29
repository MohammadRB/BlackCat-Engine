// [05/17/2016 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcVector.h"
#include "Core/Utility/bcNullable.h"
#include "Core/Utility/bcObjectStackPool.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "PlatformImp/bcIDELogger.h"

namespace black_cat
{
	namespace game
	{
		class bc_actor_component_manager;

		class bc_iactor_component_container
		{
		public:
			virtual ~bc_iactor_component_container() = default;

			virtual bc_iactor_component* get(bc_actor_component_index p_index) = 0;

			virtual bc_actor_component_index create() = 0;

			virtual bc_actor_component_index create_after(bc_actor_component_index p_parent_index = bc_iactor_component::invalid_index) = 0;

			virtual void remove(bc_actor_component_index p_index) = 0;

			virtual void handle_events(bc_actor_component_manager& p_manager, core::bc_concurrent_object_stack_pool* p_events_pool) = 0;

			virtual void update(bc_actor_component_manager& p_manager, const core_platform::bc_clock::update_param& p_clock_update_param) = 0;

			virtual bcSIZE size() = 0;

			virtual bcSIZE capacity() = 0;

		protected:
			bc_iactor_component_container() = default;

		private:
		};

		template< class TComponent >
		class bc_actor_component_container : public bc_iactor_component_container
		{
		public:
			bc_actor_component_container();

			bc_actor_component_container(bc_actor_component_container&&) noexcept;

			~bc_actor_component_container();

			bc_actor_component_container& operator=(bc_actor_component_container&&) noexcept;

			bc_iactor_component* get(bc_actor_component_index p_index) override;

			bc_actor_component_index create() override;

			// In case of parent child relative between actors, component of child actor must be inserted after
			// component of parent actor so it will be updated after parent component
			bc_actor_component_index create_after(bc_actor_component_index p_parent_index) override;

			void remove(bc_actor_component_index p_index) override;

			void handle_events(bc_actor_component_manager& p_manager, core::bc_concurrent_object_stack_pool* p_events_pool) override;

			void update(bc_actor_component_manager& p_manager, const core_platform::bc_clock::update_param& p_clock_update_param) override;

			bcSIZE size() override;

			bcSIZE capacity() override;

		private:
			core::bc_vector< core::bc_nullable< TComponent > > m_components;
		};

		template< class TComponent >
		bc_actor_component_container<TComponent>::bc_actor_component_container()
		{
			static_assert(std::is_base_of< bc_iactor_component, TComponent >::value, "TComponent must inherit from bc_iactor_component");
		}

		template< class TComponent >
		bc_actor_component_container<TComponent>::bc_actor_component_container(bc_actor_component_container&&) noexcept = default;

		template< class TComponent >
		bc_actor_component_container<TComponent>::~bc_actor_component_container() = default;

		template< class TComponent >
		bc_actor_component_container<TComponent>& bc_actor_component_container<TComponent>::operator=(bc_actor_component_container&&) noexcept = default;

		template< class TComponent >
		bc_iactor_component* bc_actor_component_container<TComponent>::get(bc_actor_component_index p_index)
		{
			auto& l_entry = m_components[p_index];

			if(!l_entry.is_set())
			{
				return nullptr;
			}

			return &l_entry.get();
		}

		template< class TComponent >
		bc_actor_component_index bc_actor_component_container<TComponent>::create()
		{
			bc_actor_component_index l_index = 0;
			bool l_has_set = false;

			for (core::bc_nullable< TComponent >& l_component : m_components) // TODO use fast free space lookup
			{
				if (!l_component.is_set())
				{
					l_component.reset(TComponent(l_index));
					l_has_set = true;
					break;
				}
				else
				{
					++l_index;
				}
			}

			if (!l_has_set)
			{
				m_components.push_back(core::bc_nullable< TComponent >(TComponent(l_index)));
			}

			return l_index;
		}

		template< class TComponent >
		bc_actor_component_index bc_actor_component_container<TComponent>::create_after(bc_actor_component_index p_parent_index)
		{
			bc_actor_component_index l_index = 0;
			bool l_has_set = false;

			auto l_begin = std::begin(m_components) + p_parent_index + 1;
			auto l_end = std::end(m_components);
			for (; l_begin != l_end; ++l_begin)
			{
				core::bc_nullable< TComponent >& l_component = *l_begin;

				if (!l_component.is_set())
				{
					l_component.reset(TComponent(l_index));
					l_has_set = true;
				}
				else
				{
					++l_index;
				}
			}

			if (!l_has_set)
			{
				m_components.push_back(core::bc_nullable< TComponent >(TComponent(l_index)));
			}

			return l_index;
		}

		template< class TComponent >
		void bc_actor_component_container<TComponent>::remove(bc_actor_component_index p_index)
		{
			m_components[p_index].reset();
		}

		template <class TComponent>
		void bc_actor_component_container<TComponent>::handle_events(bc_actor_component_manager& p_manager, core::bc_concurrent_object_stack_pool* p_events_pool)
		{
			for (auto& l_component_entry : m_components)
			{
				if (l_component_entry.is_set()) // TODO find a way to skip this if
				{
					TComponent& l_component = l_component_entry.get();
					bc_actor l_actor = p_manager.component_get_actor< TComponent >(l_component);
					bc_actor_event* l_events = p_manager.actor_get_events(l_actor);
					bc_actor_event* l_current_event = l_events;

					while (l_current_event)
					{
						l_component.handle_event(l_actor, *l_current_event);
						l_current_event = l_current_event->get_next();
					}
					
					if(p_events_pool)
					{
						l_current_event = l_events;

						while (l_current_event)
						{
							bc_actor_event* l_next = l_current_event->get_next();
							p_events_pool->free(l_current_event);
							l_current_event = l_next;
						}

						p_manager.actor_clear_events(l_actor);
					}
				}
			}
		}

		template< class TComponent >
		void bc_actor_component_container<TComponent>::update(bc_actor_component_manager& p_manager, const core_platform::bc_clock::update_param& p_clock_update_param)
		{
			for(auto& l_component : m_components)
			{
				if(l_component.is_set()) // TODO find a way to skip this if
				{
					bc_actor l_actor = p_manager.component_get_actor< TComponent >(l_component.get());
					
					l_component->update(l_actor, p_clock_update_param);
				}
			}
		}

		template< class TComponent >
		bcSIZE bc_actor_component_container<TComponent>::size()
		{
			return m_components.size();
		}

		template< class TComponent >
		bcSIZE bc_actor_component_container<TComponent>::capacity()
		{
			return m_components.capacity();
		}
	}
}
