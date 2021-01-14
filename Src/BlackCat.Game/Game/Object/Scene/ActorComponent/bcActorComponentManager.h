// [05/18/2016 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "CorePlatformImp/Concurrency/bcAtomic.h"
#include "Core/Memory/bcPtr.h"
#include "Core/bcConstant.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Concurrency/bcThreadManager.h"
#include "Core/Concurrency/bcConcurrency.h"
#include "Core/Utility/bcNullable.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcEnumOperand.h"
#include "Core/Utility/bcObjectStackPool.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/ActorComponent/bcActorEvent.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentContainer.h"

namespace black_cat
{
	namespace game
	{
		class bc_mediate_component;
		class bc_actor_component_manager;

		template< class TComponent >
		struct bc_actor_component_traits
		{
		public:
			static constexpr bool component_is_abstract()
			{
				return TComponent::component_is_abstract();
			}

			static constexpr bool component_require_event()
			{
				return TComponent::component_require_event();
			}

			static constexpr bool component_require_update()
			{
				return TComponent::component_require_update();
			}

			static constexpr const bcCHAR* component_name()
			{
				return TComponent::component_name();
			}

			static constexpr bc_actor_component_hash component_hash()
			{
				return static_cast< bc_actor_component_hash >(TComponent::component_hash());
			}
		};

		template<typename TComponent>
		class bc_component_register
		{
		public:
			using component_t = TComponent;
			
			bc_component_register(const bcCHAR* p_data_driven_name)
				: m_data_driven_name(p_data_driven_name)
			{
			}

			const bcCHAR* m_data_driven_name;
		};
		
		template< class TAbstract, class TDerived, class ...TDeriveds >
		class bc_abstract_component_register
		{
		public:
			using abstract_component_t = TAbstract;
			template< template<typename, typename...> class T >
			using apply = T<TAbstract, TDerived, TDeriveds...>;

			void operator()(bc_actor_component_manager& p_component_manager) const
			{
				p_component_manager._register_abstract_component_type< TAbstract, TDerived, TDeriveds... >();
			}
		};

		struct _bc_actor_entry
		{
		public:
			explicit _bc_actor_entry(bc_actor_index p_actor, bc_actor_index p_parent_index = bc_actor::invalid_index);

			_bc_actor_entry(const _bc_actor_entry& p_other);

			~_bc_actor_entry();

			_bc_actor_entry& operator=(const _bc_actor_entry& p_other);

			void add_event(bcUINT32 p_active_event_pool, bc_actor_event* p_event) noexcept;

			bc_actor_event* get_events(bcUINT32 p_active_event_pool) const noexcept;

			void clear_events(bcUINT32 p_active_event_pool) noexcept;

			bc_actor_index m_actor_index;
			bc_actor_index m_parent_index;
			bc_actor_event* m_events[2];
			core_platform::bc_spin_mutex m_mutex;
		};

		struct _bc_actor_component_entry
		{
		public:
			using deriveds_component_get_delegate = core::bc_delegate<bc_iactor_component*(const bc_actor&)>;

		public:
			_bc_actor_component_entry(bool p_is_abstract,
				bool p_require_event,
				bool p_require_update,
				bcUINT32 p_component_priority,
				core::bc_unique_ptr<bc_iactor_component_container> p_container,
				core::bc_vector_movable< deriveds_component_get_delegate > p_deriveds);
			
			_bc_actor_component_entry(_bc_actor_component_entry&& p_other) noexcept;

			_bc_actor_component_entry& operator=(_bc_actor_component_entry&& p_other) noexcept;

			static constexpr bcUINT32 s_invalid_priority_value = -1;
			bool m_is_abstract;
			bool m_require_event;
			bool m_require_update;
			bcUINT32 m_component_priority;
			core::bc_vector_movable< bc_actor_component_index > m_actor_to_component_index_map;
			core::bc_vector_movable< bc_actor_index > m_component_to_actor_index_map;
			core::bc_unique_ptr< bc_iactor_component_container > m_container;
			core::bc_vector_movable< deriveds_component_get_delegate > m_deriveds;
			mutable core_platform::bc_shared_mutex m_lock;
		};

		class bc_actor_component_manager : public core::bc_iservice
		{
			BC_SERVICE(ac_mng)

		private:
			template< class TAbstract, class TDerived, class ...TDeriveds >
			friend class bc_abstract_component_register;
			using actor_container_type = core::bc_vector_movable< core::bc_nullable< _bc_actor_entry > >;
			using component_container_type = core::bc_unordered_map_program< bc_actor_component_hash, _bc_actor_component_entry >;

		public:
			bc_actor_component_manager();

			bc_actor_component_manager(bc_actor_component_manager&&) noexcept = delete;

			~bc_actor_component_manager();

			bc_actor_component_manager& operator=(bc_actor_component_manager&&) noexcept = delete;

			bc_actor create_actor(const bc_actor* p_parent = nullptr);

			void remove_actor(const bc_actor& p_actor);

			template<typename TEvent>
			void actor_add_event(const bc_actor& p_actor, TEvent&& p_event);

			bc_actor_event* actor_get_events(const bc_actor& p_actor) const noexcept;

			void actor_clear_events(const bc_actor& p_actor) noexcept;
			
			// Create uninitialized component for actor
			template< class TComponent >
			void create_component(const bc_actor& p_actor);

			template< class TComponent >
			void remove_component(const bc_actor& p_actor);

			template< class TComponent >
			bool actor_has_component(const bc_actor& p_actor) const;

			template< class TComponent >
			TComponent* actor_get_component(const bc_actor& p_actor);

			template< class TComponent >
			const TComponent* actor_get_component(const bc_actor& p_actor) const;

			template< class TIterator >
			void actor_get_components(const bc_actor& p_actor, TIterator p_destination) const;

			template< class TComponent >
			bc_actor component_get_actor(const TComponent& p_component) const noexcept;

			template< class ...TCAdapter >
			void register_component_types(TCAdapter... p_components);

			template< class ...TCAdapter >
			void register_abstract_component_types(TCAdapter... p_components);

			void update_actors(const core_platform::bc_clock::update_param& p_clock);
			
		private:
			template< class TComponent >
			bc_iactor_component* _actor_get_component(const bc_actor& p_actor, std::true_type);

			template< class TComponent >
			bc_iactor_component* _actor_get_component(const bc_actor& p_actor, std::false_type);

			template< class TComponent >
			void _register_component_type(bcSIZE p_priority);

			template< class TComponent, class ...TDeriveds >
			void _register_abstract_component_type();

			template< class TComponent >
			component_container_type::value_type* _get_component_entry();

			template< class TComponent >
			const component_container_type::value_type* _get_component_entry() const;

			void _resize_actor_to_component_index_maps();

			const bcSIZE s_events_pool_capacity = 100 * 1024;

			mutable core_platform::bc_shared_mutex m_actors_lock;
			actor_container_type m_actors;
			component_container_type m_components;
			bcUINT32 m_read_event_pool;
			bcUINT32 m_write_event_pool;
			core::bc_concurrent_object_stack_pool m_events_pool[2];
		};

		inline _bc_actor_entry::_bc_actor_entry(bc_actor_index p_actor, bc_actor_index p_parent_index)
			: m_actor_index(p_actor),
			m_parent_index(p_parent_index),
			m_events{ nullptr, nullptr }
		{
		}

		inline _bc_actor_entry::_bc_actor_entry(const _bc_actor_entry& p_other)
		{
			operator=(p_other);
		}

		inline _bc_actor_entry::~_bc_actor_entry() = default;

		inline _bc_actor_entry& _bc_actor_entry::operator=(const _bc_actor_entry& p_other)
		{
			m_actor_index = p_other.m_actor_index;
			m_parent_index = p_other.m_parent_index;
			m_events[0] = p_other.m_events[0];
			m_events[1] = p_other.m_events[1];
			return *this;
		}

		inline void _bc_actor_entry::add_event(bcUINT32 p_active_event_pool, bc_actor_event* p_event) noexcept
		{
			{
				core_platform::bc_spin_mutex_guard l_lock(m_mutex);

				bc_actor_event* l_last_event = m_events[p_active_event_pool];

				if(!l_last_event)
				{
					m_events[p_active_event_pool] = p_event;
					return;
				}
				
				while (l_last_event->get_next())
				{
					l_last_event = l_last_event->get_next();
				}

				l_last_event->set_next(p_event);
			}
		}

		inline bc_actor_event* _bc_actor_entry::get_events(bcUINT32 p_active_event_pool) const noexcept
		{
			return m_events[p_active_event_pool];
		}

		inline void _bc_actor_entry::clear_events(bcUINT32 p_active_event_pool) noexcept
		{
			m_events[p_active_event_pool] = nullptr;
		}

		inline _bc_actor_component_entry::_bc_actor_component_entry(bool p_is_abstract,
			bool p_require_event,
			bool p_require_update,
			bcUINT32 p_component_priority,
			core::bc_unique_ptr<bc_iactor_component_container> p_container,
			core::bc_vector_movable< deriveds_component_get_delegate > p_deriveds)
			: m_is_abstract(p_is_abstract),
			m_require_event(p_require_event),
			m_require_update(p_require_update),
			m_component_priority(p_component_priority),
			m_actor_to_component_index_map(),
			m_component_to_actor_index_map(),
			m_container(std::move(p_container)),
			m_deriveds(std::move(p_deriveds))
		{
		}

		inline _bc_actor_component_entry::_bc_actor_component_entry(_bc_actor_component_entry&& p_other) noexcept
		{
			operator=(std::move(p_other));
		}

		inline _bc_actor_component_entry& _bc_actor_component_entry::operator=(_bc_actor_component_entry&& p_other) noexcept
		{
			m_is_abstract = p_other.m_is_abstract;
			m_require_event = p_other.m_require_event;
			m_require_update = p_other.m_require_update;
			m_component_priority = p_other.m_component_priority;
			m_actor_to_component_index_map = std::move(p_other.m_actor_to_component_index_map);
			m_component_to_actor_index_map = std::move(p_other.m_component_to_actor_index_map);
			m_container = std::move(p_other.m_container);
			m_deriveds = std::move(p_other.m_deriveds);
			return *this;
		}

		inline bc_actor_component_manager::bc_actor_component_manager()
		{
			m_events_pool[0].initialize(core::bc_get_service<core::bc_thread_manager>()->max_thread_count(), s_events_pool_capacity);
			m_events_pool[1].initialize(core::bc_get_service<core::bc_thread_manager>()->max_thread_count(), s_events_pool_capacity);
			m_read_event_pool = 0U;
			m_write_event_pool = 0U;
		}

		inline bc_actor_component_manager::~bc_actor_component_manager()
		{
			BC_ASSERT
			(
				std::count_if(std::cbegin(m_actors), std::cend(m_actors), [](core::bc_nullable< _bc_actor_entry >& p_entry)
				{
					return p_entry.is_set();
				}) == 0
			);

			m_events_pool[0].destroy();
			m_events_pool[1].destroy();
		}

		inline bc_actor bc_actor_component_manager::create_actor(const bc_actor* p_parent)
		{
			bc_actor_index l_index = 0;
			bool l_inserted = false;
			
			{
				core_platform::bc_shared_mutex_shared_guard l_lock(m_actors_lock);

				for (core::bc_nullable< _bc_actor_entry >& l_actor : m_actors) // TODO use fast free space lookup
				{
					if (!l_actor.is_set())
					{
						l_actor.reset(_bc_actor_entry(l_index, p_parent ? p_parent->get_index() : bc_actor::invalid_index));
						l_inserted = true;
						break;
					}
					
					++l_index;
				}
			}

			if (!l_inserted)
			{
				{
					core_platform::bc_shared_mutex_guard l_lock(m_actors_lock);

					m_actors.push_back(core::bc_nullable< _bc_actor_entry >
					(
						_bc_actor_entry(l_index, p_parent ? p_parent->get_index() : bc_actor::invalid_index)
					));
				}

				_resize_actor_to_component_index_maps();
			}

			return bc_actor(l_index);
		}

		inline void bc_actor_component_manager::remove_actor(const bc_actor& p_actor)
		{
			const auto l_actor_index = p_actor.get_index();

			for(auto& l_component : m_components)
			{
				_bc_actor_component_entry& l_component_entry = l_component.second;

				// It's an abstract component
				if(l_component_entry.m_component_priority == _bc_actor_component_entry::s_invalid_priority_value)
				{
					continue;
				}

				{
					core_platform::bc_shared_mutex_shared_guard l_lock(l_component_entry.m_lock);

					const auto l_component_index = l_component_entry.m_actor_to_component_index_map[l_actor_index];

					// This actor has this component type
					if (l_component_index != bc_iactor_component::invalid_index)
					{
						l_component_entry.m_container->remove(l_component_index);
						l_component_entry.m_component_to_actor_index_map[l_component_index] = bc_actor::invalid_index;
						l_component_entry.m_actor_to_component_index_map[l_actor_index] = bc_iactor_component::invalid_index;
					}
				}
			}

			{
				core_platform::bc_shared_mutex_shared_guard l_lock(m_actors_lock);
				
				auto& l_actor_entry = m_actors[p_actor.get_index()];
				BC_ASSERT(l_actor_entry.is_set());
				
				auto* l_events_pool = &m_events_pool[m_write_event_pool];
				auto* l_actor_events = l_actor_entry->get_events(m_write_event_pool);

				while (l_actor_events)
				{
					bc_actor_event* l_next = l_actor_events->get_next();
					l_events_pool->free(l_actor_events);
					l_actor_events = l_next;
				}
				
				m_actors[p_actor.get_index()].reset();
			}
		}

		template<typename TEvent>
		void bc_actor_component_manager::actor_add_event(const bc_actor& p_actor, TEvent&& p_event)
		{
			auto* l_event = m_events_pool[m_write_event_pool].alloc<TEvent>(std::move(p_event));

			{
				core_platform::bc_shared_mutex_shared_guard l_lock(m_actors_lock);

				auto& l_actor_entry = m_actors[p_actor.get_index()].get();
				l_actor_entry.add_event(m_write_event_pool, l_event);
			}
		}

		inline bc_actor_event* bc_actor_component_manager::actor_get_events(const bc_actor& p_actor) const noexcept
		{
			{
				core_platform::bc_shared_mutex_shared_guard l_lock(m_actors_lock);

				const auto& l_actor_entry = m_actors[p_actor.get_index()].get();
				return l_actor_entry.get_events(m_read_event_pool);
			}
		}

		inline void bc_actor_component_manager::actor_clear_events(const bc_actor& p_actor) noexcept
		{
			{
				core_platform::bc_shared_lock< core_platform::bc_shared_mutex > l_lock(m_actors_lock);

				auto& l_actor_entry = m_actors[p_actor.get_index()].get();
				l_actor_entry.clear_events(m_read_event_pool);
			}
		}

		template< class TComponent >
		void bc_actor_component_manager::create_component(const bc_actor& p_actor)
		{
			static component_container_type::value_type* l_s_component_entry = _get_component_entry< TComponent >();

			const auto l_actor_index = p_actor.get_index();
			bc_actor_index l_parent_index;
			
			BC_ASSERT(l_actor_index != bc_actor::invalid_index);
			
			{
				core_platform::bc_shared_mutex_shared_guard l_lock(m_actors_lock);
				l_parent_index = m_actors[l_actor_index]->m_parent_index;
			}
			
			auto l_component_index = bc_iactor_component::invalid_index;
			auto l_parent_component_index = bc_iactor_component::invalid_index;

			// If actor has parent and it's parent has this component type we must create actor component
			// after parent component in sorting order of container so parent component update earlier
			if(l_parent_index != bc_actor::invalid_index)
			{
				const TComponent* l_parent_component = actor_get_component<TComponent>(bc_actor(l_parent_index));
				if(l_parent_component)
				{
					l_parent_component_index = l_parent_component->get_index();
				}
			}

			{
				core_platform::bc_shared_mutex_guard l_lock(l_s_component_entry->second.m_lock);
				
				// Cast to concrete container to avoid virtual calls
				auto* l_concrete_container = static_cast<bc_actor_component_container< TComponent >*>(l_s_component_entry->second.m_container.get());

				if(l_parent_component_index == bc_iactor_component::invalid_index)
				{
					l_component_index = l_concrete_container->create();
				}
				else
				{
					l_component_index = l_concrete_container->create_after(l_parent_component_index);
				}

				if (l_s_component_entry->second.m_component_to_actor_index_map.size() < l_component_index + 1)
				{
					l_s_component_entry->second.m_component_to_actor_index_map.resize(l_concrete_container->capacity(), bc_actor::invalid_index);
				}

				l_s_component_entry->second.m_component_to_actor_index_map[l_component_index] = l_actor_index;
				l_s_component_entry->second.m_actor_to_component_index_map[l_actor_index] = l_component_index;
			}
		}

		template< class TComponent >
		void bc_actor_component_manager::remove_component(const bc_actor& p_actor)
		{
			static component_container_type::value_type* l_component_entry = _get_component_entry< TComponent >();

			const auto l_actor_index = p_actor.get_index();

			BC_ASSERT(l_actor_index != bc_actor::invalid_index);

			{
				core_platform::bc_shared_mutex_shared_guard l_lock(l_component_entry->second.m_lock);

				auto l_component_index = l_component_entry->second.m_actor_to_component_index_map[l_actor_index];
				// Actor has not this type of component
				if (l_component_index == bc_iactor_component::invalid_index)
				{
					BC_ASSERT(false);
					return;
				}

				// Cast to concrete container to avoid virtual calls
				auto* l_concrete_container = static_cast<bc_actor_component_container< TComponent >*>(l_component_entry->second.m_container.get());

				l_concrete_container->remove(l_component_index);

				l_component_entry->second.m_component_to_actor_index_map[l_component_index] = bc_actor::invalid_index;
				l_component_entry->second.m_actor_to_component_index_map[l_actor_index] = bc_iactor_component::invalid_index;
			}
		}

		template< class TComponent >
		bool bc_actor_component_manager::actor_has_component(const bc_actor& p_actor) const
		{
			return actor_get_component< TComponent >(p_actor) != nullptr;
		}

		template< class TComponent >
		TComponent* bc_actor_component_manager::actor_get_component(const bc_actor& p_actor)
		{
			static_assert(std::is_base_of_v<bc_iactor_component, TComponent>, "TComponent parameter is not a component");
			
			return static_cast<TComponent*>(_actor_get_component<TComponent>
			(
				p_actor, 
				std::integral_constant<bool, bc_actor_component_traits<TComponent>::component_is_abstract()>()
			));
		}

		template< class TComponent >
		const TComponent* bc_actor_component_manager::actor_get_component(const bc_actor& p_actor) const
		{
			return const_cast<bc_actor_component_manager*>(this)->actor_get_component< TComponent >(p_actor);
		}

		template< class TIterator >
		void bc_actor_component_manager::actor_get_components(const bc_actor& p_actor, TIterator p_destination) const
		{
			const auto l_actor_index = p_actor.get_index();

			for (auto& l_component_entry : m_components)
			{
				if(l_component_entry.second.m_component_priority == bc_iactor_component::invalid_index)
				{
					continue; // It is an abstract component
				}

				{
					core_platform::bc_shared_mutex_shared_guard l_lock(l_component_entry.second.m_lock);

					const auto l_component_index = l_component_entry.second.m_actor_to_component_index_map[l_actor_index];
					if (l_component_index != bc_iactor_component::invalid_index)
					{
						*p_destination = l_component_entry.second.m_container->get(l_component_index);
						++p_destination;
					}
				}
			}
		}

		template< class TComponent >
		bc_actor bc_actor_component_manager::component_get_actor(const TComponent& p_component) const noexcept
		{
			static_assert(!bc_actor_component_traits<TComponent>::component_is_abstract(), "Can't get actor from abstract component");

			static const component_container_type::value_type* l_component_entry = _get_component_entry< TComponent >();

			const bc_actor_component_index l_component_index = p_component.get_index();

			{
				core_platform::bc_shared_mutex_shared_guard l_actors_lock(m_actors_lock);
				core_platform::bc_shared_mutex_shared_guard l_component_lock(l_component_entry->second.m_lock);

				const auto l_component_to_actor = l_component_entry->second.m_component_to_actor_index_map[l_component_index];
				return bc_actor(m_actors[l_component_to_actor].get().m_actor_index);
			}
		}

		template< class ...TCAdapter >
		void bc_actor_component_manager::register_component_types(TCAdapter... p_components)
		{
			bcSIZE l_counter = m_components.size();

			auto l_expansion_list =
			{
				(
					[this, &l_counter]()
					{
						// Register mediate component as last component
						const bcSIZE l_priority = std::is_same_v< typename TCAdapter::component_t, bc_mediate_component >
							                          ? _bc_actor_component_entry::s_invalid_priority_value - 1
							                          : l_counter++;
						this->_register_component_type< typename TCAdapter::component_t >(l_priority);

						return true;
					}()
				)...
			};

			// Create a dummy actor and add all components to it to initialize local static variables
			// in component related functions
			bc_actor l_actor = create_actor();

			l_expansion_list =
			{
				(
					[this, l_actor]()
					{
						this->create_component< typename TCAdapter::component_t >(l_actor);
						typename TCAdapter::component_t* l_component = this->actor_get_component< typename TCAdapter::component_t >(l_actor);
						l_component->get_actor();
						this->remove_component< typename TCAdapter::component_t >(l_actor);

						return true;
					}()
				)...
			};

			remove_actor(l_actor);
		}

		template< class ...TCAdapter >
		void bc_actor_component_manager::register_abstract_component_types(TCAdapter... p_components)
		{
			bc_actor l_actor = create_actor();

			auto l_expansion_list =
			{
				(
					[this, l_actor, p_components]()
					{
						p_components(*this);
					
						this->actor_get_component< typename TCAdapter::abstract_component_t >(l_actor);

						return true;
					}()
				)...
			};

			remove_actor(l_actor);
		}

		inline void bc_actor_component_manager::update_actors(const core_platform::bc_clock::update_param& p_clock)
		{
			core::bc_vector_frame< _bc_actor_component_entry* > l_components_with_event;
			core::bc_vector_frame< _bc_actor_component_entry* > l_components_with_update;
			
			l_components_with_event.reserve(m_components.size());
			l_components_with_update.reserve(m_components.size());

			for (auto& l_entry : m_components)
			{
				if (l_entry.second.m_is_abstract)
				{
					continue;
				}

				if(l_entry.second.m_require_event)
				{
					l_components_with_event.push_back(&l_entry.second);
				}
				
				if(l_entry.second.m_require_update)
				{
					l_components_with_update.push_back(&l_entry.second);
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
			std::sort
			(
				std::begin(l_components_with_update),
				std::end(l_components_with_update),
				[](const _bc_actor_component_entry* p_first, const _bc_actor_component_entry* p_second)
				{
					return p_first->m_component_priority < p_second->m_component_priority;
				}
			);
			
			do
			{
				m_read_event_pool = m_write_event_pool;
				m_write_event_pool = (m_write_event_pool + 1) % 2;

				if (!m_events_pool[m_read_event_pool].size())
				{
					break;
				}

				core::bc_concurrency::concurrent_for_each
				(
					std::begin(l_components_with_event),
					std::end(l_components_with_event),
					[]() { return true; },
					[&](bool, _bc_actor_component_entry* p_entry)
					{
						p_entry->m_container->handle_events(*this);
					},
					[](bcINT32) {}
				);

				{
					core_platform::bc_shared_mutex_shared_guard l_actors_lock(m_actors_lock);

					core::bc_concurrency::concurrent_for_each
					(
						std::begin(m_actors),
						std::end(m_actors),
						[]() { return true; },
						[&](bool, actor_container_type::reference& p_actor_entry)
						{
							if (!p_actor_entry.is_set())
							{
								return;
							}

							auto* l_events_pool = &m_events_pool[m_read_event_pool];
							auto* l_actor_events = p_actor_entry->get_events(m_read_event_pool);

							while (l_actor_events)
							{
								bc_actor_event* l_next = l_actor_events->get_next();
								l_events_pool->free(l_actor_events);
								l_actor_events = l_next;
							}

							p_actor_entry->clear_events(m_read_event_pool);
						},
						[](bcINT32) {}
					);
				}
				
			} while (m_events_pool[m_write_event_pool].size());

			core::bc_concurrency::concurrent_for_each
			(
				std::begin(l_components_with_update),
				std::end(l_components_with_update),
				[]() { return true; },
				[&](bool, _bc_actor_component_entry* p_entry)
				{
					p_entry->m_container->update(*this, p_clock);
				},
				[](bcINT32) {}
			);
		}
		
		template< class TComponent >
		bc_iactor_component* bc_actor_component_manager::_actor_get_component(const bc_actor& p_actor, std::true_type)
		{
			static component_container_type::value_type* l_component_entry = _get_component_entry< TComponent >();

			// Is it abstract
			BC_ASSERT(l_component_entry->second.m_component_priority == _bc_actor_component_entry::s_invalid_priority_value);

			for(auto& l_derived_get : l_component_entry->second.m_deriveds)
			{
				auto* l_derived_component = l_derived_get(p_actor);

				if(l_derived_component != nullptr)
				{
					return l_derived_component;
				}
			}

			return nullptr;
		}

		template< class TComponent >
		bc_iactor_component* bc_actor_component_manager::_actor_get_component(const bc_actor& p_actor, std::false_type)
		{
			static component_container_type::value_type* l_component_entry = _get_component_entry< TComponent >();

			// Isn't it abstract
			BC_ASSERT(l_component_entry->second.m_component_priority != _bc_actor_component_entry::s_invalid_priority_value);

			const auto l_actor_index = p_actor.get_index();

			BC_ASSERT(l_actor_index != bc_actor::invalid_index);

			{
				core_platform::bc_shared_mutex_shared_guard l_lock(l_component_entry->second.m_lock);

				auto l_actor_to_component = l_component_entry->second.m_actor_to_component_index_map[l_actor_index];
				// Actor does not has this type of component
				if (l_actor_to_component == bc_iactor_component::invalid_index)
				{
					return nullptr;
				}

				// Cast to concrete container to avoid virtual calls
				auto* l_concrete_container = static_cast<bc_actor_component_container< TComponent >*>(l_component_entry->second.m_container.get());
				auto* l_component = l_concrete_container->get(l_actor_to_component);

				return l_component;
			}
		}

		template< class TComponent >
		void bc_actor_component_manager::_register_component_type(bcSIZE p_priority)
		{
			static_assert
			(
				!bc_actor_component_traits< TComponent >::component_is_abstract(),
				"TComponent must not be an abstract component."
			);

			// Initialize components container
			constexpr auto l_hash = bc_actor_component_traits< TComponent >::component_hash();
			_bc_actor_component_entry l_data
			(
				bc_actor_component_traits<TComponent>::component_is_abstract(),
				bc_actor_component_traits<TComponent>::component_require_event(),
				bc_actor_component_traits<TComponent>::component_require_update(),
				p_priority,
				core::bc_make_unique< bc_actor_component_container< TComponent > >(core::bc_alloc_type::program),
				core::bc_vector_movable< _bc_actor_component_entry::deriveds_component_get_delegate >()
			);

			m_components.insert(component_container_type::value_type(l_hash, std::move(l_data)));
		}

		template< class TComponent, class ...TDeriveds >
		void bc_actor_component_manager::_register_abstract_component_type()
		{
			static_assert
			(
				bc_actor_component_traits< TComponent >::component_is_abstract(),
				"TComponent is not an abstract component."
			);

			constexpr auto l_hash = bc_actor_component_traits< TComponent >::component_hash();
			_bc_actor_component_entry l_data
			(
				bc_actor_component_traits<TComponent>::component_is_abstract(),
				bc_actor_component_traits<TComponent>::component_require_event(),
				bc_actor_component_traits<TComponent>::component_require_update(),
				_bc_actor_component_entry::s_invalid_priority_value,
				nullptr,
				core::bc_vector_movable< _bc_actor_component_entry::deriveds_component_get_delegate >
				(
					{
						_bc_actor_component_entry::deriveds_component_get_delegate
						(
							[&](const bc_actor& p_actor)
							{
								return this->_actor_get_component<TDeriveds>
								(
									p_actor,
									std::integral_constant<bool, bc_actor_component_traits<TDeriveds>::component_is_abstract()>()
								);
							}
						)...
					}
				)
			);

			m_components.insert(component_container_type::value_type(l_hash, std::move(l_data)));
		}

		template< class TComponent >
		bc_actor_component_manager::component_container_type::value_type* bc_actor_component_manager::_get_component_entry()
		{
			static_assert(std::is_base_of< bc_iactor_component, TComponent >::value, "TComponent must inherit from bc_iactor_component");

			constexpr bc_actor_component_hash l_hash = bc_actor_component_traits< TComponent >::component_hash();
			const auto l_entry = m_components.find(l_hash);

			if (l_entry == std::end(m_components))
			{
				return nullptr;
			}

			return &*l_entry;
		}

		template< class TComponent >
		const bc_actor_component_manager::component_container_type::value_type* bc_actor_component_manager::_get_component_entry() const
		{
			return const_cast< bc_actor_component_manager& >(*this)._get_component_entry< TComponent >();
		}

		inline void bc_actor_component_manager::_resize_actor_to_component_index_maps()
		{
			for (auto& l_component_entry : m_components)
			{
				// It's an abstract component
				if(l_component_entry.second.m_component_priority == _bc_actor_component_entry::s_invalid_priority_value)
				{
					continue;
				}

				{
					core_platform::bc_shared_mutex_guard l_lock(l_component_entry.second.m_lock);

					// If component type doesn't have enough space in index map
					if (l_component_entry.second.m_actor_to_component_index_map.size() < m_actors.size())
					{
						l_component_entry.second.m_actor_to_component_index_map.resize(m_actors.capacity(), bc_iactor_component::invalid_index);
					}
				}
			}
		}
	}
}