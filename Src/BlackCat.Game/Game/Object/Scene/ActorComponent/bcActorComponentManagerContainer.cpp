// [06/05/2022 MRB]

#include "Game/GamePCH.h"

#include "Core/Concurrency/bcThreadManager.h"
#include "Core/Concurrency/bcConcurrency.h"
#include "Core/Utility/bcLogger.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManagerContainer.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentContainer.hpp"

namespace black_cat
{
	namespace game
	{
		struct _bc_component_to_update
		{
			bci_actor_component_container* m_container;
			core::bc_vector_frame<bcUINT32> m_alive_indices;
		};

		bc_actor_component_manager_container::bc_actor_component_manager_container() noexcept
			: m_query_manager(nullptr),
			m_game_system(nullptr),
			m_my_index(static_cast<bcUINT32>(-1)),
			m_events_pool_capacity(0),
			m_read_event_pool(0U),
			m_write_event_pool(0U)
		{
		}

		bc_actor_component_manager_container::bc_actor_component_manager_container(core::bc_query_manager& p_query_manager, 
			bc_game_system& p_game_system, 
			const bc_actor_component_manager& p_manager,
			bcUINT32 p_my_index)
			: m_query_manager(&p_query_manager),
			m_game_system(&p_game_system),
			m_my_index(p_my_index),
			m_events_pool_capacity(100 * static_cast<bcSIZE>(core::bc_mem_size::kb)),
			m_read_event_pool(0U),
			m_write_event_pool(0U)
		{
			m_event_pools[0].initialize(m_events_pool_capacity);
			m_event_pools[1].initialize(m_events_pool_capacity);

			for(auto* l_component : p_manager.get_components())
			{
				_bc_actor_component_entry l_entry(*l_component);
				m_components.insert(std::make_pair(l_component->m_hash, std::move(l_entry)));
			}
		}

		bc_actor_component_manager_container::bc_actor_component_manager_container(bc_actor_component_manager_container&& p_other) noexcept
			: m_query_manager(p_other.m_query_manager),
			m_game_system(p_other.m_game_system),
			m_my_index(p_other.m_my_index),
			m_events_pool_capacity(p_other.m_events_pool_capacity),
			m_actors(std::move(p_other.m_actors)),
			m_actors_bit(std::move(p_other.m_actors_bit)),
			m_double_update_actors(std::move(p_other.m_double_update_actors)),
			m_components(std::move(p_other.m_components)),
			m_read_event_pool(p_other.m_read_event_pool),
			m_write_event_pool(p_other.m_write_event_pool),
			m_event_pools{ std::move(p_other.m_event_pools[0]), std::move(p_other.m_event_pools[1]) }
		{
			p_other.m_my_index = -1;
		}

		bc_actor_component_manager_container::~bc_actor_component_manager_container()
		{
			const auto l_active_actors = m_actors_bit.find_true_indices();
			BC_ASSERT(l_active_actors.empty());

			m_my_index = -1;
			m_event_pools[0].destroy();
			m_event_pools[1].destroy();
		}

		bc_actor_component_manager_container& bc_actor_component_manager_container::operator=(bc_actor_component_manager_container&& p_other) noexcept
		{
			m_query_manager = p_other.m_query_manager;
			m_game_system = p_other.m_game_system;
			m_my_index = p_other.m_my_index;
			m_events_pool_capacity = p_other.m_events_pool_capacity;
			m_actors = std::move(p_other.m_actors);
			m_actors_bit = std::move(p_other.m_actors_bit);
			m_double_update_actors = std::move(p_other.m_double_update_actors);
			m_components = std::move(p_other.m_components);
			m_read_event_pool = p_other.m_read_event_pool;
			m_write_event_pool = p_other.m_write_event_pool;
			m_event_pools[0] = std::move(p_other.m_event_pools[0]);
			m_event_pools[1] = std::move(p_other.m_event_pools[1]);

			p_other.m_my_index = -1;
			return *this;
		}

		void bc_actor_component_manager_container::process_actor_events(const platform::bc_clock::update_param& p_clock)
		{
			core::bc_vector_frame<_bc_component_to_update> l_components_with_event;
			l_components_with_event.reserve(m_components.size());

			// Make a copy of actor and component alive indices for two reasons
			// First, only update actors which pre-exist because during update new actors can be created
			// Second, update is done concurrently and while these indices are read another thread may modify them
			auto l_actor_indices = m_actors_bit.find_true_indices();

			for (auto& l_entry : m_components)
			{
				if (l_entry.second.m_is_abstract)
				{
					continue;
				}

				if (l_entry.second.m_require_event)
				{
					l_components_with_event.push_back({ l_entry.second.m_container.get(), l_entry.second.m_container->export_alive_indices() });
				}
			}

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
					[&](const _bc_component_to_update& p_entry)
					{
						p_entry.m_container->handle_events(p_clock, *m_query_manager, *m_game_system, *this, p_entry.m_alive_indices);
					}
				);

				{
					platform::bc_shared_mutex_shared_guard l_actors_lock(m_actors_lock);

					core::bc_concurrency::concurrent_for_each
					(
						std::begin(l_actor_indices),
						std::end(l_actor_indices),
						[&](auto p_actor_index)
						{
							_clear_actor_events(p_actor_index, m_read_event_pool);
						}
					);
				}
			}
			while (m_event_pools[m_write_event_pool].size());
		}

		void bc_actor_component_manager_container::update_actors(const platform::bc_clock::update_param& p_clock)
		{
			core::bc_vector_frame<_bc_component_to_update> l_components_with_update;
			l_components_with_update.reserve(m_components.size());

			for (auto& l_entry : m_components)
			{
				if (l_entry.second.m_is_abstract)
				{
					continue;
				}

				if (l_entry.second.m_require_update)
				{
					l_components_with_update.push_back({ l_entry.second.m_container.get(),l_entry.second.m_container->export_alive_indices() });
				}
			}

			core::bc_concurrency::concurrent_for_each
			(
				std::begin(l_components_with_update),
				std::end(l_components_with_update),
				[&](const _bc_component_to_update& p_entry)
				{
					p_entry.m_container->update(p_clock, *m_query_manager, *m_game_system, *this, p_entry.m_alive_indices);
				}
			);
		}

		core::bc_task<void> bc_actor_component_manager_container::process_actor_events_async(const platform::bc_clock::update_param& p_clock)
		{
			auto l_task = core::bc_concurrency::start_task
			(
				core::bc_delegate<void()>
				(
					[=, &p_clock]()
					{
						process_actor_events(p_clock);
					}
				)
			);

			return l_task;
		}

		core::bc_task<void> bc_actor_component_manager_container::update_actors_async(const platform::bc_clock::update_param& p_clock)
		{
			auto l_task = core::bc_concurrency::start_task
			(
				core::bc_delegate<void()>
				(
					[=, &p_clock]()
					{
						update_actors(p_clock);
					}
					)
			);

			return l_task;
		}

		void bc_actor_component_manager_container::double_update_actors(const platform::bc_clock::update_param& p_clock)
		{
			const auto l_read_event_pool_backup = m_read_event_pool;
			const auto l_write_event_pool_backup = m_write_event_pool;

			bc_actor l_actor;
			core::bc_vector_frame<bci_actor_component*> l_components;
			l_components.reserve(10);

			while (m_double_update_actors.pop(l_actor))
			{
				// for each actor reset read and write indexes to read from last written pool
				m_read_event_pool = l_read_event_pool_backup;
				m_write_event_pool = l_write_event_pool_backup;

				actor_get_components(l_actor, std::back_inserter(l_components));
				bcSIZE l_write_event_pool_size;
				bcSIZE l_write_event_pool_new_size;

				do
				{
					m_read_event_pool = m_write_event_pool;
					m_write_event_pool = (m_write_event_pool + 1) % 2;
					l_write_event_pool_size = m_event_pools[m_write_event_pool].size();

					auto* l_events = actor_get_events(l_actor);

					for (bci_actor_component* l_component : l_components)
					{
						bc_actor_event* l_current_event = l_events;
						bcUINT32 l_event_index = 0;

						while (l_current_event)
						{
							l_component->handle_event(bc_actor_component_event_context(p_clock, *m_query_manager, *m_game_system, l_actor, *l_current_event, l_event_index));
							l_current_event = l_current_event->get_next();
							l_event_index++;
						}
					}

					l_write_event_pool_new_size = m_event_pools[m_write_event_pool].size();

					const auto l_actor_index = bc_actor_id::decompose_id(l_actor.get_id()).m_index;
					_clear_actor_events(l_actor_index, m_read_event_pool);
				}
				while (l_write_event_pool_new_size > l_write_event_pool_size);

				for (bci_actor_component* l_component : l_components)
				{
					l_component->update(bc_actor_component_update_content(p_clock, *m_query_manager, *m_game_system, l_actor, true));
				}

				l_components.clear();
			}

			m_read_event_pool = l_read_event_pool_backup;
			m_write_event_pool = l_write_event_pool_backup;
		}
	}
}
