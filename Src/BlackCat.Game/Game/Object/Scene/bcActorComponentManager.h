// [05/18/2016 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Core/bcConstant.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Utility/bcNullable.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcEnumOperand.h"
#include "Game/Object/Scene/bcActor.h"
#include "Game/Object/Scene/bcActorComponent.h"
#include "Game/Object/Scene/bcActorComponentContainer.h"
#include "PlatformImp/bcIDELogger.h"

namespace black_cat
{
	namespace game
	{
		class bc_actor_component_manager;

		template< class TComponent >
		struct bc_actor_component_traits
		{
		public:
			static constexpr bool component_is_abstract()
			{
				return TComponent::component_is_abstract();
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

		template< class TAbstract, class TDerived, class ...TDeriveds >
		class bc_abstract_component_register
		{
		public:
			using abstract_component_t = TAbstract;
			template< template<typename, typename...> class T >
			using apply = T<TAbstract, TDerived, TDeriveds...>;
		};

		template< class TAbstract, class ...TDeriveds >
		class _bc_abstract_component_registrar
		{
		public:
			void operator()(bc_actor_component_manager& p_component_manager) const
			{
				p_component_manager._register_abstract_component_type< TAbstract, TDeriveds... >();
			}
		};

		struct _bc_actor_entry
		{
		public:
			explicit _bc_actor_entry(const bc_actor& p_actor, bc_actor_index p_parent_index = bc_actor::invalid_index)
				: m_actor(p_actor),
				m_parent_index(p_parent_index)
			{
			}

			bc_actor m_actor;
			bc_actor_index m_parent_index;
		};

		struct _bc_actor_component_entry
		{
		public:
			using deriveds_component_get_delegate = core::bc_delegate<bc_iactor_component*(const bc_actor&)>;

		public:
			static constexpr bcUINT32 s_invalid_priority_value = -1;
			bcUINT32 m_component_priority;
			core::bc_vector_movale< bc_actor_component_index > m_actor_to_component_index_map;
			core::bc_vector_movale< bc_actor_index > m_component_to_actor_index_map;
			core::bc_unique_ptr< bc_iactor_component_container > m_container;
			core::bc_vector_movale< deriveds_component_get_delegate > m_deriveds;
		};

		class bc_actor_component_manager : public core::bc_iservice
		{
			BC_SERVICE(actor_component_manager)

		private:
			template<class TAbstract, class ...TDeriveds>
			friend class _bc_abstract_component_registrar;

			using actor_container_type = core::bc_vector_movale< core::bc_nullable< _bc_actor_entry > >;
			using component_map_type = core::bc_unordered_map_program< bc_actor_component_hash, _bc_actor_component_entry >;

		public:
			bc_actor_component_manager() = default;

			bc_actor_component_manager(bc_actor_component_manager&&) noexcept = delete;

			~bc_actor_component_manager();

			bc_actor_component_manager& operator=(bc_actor_component_manager&&) noexcept = delete;

			bc_actor create_actor(const bc_actor* p_parent = nullptr);

			void remove_actor(const bc_actor& p_actor);

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

			void update(const core_platform::bc_clock::update_param& p_clock_update_param) override;

			template< class ...TComponent >
			void register_component_types();

			template< class ...TComponent >
			void register_abstract_component_types();

		protected:

		private:
			template< class TComponent >
			TComponent* _actor_get_component(const bc_actor& p_actor, std::true_type);

			template< class TComponent >
			TComponent* _actor_get_component(const bc_actor& p_actor, std::false_type);

			template< class TComponent >
			void _register_component_type(bcSIZE p_priority);

			template< class TComponent, class ...TDeriveds >
			void _register_abstract_component_type();

			template< class TComponent >
			component_map_type::value_type* _get_component_entry();

			template< class TComponent >
			const component_map_type::value_type* _get_component_entry() const;

			void _resize_actor_to_component_index_maps();

			actor_container_type m_actors;
			component_map_type m_components;
		};

		inline bc_actor_component_manager::~bc_actor_component_manager()
		{
			bcAssert
			(
				std::count_if(std::cbegin(m_actors), std::cend(m_actors), [](core::bc_nullable< _bc_actor_entry >& p_entry)
				{
					return p_entry.is_set();
				}) == 0
			);
		}

		inline bc_actor bc_actor_component_manager::create_actor(const bc_actor* p_parent)
		{
			bc_actor_index l_index = 0;
			bool l_has_value = false;

			for (core::bc_nullable< _bc_actor_entry >& l_actor : m_actors) // TODO use fast free space lookup
			{
				if (!l_actor.is_set())
				{
					l_actor.reset(_bc_actor_entry(bc_actor(l_index), p_parent ? p_parent->get_index() : bc_actor::invalid_index));
					l_has_value = true;
					break;
				}
				else
				{
					++l_index;
				}
			}

			if (!l_has_value)
			{
				m_actors.push_back(core::bc_nullable< _bc_actor_entry >
				(
					_bc_actor_entry(bc_actor(l_index), p_parent ? p_parent->get_index() : bc_actor::invalid_index)
				));

				_resize_actor_to_component_index_maps();
			}

			return bc_actor(l_index);
		}

		inline void bc_actor_component_manager::remove_actor(const bc_actor& p_actor)
		{
			bc_actor_index l_actor_index = p_actor.get_index();

			for(auto& l_component : m_components)
			{
				_bc_actor_component_entry& l_component_entry = l_component.second;

				// It's an abstract component
				if(l_component_entry.m_component_priority == _bc_actor_component_entry::s_invalid_priority_value)
				{
					continue;
				}

				bc_actor_component_index l_component_index = l_component_entry.m_actor_to_component_index_map[l_actor_index];
				
				// This actor has this component type
				if(l_component_index != bc_iactor_component::invalid_index)
				{
					l_component_entry.m_container->remove(l_component_index);
					l_component_entry.m_component_to_actor_index_map[l_component_index] = bc_actor::invalid_index;
					l_component_entry.m_actor_to_component_index_map[l_actor_index] = bc_iactor_component::invalid_index;
				}
			}

			m_actors[p_actor.get_index()].reset();
		}

		template< class TComponent >
		void bc_actor_component_manager::create_component(const bc_actor& p_actor)
		{
			static component_map_type::value_type* l_component_entry = _get_component_entry< TComponent >();

			bcAssert(l_component_entry != nullptr);

			// Cast to concrete container to avoid virtual calls
			auto* l_concrete_container = static_cast<bc_actor_component_container< TComponent >*>(l_component_entry->second.m_container.get());

			const bc_actor_index l_actor_index = p_actor.get_index();

			bcAssert(l_actor_index != bc_actor::invalid_index);

			bc_actor_index l_parent_actor_index = m_actors[l_actor_index]->m_parent_index;
			bc_actor_component_index l_component_index = bc_iactor_component::invalid_index;

			// If actor has parent and it's parent has this component type we must create actor component
			// after parent component in sorting order of container so parent component update earlier
			if(l_parent_actor_index != bc_actor::invalid_index)
			{
				const bc_actor& l_parent = m_actors[l_parent_actor_index]->m_actor;
				const TComponent* l_parent_component = l_parent.get_component<TComponent>();

				if(l_parent_component)
				{
					bc_actor_component_index l_parent_component_index = l_parent_component->get_index();
					l_component_index = l_concrete_container->create_after(l_parent_component_index);
				}
			}
			
			if(l_component_index == bc_iactor_component::invalid_index)
			{
				l_component_index = l_concrete_container->create();
			}

			if (l_component_entry->second.m_component_to_actor_index_map.size() < l_component_index + 1)
			{
				l_component_entry->second.m_component_to_actor_index_map.resize(l_concrete_container->capacity(), bc_actor::invalid_index);
			}

			l_component_entry->second.m_component_to_actor_index_map[l_component_index] = l_actor_index;
			l_component_entry->second.m_actor_to_component_index_map[l_actor_index] = l_component_index;
		}

		template< class TComponent >
		void bc_actor_component_manager::remove_component(const bc_actor& p_actor)
		{
			static component_map_type::value_type* l_component_entry = _get_component_entry< TComponent >();

			bc_actor_index l_actor_index = p_actor.get_index();

			bcAssert(l_actor_index != bc_actor::invalid_index);

			bcINT32 l_component_index = l_component_entry->second.m_actor_to_component_index_map[l_actor_index];
			// Actor has not this type of component
			if (l_component_index == bc_iactor_component::invalid_index)
			{
				bcAssert(false);
				return;
			}

			// Cast to concrete container to avoid virtual calls
			auto* l_concrete_container = static_cast<bc_actor_component_container< TComponent >*>(l_component_entry->second.m_container.get());

			l_concrete_container->remove(l_component_index);

			l_component_entry->second.m_component_to_actor_index_map[l_component_index] = bc_actor::invalid_index;
			l_component_entry->second.m_actor_to_component_index_map[l_actor_index] = bc_iactor_component::invalid_index;
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
			return _actor_get_component<TComponent>(p_actor, std::integral_constant<bool, bc_actor_component_traits<TComponent>::component_is_abstract()>());
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

			for (auto& l_component : m_components)
			{
				if(l_component.second.m_component_priority == bc_iactor_component::invalid_index)
				{
					continue; // It is an abstract component
				}

				const auto l_component_index = l_component.second.m_actor_to_component_index_map[l_actor_index];
				if(l_component_index != bc_iactor_component::invalid_index)
				{
					*p_destination = l_component.second.m_container->get(l_component_index);
					++p_destination;
				}
			}
		}

		template< class TComponent >
		bc_actor bc_actor_component_manager::component_get_actor(const TComponent& p_component) const noexcept
		{
			static_assert(!bc_actor_component_traits<TComponent>::component_is_abstract(), "Can't get actor from abstract component");

			static const component_map_type::value_type* l_component_entry = _get_component_entry< TComponent >();

			const bc_actor_component_index l_component_index = p_component.get_index();
			const bcINT32 l_component_to_actor = l_component_entry->second.m_component_to_actor_index_map[l_component_index];
			return m_actors[l_component_to_actor].get().m_actor;
		}

		inline void bc_actor_component_manager::update(const core_platform::bc_clock::update_param& p_clock_update_param)
		{
			core::bc_vector_frame< _bc_actor_component_entry* > l_components;
			l_components.reserve(m_components.size());

			for (auto& l_entry : m_components)
			{
				// It's an abstract component
				if(l_entry.second.m_component_priority == _bc_actor_component_entry::s_invalid_priority_value)
				{
					continue;
				}

				l_components.push_back(&l_entry.second);
			}

			std::sort(std::begin(l_components), std::end(l_components), [](const _bc_actor_component_entry* p_first, const _bc_actor_component_entry* p_second)
				{
					return p_first->m_component_priority < p_second->m_component_priority;
				});

			for (auto l_component_data : l_components)
			{
				l_component_data->m_container->update(this, p_clock_update_param);
			}
		}

		template< class ...TComponent >
		void bc_actor_component_manager::register_component_types()
		{
			bcSIZE l_counter = 0;

			auto l_expansion_list =
			{
				(
					[this, &l_counter]()
					{						
						this->_register_component_type< TComponent >(l_counter++);

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
						this->create_component< TComponent >(l_actor);
						TComponent* l_component = this->actor_get_component< TComponent >(l_actor);
						l_component->get_actor();
						this->remove_component< TComponent >(l_actor);

						return true;
					}()
				)...
			};

			remove_actor(l_actor);
		}

		template< class ...TComponent >
		void bc_actor_component_manager::register_abstract_component_types()
		{
			bc_actor l_actor = create_actor();

			auto l_expansion_list =
			{
				(
					[this, l_actor]()
					{
						using expanded_type = typename TComponent::template apply< _bc_abstract_component_registrar >;
						expanded_type()(*this);

						this->actor_get_component<typename TComponent::abstract_component_t>(l_actor);

						return true;
					}()
				)...
			};

			remove_actor(l_actor);
		}

		template< class TComponent >
		TComponent* bc_actor_component_manager::_actor_get_component(const bc_actor& p_actor, std::true_type)
		{
			static component_map_type::value_type* l_component_entry = _get_component_entry< TComponent >();

			// Is it abstract
			bcAssert(l_component_entry->second.m_component_priority == _bc_actor_component_entry::s_invalid_priority_value);

			for(auto& l_derived_get : l_component_entry->second.m_deriveds)
			{
				auto* l_derived_component = l_derived_get(p_actor);

				if(l_derived_component != nullptr)
				{
					return static_cast< TComponent* >(l_derived_component); // Return type of get delegate is bc_iactor_component*
				}
			}

			return nullptr;
		}

		template< class TComponent >
		TComponent* bc_actor_component_manager::_actor_get_component(const bc_actor& p_actor, std::false_type)
		{
			static component_map_type::value_type* l_component_entry = _get_component_entry< TComponent >();

			// Isn't it abstract
			bcAssert(l_component_entry->second.m_component_priority != _bc_actor_component_entry::s_invalid_priority_value);

			const bc_actor_index l_actor_index = p_actor.get_index();

			bcAssert(l_actor_index != bc_actor::invalid_index);

			bcINT32 l_actor_to_component = l_component_entry->second.m_actor_to_component_index_map[l_actor_index];
			// Actor does not has this type of component
			if (l_actor_to_component == bc_iactor_component::invalid_index)
			{
				return nullptr;
			}

			// Cast to concrete container to avoid virtual calls
			auto* l_concrete_container = static_cast<bc_actor_component_container< TComponent >*>(l_component_entry->second.m_container.get());

			TComponent* l_component = static_cast< TComponent* >(l_concrete_container->get(l_actor_to_component));

			return l_component;
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
			auto l_hash = bc_actor_component_traits< TComponent >::component_hash();
			_bc_actor_component_entry l_data
			{
				p_priority,
				core::bc_vector_movale< bcINT32 >(),
				core::bc_vector_movale< bcINT32 >(),
				core::bc_make_unique< bc_actor_component_container< TComponent > >(core::bc_alloc_type::program)
			};

			m_components.insert(component_map_type::value_type(l_hash, std::move(l_data)));
		}

		template< class TComponent, class ...TDeriveds >
		void bc_actor_component_manager::_register_abstract_component_type()
		{
			static_assert
			(
				bc_actor_component_traits< TComponent >::component_is_abstract(),
				"TComponent is not an abstract component."
			);

			auto l_hash = bc_actor_component_traits< TComponent >::component_hash();
			_bc_actor_component_entry l_data
			{
				_bc_actor_component_entry::s_invalid_priority_value,
				core::bc_vector_movale< bc_actor_component_index >(),
				core::bc_vector_movale< bc_actor_index >(),
				nullptr,
				core::bc_vector_movale< _bc_actor_component_entry::deriveds_component_get_delegate >
				(
					{
						_bc_actor_component_entry::deriveds_component_get_delegate
						(
							this,
							reinterpret_cast< bc_iactor_component*(bc_actor_component_manager::*)(const bc_actor&) >
							(
								static_cast<TDeriveds*(bc_actor_component_manager::*)(const bc_actor&)>(&bc_actor_component_manager::actor_get_component< TDeriveds >)
							)
						)...
					}
				)
			};

			m_components.insert(component_map_type::value_type(l_hash, std::move(l_data)));
		}

		template< class TComponent >
		bc_actor_component_manager::component_map_type::value_type* bc_actor_component_manager::_get_component_entry()
		{
			static_assert(std::is_base_of< bc_iactor_component, TComponent >::value, "TComponent must inherit from bc_iactor_component");

			bc_actor_component_hash l_hash = bc_actor_component_traits< TComponent >::component_hash();
			auto l_entry = m_components.find(l_hash);

			if (l_entry == std::end(m_components))
			{
				return nullptr;
			}

			return &*l_entry;
		}

		template< class TComponent >
		const bc_actor_component_manager::component_map_type::value_type* bc_actor_component_manager::_get_component_entry() const
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

				// If component type haven't enough index map
				if (l_component_entry.second.m_actor_to_component_index_map.size() < m_actors.size())
				{
					l_component_entry.second.m_actor_to_component_index_map.resize(m_actors.capacity(), bc_iactor_component::invalid_index);
				}
			}
		}
	}
}
