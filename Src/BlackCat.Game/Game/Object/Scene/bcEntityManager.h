// [05/21/2016 MRB]

#pragma once

#include "Core/bcConstant.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcDelegate.hpp"
#include "Core/Utility/bcDataDrivenParameter.h"
#include "Game/bcExport.h"
#include "Game/Object/Scene/ActorComponent/bcActorController.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"

namespace black_cat
{
	namespace core
	{
		class bc_content_stream_manager;
	}
	
	namespace game
	{
		struct _bc_entity_data;
		struct _bc_entity_component_data;
		struct _bc_entity_component_callbacks;
		class bc_entity_manager;
		class bc_game_system;

		template<typename TController>
		class bc_actor_controller_register
		{
		public:
			using controller_t = TController;

			bc_actor_controller_register(const bcCHAR* p_data_driven_name)
				: m_data_driven_name(p_data_driven_name)
			{
			}

			const bcCHAR* m_data_driven_name;
		};
		
		template< class ...TCAdapter >
		void bc_register_component_types(TCAdapter... p_components)
		{
			core::bc_get_service< bc_entity_manager >()->register_component_types(p_components...);
		}

		template< class ...TCAdapter >
		void bc_register_abstract_component_types(TCAdapter... p_components)
		{
			core::bc_get_service< bc_entity_manager >()->register_abstract_component_types(p_components...);
		}

		template< class ...TCAdapter >
		void bc_register_actor_controller_types(TCAdapter... p_controllers)
		{
			core::bc_get_service< bc_entity_manager >()->register_actor_controller(p_controllers...);
		}
		
		/**
		 * \brief Make bcActorComponentManager data driven
		 */
		class BC_GAME_DLL bc_entity_manager : public core::bci_service
		{
			BC_SERVICE(ent_mng)

		private:
			friend struct _bc_entity_component_callbacks;
			using string_hash = std::hash<const bcCHAR*>;
			using actor_component_create_delegate = core::bc_delegate<void(const bc_actor&)>;
			using actor_component_initialize_delegate = core::bc_delegate<void(const bc_actor_component_initialize_context&)>;
			using actor_component_initialize_entity_delegate = core::bc_delegate<void(const bc_actor_component_initialize_entity_context&)>;
			using actor_controller_create_delegate = core::bc_delegate<core::bc_unique_ptr<bci_actor_controller>()>;
			using component_map_type = core::bc_unordered_map_program<bc_actor_component_hash, _bc_entity_component_callbacks>;
			using entity_map_type = core::bc_unordered_map_program<string_hash::result_type, _bc_entity_data>;
			using controller_map_type = core::bc_unordered_map_program<string_hash::result_type, actor_controller_create_delegate>;

		public:
			bc_entity_manager(core::bc_content_stream_manager& p_content_stream_manager, bc_actor_component_manager& p_actor_manager, bc_game_system& p_game_system);

			bc_entity_manager(bc_entity_manager&&) noexcept = delete;

			~bc_entity_manager();

			bc_entity_manager& operator=(bc_entity_manager&&) noexcept = delete;

			void read_entity_file(const bcECHAR* p_json_file_path);

			core::bc_vector_frame<const bcCHAR*> get_entity_names() const;

			/**
			 * \brief Create an actor along with it's components
			 * \param p_entity_name 
			 * \return 
			 */
			bc_actor create_entity(const bcCHAR* p_entity_name);

			/**
			 * \brief Remove actor and all of it's components
			 * \param p_entity 
			 */
			void remove_entity(const bc_actor& p_entity);

			template<class TComponent>
			void create_entity_component(bc_actor& p_actor);

			template<class ...TCAdapter>
			void register_component_types(TCAdapter ... p_components);

			template<class ...TCAdapter>
			void register_abstract_component_types(TCAdapter ... p_components);

			template<class ...TCAdapter>
			void register_actor_controller(TCAdapter ... p_controllers);
			
		private:
			template<class TComponent>
			void _register_component_type(const bcCHAR* p_data_driven_name);

			template<class TComponent>
			void _actor_component_initialization(const bc_actor_component_initialize_context& p_context) const;

			template<class TComponent>
			void _actor_component_entity_initialization(const bc_actor_component_initialize_entity_context& p_context) const;

			core::bc_content_stream_manager& m_content_stream_manager;
			bc_actor_component_manager& m_actor_component_manager;
			bc_game_system& m_game_system;
			component_map_type m_components;
			entity_map_type m_entities;
			controller_map_type m_controllers;
		};

		struct _bc_entity_data
		{
			core::bc_string_program m_entity_name;
			core::bc_string_program m_controller_name;
			core::bc_data_driven_parameter m_parameters;
			core::bc_vector_program< _bc_entity_component_data > m_components;
		};

		struct _bc_entity_component_data
		{
			bc_actor_component_hash m_component_hash;
			core::bc_data_driven_parameter m_component_parameters;
		};

		struct _bc_entity_component_callbacks
		{
			bc_entity_manager::actor_component_create_delegate m_create_delegate;
			bc_entity_manager::actor_component_initialize_delegate m_initialize_delegate;
			bc_entity_manager::actor_component_initialize_entity_delegate m_initialize_entity_delegate;
		};

		template<class TComponent>
		void bc_entity_manager::create_entity_component(bc_actor& p_actor)
		{
			m_actor_component_manager.create_component<TComponent>(p_actor);
			_actor_component_initialization<TComponent>(bc_actor_component_initialize_context
			(
				core::bc_data_driven_parameter(),
				m_content_stream_manager,
				m_game_system,
				p_actor
			));
			_actor_component_entity_initialization<TComponent>(bc_actor_component_initialize_entity_context
			(
				m_content_stream_manager,
				m_game_system,
				p_actor
			));
		}

		template<class ...TCAdapter>
		void bc_entity_manager::register_component_types(TCAdapter... p_components)
		{
			m_actor_component_manager.register_component_types(p_components...);

			auto l_expansion_list =
			{
				(
					[this, p_components]()
					{
						this->_register_component_type< typename TCAdapter::component_t >(p_components.m_data_driven_name);
						return true;
					}()
				)...
			};
		}

		template<class ...TCAdapter>
		void bc_entity_manager::register_abstract_component_types(TCAdapter... p_components)
		{
			m_actor_component_manager.register_abstract_component_types(p_components...);
		}

		template<class ... TCAdapter>
		void bc_entity_manager::register_actor_controller(TCAdapter... p_controllers)
		{
			auto l_expansion_list =
			{
				(
					[this, p_controllers]()
					{
						bc_actor_component_hash l_data_driven_hash = string_hash()(p_controllers.m_data_driven_name);
						actor_controller_create_delegate l_create_delegate = []()
						{
							static_assert(std::is_default_constructible_v< typename TCAdapter::controller_t>, "actor controller must be default constructible");
							return core::bc_make_unique< typename TCAdapter::controller_t >();
						};
						
						m_controllers.insert(std::make_pair(l_data_driven_hash, std::move(l_create_delegate)));
						return true;
					}()
				)...
			};
		}

		template<class TComponent>
		void bc_entity_manager::_register_component_type(const bcCHAR* p_data_driven_name)
		{
			bc_actor_component_hash l_data_driven_hash = string_hash()(p_data_driven_name);
			actor_component_create_delegate l_creation_delegate(m_actor_component_manager, &bc_actor_component_manager::create_component<TComponent>);
			actor_component_initialize_delegate l_initialization_delegate(*this, &bc_entity_manager::_actor_component_initialization<TComponent>);
			actor_component_initialize_entity_delegate l_entity_initialization_delegate(*this, &bc_entity_manager::_actor_component_entity_initialization<TComponent>);

			_bc_entity_component_callbacks l_component_callbacks;
			l_component_callbacks.m_create_delegate = std::move(l_creation_delegate);
			l_component_callbacks.m_initialize_delegate = std::move(l_initialization_delegate);
			l_component_callbacks.m_initialize_entity_delegate = std::move(l_entity_initialization_delegate);

			m_components.insert(component_map_type::value_type(l_data_driven_hash, std::move(l_component_callbacks)));
		}

		template<class TComponent>
		void bc_entity_manager::_actor_component_initialization(const bc_actor_component_initialize_context& p_context) const
		{
			auto* l_component = p_context.m_actor.get_component<TComponent>();
			if (l_component)
			{
				l_component->initialize(p_context);
			}
		}

		template<class TComponent>
		void bc_entity_manager::_actor_component_entity_initialization(const bc_actor_component_initialize_entity_context& p_context) const
		{
			auto* l_component = p_context.m_actor.get_component<TComponent>();
			if (l_component)
			{
				l_component->initialize_entity(p_context);
			}
		}
	}
}