// [05/18/2016 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Container/bcSpan.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentContainer.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_actor_component_manager;
		class bc_actor_component_manager_container;

		template<class TComponent>
		struct bc_actor_component_traits
		{
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
				return static_cast<bc_actor_component_hash>(TComponent::component_hash());
			}
		};

		template<typename TComponent>
		struct bc_component_register
		{
			using component_t = TComponent;
			
			bc_component_register(const bcCHAR* p_data_driven_name)
				: m_data_driven_name(p_data_driven_name)
			{
			}

			const bcCHAR* m_data_driven_name;
		};
		
		template<class TAbstract, class TDerived, class ...TDeriveds>
		struct bc_abstract_component_register
		{
			using abstract_component_t = TAbstract;
			template<template<typename, typename...> class T>
			using apply = T<TAbstract, TDerived, TDeriveds...>;

			void operator()(bc_actor_component_manager& p_component_manager) const;
		};

		struct bc_actor_component_register
		{
			using container_create_delegate = core::bc_delegate<core::bc_unique_ptr<bci_actor_component_container>()>;
			using deriveds_get_delegate = core::bc_delegate<void*(const bc_actor&, bc_actor_component_manager_container&)>;
			
			bc_actor_component_register(bc_actor_component_hash p_hash,
				bool p_is_abstract,
				bool p_require_event,
				bool p_require_update,
				container_create_delegate p_container_delegate,
				core::bc_vector_movable<deriveds_get_delegate> p_deriveds_delegates);

			bc_actor_component_register(const bc_actor_component_register& p_other) noexcept = default;

			bc_actor_component_register(bc_actor_component_register&& p_other) noexcept = default;

			~bc_actor_component_register() = default;

			bc_actor_component_register& operator=(const bc_actor_component_register& p_other) noexcept = default;

			bc_actor_component_register& operator=(bc_actor_component_register&& p_other) noexcept = default;

			bc_actor_component_hash m_hash;
			bool m_is_abstract;
			bool m_require_event;
			bool m_require_update;
			container_create_delegate m_container_delegate;
			core::bc_vector_movable<deriveds_get_delegate> m_deriveds_delegates;
		};

		class BC_GAME_DLL bc_actor_component_manager
		{
		private:
			template<class TAbstract, class TDerived, class ...TDeriveds>
			friend struct bc_abstract_component_register;
			using component_container_type = core::bc_unordered_map_program<bc_actor_component_hash, bc_actor_component_register>;

		public:
			bc_actor_component_manager() = default;

			bc_actor_component_manager(bc_actor_component_manager&&) noexcept = delete;

			~bc_actor_component_manager() = default;

			bc_actor_component_manager& operator=(bc_actor_component_manager&&) noexcept = delete;

			template<class TComponent>
			bc_actor_component_register* get_component_entry() noexcept;

			bc_actor_component_register* get_component_entry(bc_actor_component_hash p_hash) noexcept;
			
			core::bc_vector_frame<const bc_actor_component_register*> get_components() const noexcept;
			
			template<class ...TCAdapter>
			void register_component_types(TCAdapter... p_components);

			template<class ...TCAdapter>
			void register_abstract_component_types(TCAdapter... p_components);
			
		private:
			template<class TComponent>
			void _register_component_type();

			template<class TComponent, class ...TDeriveds>
			void _register_abstract_component_type();
			
			component_container_type m_components;
		};

		template<class TAbstract, class TDerived, class ... TDeriveds>
		void bc_abstract_component_register<TAbstract, TDerived, TDeriveds...>::operator()(bc_actor_component_manager& p_component_manager) const
		{
			p_component_manager._register_abstract_component_type<TAbstract, TDerived, TDeriveds...>();
		}
				
		inline bc_actor_component_register::bc_actor_component_register(bc_actor_component_hash p_hash,
			bool p_is_abstract,
			bool p_require_event,
			bool p_require_update,
			container_create_delegate p_container_delegate,
			core::bc_vector_movable<deriveds_get_delegate> p_deriveds_delegates)
			: m_hash(p_hash),
			m_is_abstract(p_is_abstract),
			m_require_event(p_require_event),
			m_require_update(p_require_update),
			m_container_delegate(std::move(p_container_delegate)),
			m_deriveds_delegates(std::move(p_deriveds_delegates))
		{
		}
		
		template<class TComponent>
		bc_actor_component_register* bc_actor_component_manager::get_component_entry() noexcept
		{
			static_assert(std::is_base_of_v<bci_actor_component, TComponent>, "TComponent must inherit from bc_iactor_component");

			constexpr bc_actor_component_hash l_hash = bc_actor_component_traits<TComponent>::component_hash();
			return get_component_entry(l_hash);
		}

		inline bc_actor_component_register* bc_actor_component_manager::get_component_entry(bc_actor_component_hash p_hash) noexcept
		{
			const auto l_entry = m_components.find(p_hash);
			
			if (l_entry == std::end(m_components))
			{
				return nullptr;
			}

			return &l_entry->second;
		}

		inline core::bc_vector_frame<const bc_actor_component_register*> bc_actor_component_manager::get_components() const noexcept
		{
			core::bc_vector_frame<const bc_actor_component_register*> l_result;
			l_result.reserve(m_components.size());

			std::transform
			(
				std::begin(m_components),
				std::end(m_components),
				std::back_inserter(l_result),
				[](const component_container_type::value_type& p_entry)
				{
					return &p_entry.second;
				}
			);

			return l_result;
		}
		
		template<class ...TCAdapter>
		void bc_actor_component_manager::register_component_types(TCAdapter... p_components)
		{
			auto l_expansion_list =
			{
				(
					[this]()
					{
						this->_register_component_type<typename TCAdapter::component_t>();
						return true;
					}()
				)...
			};
		}

		template<class ...TCAdapter>
		void bc_actor_component_manager::register_abstract_component_types(TCAdapter... p_components)
		{
			auto l_expansion_list =
			{
				(
					[this, p_components]()
					{
						p_components(*this);
						return true;
					}()
				)...
			};
		}

		template<class TComponent>
		void bc_actor_component_manager::_register_component_type()
		{
			static_assert(std::is_base_of_v<bci_actor_component, TComponent>, "TComponent must be inherited from bci_actor_component");
			static_assert(!bc_actor_component_traits<TComponent>::component_is_abstract(), "TComponent must not be an abstract component.");

			// Initialize components container
			constexpr auto l_hash = bc_actor_component_traits<TComponent>::component_hash();
			bc_actor_component_register l_data
			(
				l_hash,
				bc_actor_component_traits<TComponent>::component_is_abstract(),
				bc_actor_component_traits<TComponent>::component_require_event(),
				bc_actor_component_traits<TComponent>::component_require_update(),
				bc_actor_component_register::container_create_delegate
				(
					[]()
					{
						return core::bc_make_unique<bc_actor_component_container<TComponent>>();
					}
				),
				core::bc_vector_movable<bc_actor_component_register::deriveds_get_delegate>()
			);

			m_components.insert(component_container_type::value_type(l_hash, std::move(l_data)));
		}

		template<class TComponent, class ...TDeriveds>
		void bc_actor_component_manager::_register_abstract_component_type()
		{
			static_assert(std::is_base_of_v<bci_actor_abstract_component, TComponent>, "TComponent must be inherited from bci_actor_abstract_component");
			static_assert(bc_actor_component_traits<TComponent>::component_is_abstract(), "TComponent is not an abstract component.");

			constexpr auto l_hash = bc_actor_component_traits<TComponent>::component_hash();
			bc_actor_component_register l_data
			(
				l_hash,
				bc_actor_component_traits<TComponent>::component_is_abstract(),
				bc_actor_component_traits<TComponent>::component_require_event(),
				bc_actor_component_traits<TComponent>::component_require_update(),
				bc_actor_component_register::container_create_delegate(),
				core::bc_vector_movable<bc_actor_component_register::deriveds_get_delegate>
				(
					{
						bc_actor_component_register::deriveds_get_delegate
						(
							[&](const bc_actor& p_actor, bc_actor_component_manager_container& p_container)
							{
								return reinterpret_cast<void*>
								(
									static_cast<TComponent*>
									(
										p_container._actor_get_component<TDeriveds>
										(
											p_actor,
											std::integral_constant<bool, bc_actor_component_traits<TDeriveds>::component_is_abstract()>()
										)
									)
								);
							}
						)...
					}
				)
			);

			m_components.insert(component_container_type::value_type(l_hash, std::move(l_data)));
		}
	}
}