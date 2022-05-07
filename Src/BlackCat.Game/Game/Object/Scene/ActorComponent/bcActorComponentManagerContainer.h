// [05/06/2022 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "Core/Concurrency/bcTask.h"
#include "Core/Container/bcBitVector.h"
#include "Core/Container/bcConcurrentQueue1.h"
#include "Core/Utility/bcObjectStackPool.h"
#include "Game/Object/Scene/ActorComponent/bcActorId.h"
#include "Game/Object/Scene/ActorComponent/bcActorEvent.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentContainer.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace core
	{
		class bc_query_manager;
	}

	namespace game
	{
		class bc_game_system;

		struct _bc_actor_entry
		{
			explicit _bc_actor_entry(bc_actor_id p_id, bc_actor_id p_parent_id = bc_actor_id());

			_bc_actor_entry(const _bc_actor_entry& p_other);

			~_bc_actor_entry() = default;

			_bc_actor_entry& operator=(const _bc_actor_entry& p_other);

			void add_event(bcUINT32 p_active_event_pool, bc_actor_event* p_event) noexcept;

			bc_actor_event* get_events(bcUINT32 p_active_event_pool) const noexcept;

			void clear_events(bcUINT32 p_active_event_pool) noexcept;

			bc_actor_id m_actor_id;
			bc_actor_id m_parent_id;
			bc_actor_event* m_events[2];
			platform::bc_atomic_flag m_lock_flag;
		};

		struct _bc_actor_component_entry : bc_actor_component_register
		{
			_bc_actor_component_entry(bc_actor_component_register p_register);

			_bc_actor_component_entry(_bc_actor_component_entry&& p_other) noexcept;

			~_bc_actor_component_entry() = default;

			_bc_actor_component_entry& operator=(_bc_actor_component_entry&& p_other) noexcept;
			
			core::bc_vector_movable<bc_actor_component_id> m_actor_to_component_index_map;
			core::bc_unique_ptr<bci_actor_component_container> m_container;
			mutable platform::bc_shared_mutex m_lock;
		};

		class BC_GAME_DLL bc_actor_component_manager_container
		{
			friend class bc_actor_component_manager;
			using actor_container_type = core::bc_vector<_bc_actor_entry>;
			using double_update_actor_container_type = core::bc_concurrent_queue1<bc_actor>;
			using component_container_type = core::bc_unordered_map_program<bc_actor_component_hash, _bc_actor_component_entry>;

		public:
			bc_actor_component_manager_container() noexcept;

			bc_actor_component_manager_container(core::bc_query_manager& p_query_manager, 
				bc_game_system& p_game_system, 
				const bc_actor_component_manager& p_manager, 
				bcUINT32 p_my_index);

			bc_actor_component_manager_container(bc_actor_component_manager_container&&) noexcept;

			~bc_actor_component_manager_container();

			bc_actor_component_manager_container& operator=(bc_actor_component_manager_container&&) noexcept;

			bc_actor create_actor(const bc_actor* p_parent = nullptr);

			void remove_actor(const bc_actor& p_actor);

			template<typename TEvent>
			void actor_add_event(const bc_actor& p_actor, TEvent&& p_event);

			bc_actor_event* actor_get_events(const bc_actor& p_actor) const noexcept;

			bool actor_is_valid(const bc_actor& p_actor) noexcept;

			void mark_actor_for_double_update(const bc_actor& p_actor);

			/**
			 * \brief Create uninitialized component for actor
			 * \tparam TComponent
			 * \param p_actor
			 */
			template<class TComponent>
			void create_component(const bc_actor& p_actor);

			template<class TComponent>
			void remove_component(const bc_actor& p_actor);

			template<class TComponent>
			bool actor_has_component(const bc_actor& p_actor) const noexcept;

			template<class TComponent>
			TComponent* actor_get_component(const bc_actor& p_actor) noexcept;

			template<class TComponent>
			const TComponent* actor_get_component(const bc_actor& p_actor) const noexcept;

			template<class TIterator>
			void actor_get_components(const bc_actor& p_actor, TIterator p_destination) const;

			template<class TComponent>
			bc_actor component_get_actor(const TComponent& p_component) const noexcept;

			void process_actor_events(const platform::bc_clock::update_param& p_clock);

			void update_actors(const platform::bc_clock::update_param& p_clock);

			core::bc_task<void> process_actor_events_async(const platform::bc_clock::update_param& p_clock);

			core::bc_task<void> update_actors_async(const platform::bc_clock::update_param& p_clock);

			void double_update_actors(const platform::bc_clock::update_param& p_clock);

		private:
			template<class TComponent>
			TComponent* _actor_get_component(const bc_actor& p_actor, std::true_type);

			template<class TComponent>
			TComponent* _actor_get_component(const bc_actor& p_actor, std::false_type);

			void _clear_actor_events(bcUINT32 p_index, bcUINT32 p_event_pool);

			template<class TComponent>
			_bc_actor_component_entry* _get_component_entry();

			template<class TComponent>
			const _bc_actor_component_entry* _get_component_entry() const;

			void _resize_actor_to_component_index_maps();

			core::bc_query_manager* m_query_manager;
			bc_game_system* m_game_system;
			bcUINT32 m_my_index;
			bcUINT32 m_events_pool_capacity;

			mutable platform::bc_shared_mutex m_actors_lock;
			actor_container_type m_actors;
			core::bc_bit_vector m_actors_bit;
			double_update_actor_container_type m_double_update_actors;
			component_container_type m_components;

			bcUINT32 m_read_event_pool;
			bcUINT32 m_write_event_pool;
			core::bc_concurrent_object_stack_pool m_event_pools[2];
		};

		inline _bc_actor_entry::_bc_actor_entry(bc_actor_id p_id, bc_actor_id p_parent_id)
			: m_actor_id(p_id),
			m_parent_id(p_parent_id),
			m_events{ nullptr, nullptr },
			m_lock_flag(false)
		{
		}

		inline _bc_actor_entry::_bc_actor_entry(const _bc_actor_entry& p_other)
			: m_actor_id(p_other.m_actor_id),
			m_parent_id(p_other.m_parent_id),
			m_events{ p_other.m_events[0], p_other.m_events[1] },
			m_lock_flag(false)
		{
		}

		inline _bc_actor_entry& _bc_actor_entry::operator=(const _bc_actor_entry& p_other)
		{
			m_actor_id = p_other.m_actor_id;
			m_parent_id = p_other.m_parent_id;
			m_events[0] = p_other.m_events[0];
			m_events[1] = p_other.m_events[1];
			return *this;
		}

		inline void _bc_actor_entry::add_event(bcUINT32 p_active_event_pool, bc_actor_event* p_event) noexcept
		{
			while (m_lock_flag.test_and_set(platform::bc_memory_order::acquire)) {}

			bc_actor_event* l_last_event = m_events[p_active_event_pool];

			if (!l_last_event)
			{
				m_events[p_active_event_pool] = p_event;

				m_lock_flag.clear(platform::bc_memory_order::release);
				return;
			}

			while (l_last_event->get_next())
			{
				l_last_event = l_last_event->get_next();
			}
			l_last_event->set_next(p_event);

			m_lock_flag.clear(platform::bc_memory_order::release);
		}

		inline bc_actor_event* _bc_actor_entry::get_events(bcUINT32 p_active_event_pool) const noexcept
		{
			return m_events[p_active_event_pool];
		}

		inline void _bc_actor_entry::clear_events(bcUINT32 p_active_event_pool) noexcept
		{
			m_events[p_active_event_pool] = nullptr;
		}

		inline _bc_actor_component_entry::_bc_actor_component_entry(bc_actor_component_register p_register)
			: bc_actor_component_register(std::move(p_register))
		{
			if(!m_is_abstract)
			{
				m_container = m_container_delegate();
			}
		}

		inline _bc_actor_component_entry::_bc_actor_component_entry(_bc_actor_component_entry&& p_other) noexcept
			: bc_actor_component_register(std::move(p_other)),
			m_actor_to_component_index_map(std::move(p_other.m_actor_to_component_index_map)),
			m_container(std::move(p_other.m_container))
		{
		}

		inline _bc_actor_component_entry& _bc_actor_component_entry::operator=(_bc_actor_component_entry&& p_other) noexcept
		{
			bc_actor_component_register::operator=(std::move(p_other));
			m_actor_to_component_index_map = std::move(p_other.m_actor_to_component_index_map);
			m_container = std::move(p_other.m_container);
			return *this;
		}

		inline bc_actor bc_actor_component_manager_container::create_actor(const bc_actor* p_parent)
		{
			bc_actor_id l_actor_id;
			const bc_actor_id l_parent_id = p_parent ? p_parent->get_id() : bc_actor_id();
			bool l_actor_to_component_need_resize = false;

			{
				platform::bc_shared_mutex_guard l_lock(m_actors_lock);
				bcUINT32 l_free_slot;

				if (m_actors_bit.find_first_false(l_free_slot))
				{
					auto& l_entry = m_actors[l_free_slot];
					const auto l_repeat_index = bc_actor_id::decompose_id(l_entry.m_actor_id).m_repeat_index;
					const auto l_new_repeat_index = bc_actor_id::generate_new_repeat_id(l_repeat_index);
					l_actor_id = bc_actor_id::compose_id(l_free_slot, m_my_index, l_new_repeat_index);

					new (&l_entry) _bc_actor_entry(l_actor_id, l_parent_id);
					m_actors_bit.make_true(l_free_slot);
				}
				else
				{
					l_actor_to_component_need_resize = true;
					l_free_slot = m_actors.size();

					l_actor_id = bc_actor_id::compose_id(l_free_slot, m_my_index, 0);
					m_actors.push_back(_bc_actor_entry(l_actor_id, l_parent_id));
					m_actors_bit.resize(m_actors.size());
					m_actors_bit.make_true(l_free_slot);
				}
			}

			if (l_actor_to_component_need_resize)
			{
				{
					platform::bc_shared_mutex_shared_guard l_lock(m_actors_lock);
					_resize_actor_to_component_index_maps();
				}
			}

			return bc_actor(l_actor_id);
		}

		inline void bc_actor_component_manager_container::remove_actor(const bc_actor& p_actor)
		{
			BC_ASSERT(p_actor.is_valid());

			const auto [l_actor_index, l_manager_index, l_actor_repeat_index] = bc_actor_id::decompose_id(p_actor.get_id());

			for (auto& l_component : m_components)
			{
				_bc_actor_component_entry& l_component_entry = l_component.second;

				if (l_component_entry.m_is_abstract)
				{
					continue;
				}

				{
					platform::bc_shared_mutex_shared_guard l_lock(l_component_entry.m_lock);

					const auto l_component_index = l_component_entry.m_actor_to_component_index_map[l_actor_index];

					// This actor has this component type
					if (l_component_index != bci_actor_component::invalid_id)
					{
						l_component_entry.m_container->remove(l_component_index);
						l_component_entry.m_actor_to_component_index_map[l_actor_index] = bci_actor_component::invalid_id;
					}
				}
			}

			{
				platform::bc_shared_mutex_shared_guard l_lock(m_actors_lock);

				BC_ASSERT(m_actors_bit[l_actor_index]);

				auto& l_actor_entry = m_actors[l_actor_index];

				_clear_actor_events(l_actor_index, m_read_event_pool);
				_clear_actor_events(l_actor_index, m_write_event_pool);

				m_actors_bit.make_false(l_actor_index);
				l_actor_entry.~_bc_actor_entry();
				// Mark actor with invalid index but preserve repeat index for future actor creation
				l_actor_entry.m_actor_id = bc_actor_id::compose_invalid_index(l_actor_repeat_index);
			}
		}

		template<typename TEvent>
		void bc_actor_component_manager_container::actor_add_event(const bc_actor& p_actor, TEvent&& p_event)
		{
			BC_ASSERT(p_actor.is_valid());

			const auto l_actor_index = bc_actor_id::decompose_id(p_actor.get_id()).m_index;
			auto* l_event = m_event_pools[m_write_event_pool].alloc<TEvent>(std::move(p_event));

			{
				platform::bc_shared_mutex_shared_guard l_lock(m_actors_lock);

				auto& l_actor_entry = m_actors[l_actor_index];
				l_actor_entry.add_event(m_write_event_pool, l_event);
			}
		}

		inline bc_actor_event* bc_actor_component_manager_container::actor_get_events(const bc_actor& p_actor) const noexcept
		{
			BC_ASSERT(p_actor.is_valid());

			const auto l_actor_index = bc_actor_id::decompose_id(p_actor.get_id()).m_index;

			{
				platform::bc_shared_mutex_shared_guard l_lock(m_actors_lock);

				const auto& l_actor_entry = m_actors[l_actor_index];
				return l_actor_entry.get_events(m_read_event_pool);
			}
		}

		inline bool bc_actor_component_manager_container::actor_is_valid(const bc_actor& p_actor) noexcept
		{
			const auto l_actor_id = p_actor.get_id();
			if (!l_actor_id.is_valid())
			{
				return false;
			}

			const auto l_actor_index = bc_actor_id::decompose_id(l_actor_id).m_index;

			{
				platform::bc_shared_mutex_shared_guard l_lock(m_actors_lock);

				const auto& l_actor_entry = m_actors[l_actor_index];
				return l_actor_entry.m_actor_id == l_actor_id;
			}
		}

		inline void bc_actor_component_manager_container::mark_actor_for_double_update(const bc_actor& p_actor)
		{
			m_double_update_actors.push(p_actor);
		}

		template<class TComponent>
		void bc_actor_component_manager_container::create_component(const bc_actor& p_actor)
		{
			_bc_actor_component_entry* l_component_entry = _get_component_entry<TComponent>();

			BC_ASSERT(p_actor.is_valid());
			BC_ASSERT(!l_component_entry->m_is_abstract);

			const auto l_actor_id = p_actor.get_id();
			const auto l_actor_index = bc_actor_id::decompose_id(l_actor_id).m_index;
			bc_actor_id l_parent_id;

			{
				platform::bc_shared_mutex_shared_guard l_lock(m_actors_lock);
				l_parent_id = m_actors[l_actor_index].m_parent_id;
			}

			auto l_component_id = bci_actor_component::invalid_id;
			auto l_parent_component_id = bci_actor_component::invalid_id;

			// If actor has parent and it's parent has this component type we must create actor component
			// after parent component in sorting order of container so parent component update earlier
			if (l_parent_id.is_valid())
			{
				if (const TComponent* l_parent_component = actor_get_component<TComponent>(bc_actor(l_parent_id)))
				{
					l_parent_component_id = l_parent_component->get_id();
				}
			}

			{
				platform::bc_shared_mutex_guard l_lock(l_component_entry->m_lock);

				// Cast to concrete container to avoid virtual calls
				auto* l_concrete_container = static_cast<bc_actor_component_container<TComponent>*>(l_component_entry->m_container.get());

				if (l_parent_component_id == bci_actor_component::invalid_id)
				{
					l_component_id = l_concrete_container->create(l_actor_id);
				}
				else
				{
					l_component_id = l_concrete_container->create_after(l_actor_id, l_parent_component_id);
				}

				l_component_entry->m_actor_to_component_index_map[l_actor_index] = l_component_id;
			}
		}

		template<class TComponent>
		void bc_actor_component_manager_container::remove_component(const bc_actor& p_actor)
		{
			_bc_actor_component_entry* l_component_entry = _get_component_entry<TComponent>();

			BC_ASSERT(p_actor.is_valid());
			BC_ASSERT(!l_component_entry->m_is_abstract);

			const auto l_actor_index = bc_actor_id::decompose_id(p_actor.get_id()).m_index;

			{
				platform::bc_shared_mutex_shared_guard l_lock(l_component_entry->m_lock);

				auto l_component_id = l_component_entry->m_actor_to_component_index_map[l_actor_index];
				// Actor has not this type of component
				if (l_component_id == bci_actor_component::invalid_id)
				{
					BC_ASSERT(false);
					return;
				}

				// Cast to concrete container to avoid virtual calls
				auto* l_concrete_container = static_cast<bc_actor_component_container<TComponent>*>(l_component_entry->m_container.get());
				l_concrete_container->remove(l_component_id);

				l_component_entry->m_actor_to_component_index_map[l_actor_index] = bci_actor_component::invalid_id;
			}
		}

		template<class TComponent>
		bool bc_actor_component_manager_container::actor_has_component(const bc_actor& p_actor) const noexcept
		{
			return actor_get_component<TComponent>(p_actor) != nullptr;
		}

		template<class TComponent>
		TComponent* bc_actor_component_manager_container::actor_get_component(const bc_actor& p_actor) noexcept
		{
			static_assert(std::is_base_of_v<bci_actor_component, TComponent>, "TComponent parameter is not a component");

			return static_cast<TComponent*>
			(
				_actor_get_component<TComponent>
				(
					p_actor,
					std::integral_constant<bool, bc_actor_component_traits<TComponent>::component_is_abstract()>()
				)
			);
		}

		template<class TComponent>
		const TComponent* bc_actor_component_manager_container::actor_get_component(const bc_actor& p_actor) const noexcept
		{
			return const_cast<bc_actor_component_manager_container&>(*this).actor_get_component<TComponent>(p_actor);
		}

		template<class TIterator>
		void bc_actor_component_manager_container::actor_get_components(const bc_actor& p_actor, TIterator p_destination) const
		{
			BC_ASSERT(p_actor.is_valid());

			const auto l_actor_index = bc_actor_id::decompose_id(p_actor.get_id()).m_index;

			for (auto& l_component_entry : m_components)
			{
				if (l_component_entry.second.m_is_abstract)
				{
					continue; // It is an abstract component
				}

				{
					platform::bc_shared_mutex_shared_guard l_lock(l_component_entry.second.m_lock);

					const auto l_component_id = l_component_entry.second.m_actor_to_component_index_map[l_actor_index];
					if (l_component_id != bci_actor_component::invalid_id)
					{
						*p_destination = l_component_entry.second.m_container->get(l_component_id);
						++p_destination;
					}
				}
			}
		}

		template<class TComponent>
		bc_actor bc_actor_component_manager_container::component_get_actor(const TComponent& p_component) const noexcept
		{
			static_assert(!bc_actor_component_traits<TComponent>::component_is_abstract(), "Can't get actor from abstract component");

			const bc_actor_id l_actor_id = p_component.get_actor_id();
			return bc_actor(l_actor_id);
		}
		
		template<class TComponent>
		TComponent* bc_actor_component_manager_container::_actor_get_component(const bc_actor& p_actor, std::true_type)
		{
			_bc_actor_component_entry* l_component_entry = _get_component_entry<TComponent>();
			
			BC_ASSERT(l_component_entry->m_is_abstract);

			for (auto& l_derived_get : l_component_entry->m_deriveds_delegates)
			{
				auto* l_derived_component = l_derived_get(p_actor, *this);
				if (l_derived_component != nullptr)
				{
					return static_cast<TComponent*>(l_derived_component);
				}
			}

			return nullptr;
		}

		template<class TComponent>
		TComponent* bc_actor_component_manager_container::_actor_get_component(const bc_actor& p_actor, std::false_type)
		{
			_bc_actor_component_entry* l_component_entry = _get_component_entry<TComponent>();

			BC_ASSERT(p_actor.is_valid());
			BC_ASSERT(!l_component_entry->m_is_abstract);

			const auto l_actor_index = bc_actor_id::decompose_id(p_actor.get_id()).m_index;

			{
				platform::bc_shared_mutex_shared_guard l_lock(l_component_entry->m_lock);

				auto l_actor_to_component = l_component_entry->m_actor_to_component_index_map[l_actor_index];
				// Actor does not has this type of component
				if (l_actor_to_component == bci_actor_component::invalid_id)
				{
					return nullptr;
				}

				// Cast to concrete container to avoid virtual calls
				auto* l_concrete_container = static_cast<bc_actor_component_container<TComponent>*>(l_component_entry->m_container.get());
				auto* l_component = l_concrete_container->get(l_actor_to_component);

				return l_component;
			}
		}

		inline void bc_actor_component_manager_container::_clear_actor_events(bcUINT32 p_index, bcUINT32 p_event_pool)
		{
			auto& l_actor_entry = m_actors[p_index];
			auto* l_events_pool = &m_event_pools[p_event_pool];
			auto* l_actor_events = l_actor_entry.get_events(p_event_pool);

			while (l_actor_events)
			{
				bc_actor_event* l_next = l_actor_events->get_next();
				l_events_pool->free(l_actor_events);
				l_actor_events = l_next;
			}

			l_actor_entry.clear_events(p_event_pool);
		}

		template<class TComponent>
		_bc_actor_component_entry* bc_actor_component_manager_container::_get_component_entry()
		{
			static_assert(std::is_base_of_v<bci_actor_component, TComponent>, "TComponent must inherit from bc_iactor_component");

			constexpr bc_actor_component_hash l_hash = bc_actor_component_traits<TComponent>::component_hash();
			const auto l_entry = m_components.find(l_hash);

			if (l_entry == std::end(m_components))
			{
				return nullptr;
			}

			return &l_entry->second;
		}

		template<class TComponent>
		const _bc_actor_component_entry* bc_actor_component_manager_container::_get_component_entry() const
		{
			return const_cast<bc_actor_component_manager_container&>(*this)._get_component_entry<TComponent>();
		}

		inline void bc_actor_component_manager_container::_resize_actor_to_component_index_maps()
		{
			for (auto& l_component_entry : m_components)
			{
				// It's an abstract component
				if (l_component_entry.second.m_is_abstract)
				{
					continue;
				}

				{
					platform::bc_shared_mutex_guard l_lock(l_component_entry.second.m_lock);

					// If component type doesn't have enough space in index map
					if (l_component_entry.second.m_actor_to_component_index_map.size() < m_actors.size())
					{
						l_component_entry.second.m_actor_to_component_index_map.resize(m_actors.capacity(), bci_actor_component::invalid_id);
					}
				}
			}
		}
	}
}
