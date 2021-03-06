// [04/02/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"

namespace black_cat
{
	namespace game
	{
		void bc_actor_component_manager::process_actor_events(const core_platform::bc_clock::update_param& p_clock)
		{
			core::bc_vector_frame<_bc_actor_component_entry*> l_components_with_event;

			l_components_with_event.reserve(m_components.size());

			for (auto& l_entry : m_components)
			{
				if (l_entry.second.m_is_abstract)
				{
					continue;
				}

				if (l_entry.second.m_require_event)
				{
					l_components_with_event.push_back(&l_entry.second);
				}
			}

			std::sort
			(
				std::begin(l_components_with_event),
				std::end(l_components_with_event),
				[](const _bc_actor_component_entry* p_first, const _bc_actor_component_entry* p_second)
				{
					return p_first->m_component_priority < p_second->m_component_priority;
				}
			);

			do
			{
				m_read_event_pool = m_write_event_pool;
				m_write_event_pool = (m_write_event_pool + 1) % 2;

				if (!m_event_pools[m_read_event_pool].size())
				{
					break;
				}

				core::bc_concurrency::concurrent_for_each
				(
					std::begin(l_components_with_event),
					std::end(l_components_with_event),
					[&](_bc_actor_component_entry* p_entry)
					{
						p_entry->m_container->handle_events(m_query_manager, m_game_system, *this);
					}
				);

				{
					core_platform::bc_shared_mutex_shared_guard l_actors_lock(m_actors_lock);

					auto l_actor_indices = m_actors_bit.find_true_indices();
					core::bc_concurrency::concurrent_for_each
					(
						std::begin(l_actor_indices),
						std::end(l_actor_indices),
						[&](decltype(l_actor_indices)::value_type p_actor_index)
						{
							_clear_actor_events(p_actor_index);
						}
					);
				}
			} while (m_event_pools[m_write_event_pool].size());
		}

		void bc_actor_component_manager::update_actors(const core_platform::bc_clock::update_param& p_clock)
		{
			core::bc_vector_frame<_bc_actor_component_entry*> l_components_with_update;
			
			l_components_with_update.reserve(m_components.size());

			for (auto& l_entry : m_components)
			{
				if (l_entry.second.m_is_abstract)
				{
					continue;
				}

				if (l_entry.second.m_require_update)
				{
					l_components_with_update.push_back(&l_entry.second);
				}
			}

			std::sort
			(
				std::begin(l_components_with_update),
				std::end(l_components_with_update),
				[](const _bc_actor_component_entry* p_first, const _bc_actor_component_entry* p_second)
				{
					return p_first->m_component_priority < p_second->m_component_priority;
				}
			);

			core::bc_concurrency::concurrent_for_each
			(
				std::begin(l_components_with_update),
				std::end(l_components_with_update),
				[&](_bc_actor_component_entry* p_entry)
				{
					p_entry->m_container->update(m_query_manager, m_game_system, *this, p_clock);
				}
			);
		}

		core::bc_task<void> bc_actor_component_manager::process_actor_events_async(const core_platform::bc_clock::update_param& p_clock)
		{
			auto l_task = core::bc_concurrency::start_task
			(
				core::bc_delegate< void() >
				(
					[=, &p_clock]()
					{
						process_actor_events(p_clock);
					}
				)
			);

			return l_task;
		}

		core::bc_task<void> bc_actor_component_manager::update_actors_async(const core_platform::bc_clock::update_param& p_clock)
		{
			auto l_task = core::bc_concurrency::start_task
			(
				core::bc_delegate< void() >
				(
					[=, &p_clock]()
					{
						update_actors(p_clock);
					}
				)
			);

			return l_task;
		}

		void bc_actor_component_manager::double_update_actors(const core_platform::bc_clock::update_param& p_clock)
		{
			const auto l_read_event_pool = m_read_event_pool;
			const auto l_write_event_pool = m_write_event_pool;
			
			bc_actor l_actor;
			core::bc_vector<bci_actor_component*> l_components;

			while (m_double_update_actors.pop(l_actor))
			{
				actor_get_components(l_actor, std::back_inserter(l_components));

				do
				{
					m_read_event_pool = m_write_event_pool;
					m_write_event_pool = (m_write_event_pool + 1) % 2;

					auto* l_events = actor_get_events(l_actor);

					for (bci_actor_component* l_component : l_components)
					{
						bc_actor_event* l_current_event = l_events;

						while (l_current_event)
						{
							l_component->handle_event(bc_actor_component_event_context(m_query_manager, m_game_system, l_actor, *l_current_event));
							l_current_event = l_current_event->get_next();
						}
					}

					_clear_actor_events(l_actor.get_index());
				}
				while (m_event_pools[m_write_event_pool].size());

				for (bci_actor_component* l_component : l_components)
				{
					l_component->update(bc_actor_component_update_content(p_clock, m_query_manager, m_game_system, l_actor));
				}

				l_components.clear();
			}

			m_read_event_pool = l_read_event_pool;
			m_write_event_pool = l_write_event_pool;
		}
	}	
}