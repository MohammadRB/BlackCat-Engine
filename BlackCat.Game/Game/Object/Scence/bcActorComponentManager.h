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
#include "Game/Object/Scence/bcActor.h"
#include "Game/Object/Scence/bcActorComponent.h"
#include "Game/Object/Scence/bcActorComponentContainer.h"

namespace black_cat
{
	namespace game
	{
		template< class TComponent >
		struct bc_actor_component_traits
		{
		public:
			static constexpr const bcCHAR* component_name()
			{
				return TComponent::component_name();
			}

			static constexpr bc_actor_component_hash component_hash()
			{
				return static_cast< bc_actor_component_hash >(core::bc_enum::value(TComponent::component_name()));
			}
		};

		struct _bc_actor_entry
		{
		public:
			_bc_actor_entry(const bc_actor& p_actor, bc_actor_index p_parent_index = g_actor_invalid_index)
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
			bcSIZE m_component_priority;
			core::bc_vector_movale< bc_actor_component_index > m_actor_to_component_index_map;
			core::bc_vector_movale< bc_actor_index > m_component_to_actor_index_map;
			core::bc_unique_ptr< bc_iactor_component_container > m_container;
		};

		class bc_actor_component_manager : public core::bc_iservice
		{
		private:
			template< class TComponent >
			friend class bc_actor_component_container;

			using actor_container_type = core::bc_vector_movale< core::bc_nullable< _bc_actor_entry > >;
			using component_map_type = core::bc_unordered_map_program< bc_actor_component_hash, _bc_actor_component_entry >;

		public:
			bc_actor_component_manager();

			bc_actor_component_manager(bc_actor_component_manager&&);

			~bc_actor_component_manager();

			bc_actor_component_manager& operator=(bc_actor_component_manager&&);

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
			
			void update(core_platform::bc_clock::update_param p_clock_update_param) override;

			template< class ...TComponent >
			void register_component_types();

			static const bcCHAR* service_name()
			{
				return core::g_srv_actor_component_manager;
			}

		protected:

		private:
			template< class TComponent >
			void _register_component_type(bcSIZE p_priority);

			template< class TComponent >
			component_map_type::value_type* _get_component_entry();

			template< class TComponent > // This function will be used by component containers during update phase
			const bc_actor& _component_get_actor(const TComponent& p_component) const noexcept;

			actor_container_type m_actors;
			component_map_type m_components;
		};

		inline bc_actor_component_manager::bc_actor_component_manager() = default;

		inline bc_actor_component_manager::bc_actor_component_manager(bc_actor_component_manager&&) = default;

		inline bc_actor_component_manager::~bc_actor_component_manager() = default;

		inline bc_actor_component_manager& bc_actor_component_manager::operator=(bc_actor_component_manager&&) = default;

		inline bc_actor bc_actor_component_manager::create_actor(const bc_actor* p_parent)
		{
			bc_actor_index l_index = 0;
			bool l_has_setted = false;

			for (core::bc_nullable< _bc_actor_entry >& l_actor : m_actors) // TODO use fast free space lookup
			{
				if (!l_actor.is_set())
				{
					l_actor.reset(_bc_actor_entry(bc_actor(this, l_index), p_parent ? p_parent->get_index() : g_actor_invalid_index));
					l_has_setted = true;
				}
				else
				{
					++l_index;
				}
			}

			if (!l_has_setted)
			{
				m_actors.push_back(core::bc_nullable< _bc_actor_entry >
					(
						_bc_actor_entry(bc_actor(this, l_index), p_parent ? p_parent->get_index() : g_actor_invalid_index)
					));
			}

			return bc_actor(this, l_index);
		}

		inline void bc_actor_component_manager::remove_actor(const bc_actor& p_actor)
		{
			bc_actor_index l_actor_index = p_actor.get_index();

			for(auto& l_component : m_components)
			{
				const _bc_actor_component_entry& l_component_entry = l_component.second;
				bc_actor_component_index l_component_index = l_component_entry.m_actor_to_component_index_map[l_actor_index];
				
				// This actor has this component type
				if(l_component_index != g_actor_component_invalid_index)
				{
					l_component_entry.m_container->remove(l_component_index);
				}
			}

			m_actors[p_actor.get_index()].reset();
		}

		template< class TComponent >
		void bc_actor_component_manager::create_component(const bc_actor& p_actor)
		{
			static component_map_type::value_type* l_component_entry = _get_component_entry< TComponent >();

			/*if (l_component_entry == nullptr)
			{
				auto l_hash = bc_actor_component_traits< TComponent >::component_hash();
				auto l_priority = bc_actor_component_traits< TComponent >::component_priority();
				_bc_actor_component_entry l_data
				{
					l_priority,
					core::bc_vector_movale< bcINT32 >(),
					core::bc_vector_movale< bcINT32 >(),
					core::bc_make_unique< bc_actor_component_container< TComponent > >()
				};

				l_component_entry = &*m_components.insert(component_map_type::value_type(l_hash, std::move(l_data))).first;
			}*/

			// Cast to concrete container to avoid virtual calls
			auto* l_concrete_container = static_cast<bc_actor_component_container< TComponent >*>(l_component_entry->second.m_container.get());

			bc_actor_index l_actor_index = p_actor.get_index();

			bcAssert(l_actor_index != g_actor_invalid_index);

			bc_actor_index l_parent_actor_index = m_actors[l_actor_index]->m_parent_index;
			bc_actor_component_index l_component_index = g_actor_component_invalid_index;

			// If actor has parent and it's parent has this component type we must create actor component
			// after parent component in sorting order of container so parent component get updated earlier
			if(l_parent_actor_index != g_actor_invalid_index)
			{
				const bc_actor& l_parent = m_actors[l_parent_actor_index]->m_actor;
				const TComponent* l_parent_component = l_parent.get_component<TComponent>();

				if(l_parent_component)
				{
					bc_actor_component_index l_parent_component_index = l_parent_component->get_index();
					l_component_index = l_concrete_container->create_after(l_parent_component_index);
				}
			}
			
			if(l_component_index == g_actor_component_invalid_index)
			{
				l_component_index = l_concrete_container->create();
			}

			// If component type haven't enaugh index map
			if (l_component_entry->second.m_actor_to_component_index_map.size() < m_actors.size())
			{
				l_component_entry->second.m_actor_to_component_index_map.resize(m_actors.capacity(), g_actor_component_invalid_index);
			}

			if (l_component_entry->second.m_component_to_actor_index_map.size() < l_component_index + 1)
			{
				l_component_entry->second.m_component_to_actor_index_map.resize(l_concrete_container->capacity(), g_actor_invalid_index);
			}

			l_component_entry->second.m_component_to_actor_index_map[l_component_index] = l_actor_index;
			l_component_entry->second.m_actor_to_component_index_map[l_actor_index] = l_component_index;
		}

		template< class TComponent >
		void bc_actor_component_manager::remove_component(const bc_actor& p_actor)
		{
			static component_map_type::value_type* l_component_entry = _get_component_entry< TComponent >();

			//// There is not any component of type TComponent
			//if (l_component_entry == nullptr)
			//{
			//	bcAssert(false);
			//	return;
			//}

			bc_actor_index l_actor_index = p_actor.get_index();

			bcAssert(l_actor_index != g_actor_invalid_index);

			bcINT32 l_actor_to_component = l_component_entry->second.m_actor_to_component_index_map[l_actor_index];
			// Actor has not this type of component
			if (l_actor_to_component == g_actor_component_invalid_index)
			{
				bcAssert(false);
				return;
			}

			// Cast to concrete container to avoid virtual calls
			auto* l_concrete_container = static_cast<bc_actor_component_container< TComponent >*>(l_component_entry->second.m_container.get());

			l_concrete_container->remove(l_actor_to_component);
		}

		template< class TComponent >
		bool bc_actor_component_manager::actor_has_component(const bc_actor& p_actor) const
		{
			return actor_get_component< TComponent >(p_actor) != nullptr;
		}

		template< class TComponent >
		TComponent* bc_actor_component_manager::actor_get_component(const bc_actor& p_actor)
		{
			static component_map_type::value_type* l_component_entry = _get_component_entry< TComponent >();

			//// There is not any component of type TComponent
			//if (l_component_entry == nullptr)
			//{
			//	// Try to get component entry from hash map again. due to usage of static initialization of
			//	// local variable it is possible to initialize local variable with null when this function 
			//	// were called for the first time
			//	l_component_entry = _get_component_entry< TComponent >();

			//	if (l_component_entry == nullptr)
			//	{
			//		return nullptr;
			//	}
			//}

			bc_actor_index l_actor_index = p_actor.get_index();

			bcAssert(l_actor_index != g_actor_invalid_index);

			bcINT32 l_actor_to_component = l_component_entry->second.m_actor_to_component_index_map[l_actor_index];
			// Actor has not this type of component
			if (l_actor_to_component == g_actor_component_invalid_index)
			{
				return nullptr;
			}

			// Cast to concrete container to avoid virtual calls
			auto* l_concrete_container = static_cast<bc_actor_component_container< TComponent >*>(l_component_entry->second.m_container.get());

			TComponent* l_component = static_cast< TComponent* >(l_concrete_container->get(l_actor_to_component));

			return l_component;
		}

		template< class TComponent >
		const TComponent* bc_actor_component_manager::actor_get_component(const bc_actor& p_actor) const
		{
			return const_cast<bc_actor_component_manager*>(this)->actor_get_component(p_actor);
		}

		inline void bc_actor_component_manager::update(core_platform::bc_clock::update_param p_clock_update_param)
		{
			core::bc_vector_frame< _bc_actor_component_entry* > l_components;
			l_components.reserve(m_components.size());

			for (auto& l_entry : m_components)
			{
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
			// Create a dummy actor and add all components to it to initialize local static variables
			// in component related functions
			bc_actor l_actor = create_actor();
			bcSIZE l_counter = 0;

			auto l_expansion_list =
				{
					(
						[this, l_actor, &l_counter]()
						{
							this->_register_component_type< TComponent >(l_counter++);

							this->create_component< TComponent >(l_actor);
							this->actor_get_component< TComponent >(l_actor);
							this->remove_component< TComponent >(l_actor);

							return true;
						}()
					)...
				};

			remove_actor(l_actor);
		}

		template< class TComponent >
		void bc_actor_component_manager::_register_component_type(bcSIZE p_priority)
		{
			// Initialize components container
			auto l_hash = bc_actor_component_traits< TComponent >::component_hash();
			auto l_priority = p_priority;
			_bc_actor_component_entry l_data
			{
				l_priority,
				core::bc_vector_movale< bcINT32 >(),
				core::bc_vector_movale< bcINT32 >(),
				core::bc_make_unique< bc_actor_component_container< TComponent > >()
			};

			m_components.insert(component_map_type::value_type(l_hash, std::move(l_data))).first;
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
		const bc_actor& bc_actor_component_manager::_component_get_actor(const TComponent& p_component) const noexcept
		{
			// This function will be used by component containers during update phase, so when this function
			// is called with TComponent argument, there will be an entry for TComponent in components hash
			// map, so it is safe to use static initialization for local variable
			static component_map_type::value_type* l_component_entry = const_cast<bc_actor_component_manager*>(this)->_get_component_entry< TComponent >();

			bc_actor_component_index l_component_index = p_component.get_index();
			bcINT32 l_component_to_actor = l_component_entry->second.m_component_to_actor_index_map[l_component_index];
			bc_actor& l_actor = m_actors[l_component_to_actor].get().m_actor;

			return l_actor;
		}
	}
}