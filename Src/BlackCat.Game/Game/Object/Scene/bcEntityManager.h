// [21/05/2016 MRB]

#pragma once

#include "Core/bcConstant.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/File/bcJsonDocument.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcDelegate.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/ActorComponent/bcActorController.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentContainer.hpp"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManagerContainer.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace core
	{
		class bc_content_stream_manager;
		class bc_query_manager;
	}
	
	namespace game
	{
		struct _bc_entity_data;
		struct _bc_entity_component_data;
		struct _bc_entity_component_callbacks;
		class bc_entity_manager;
		class bc_game_system;
		class bc_scene;

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
		
		template<class ...TCAdapter>
		void bc_register_component_types(TCAdapter... p_components)
		{
			core::bc_get_service<bc_entity_manager>()->register_component_types(p_components...);
		}

		template<class ...TCAdapter>
		void bc_register_abstract_component_types(TCAdapter... p_components)
		{
			core::bc_get_service<bc_entity_manager>()->register_abstract_component_types(p_components...);
		}

		template<class ...TCAdapter>
		void bc_register_actor_controller_types(TCAdapter... p_controllers)
		{
			core::bc_get_service<bc_entity_manager>()->register_actor_controller(p_controllers...);
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
			using actor_component_container_array_type = core::bc_array<bc_actor_component_manager_container*, bc_actor_id::s_manager_index_size>;

		public:
			bc_entity_manager(core::bc_content_stream_manager& p_content_stream_manager, core::bc_query_manager& p_query_manager, bc_game_system& p_game_system);

			bc_entity_manager(bc_entity_manager&&) noexcept = delete;

			~bc_entity_manager() override;

			bc_entity_manager& operator=(bc_entity_manager&&) noexcept = delete;

			void read_entity_file(const bcECHAR* p_json_file_path);

			core::bc_vector_frame<core::bc_string_view> get_entity_names() const;

			core::bc_unique_ptr<bc_actor_component_manager_container> create_actor_component_container() noexcept;

			bc_actor_component_manager_container& get_actor_component_container(const bc_actor& p_actor) noexcept;

			/**
			 * \brief Create an actor along with it's components. In case of any error returns invalid actor
			 * \param p_scene
			 * \param p_entity_name
			 * \param p_world_transform 
			 * \return Created actor or invalid actor
			 */
			bc_actor create_entity(bc_scene& p_scene, const bcCHAR* p_entity_name, const core::bc_matrix4f& p_world_transform);

			bc_actor create_entity(bc_scene& p_scene, const bcCHAR* p_entity_name, const core::bc_matrix4f& p_world_transform, const core::bc_json_key_value& p_instance_parameters);
			
			template<class TComponent>
			void create_entity_component(bc_actor& p_actor);

			template<class TComponent>
			void create_entity_component(bc_actor& p_actor, const core::bc_json_key_value& p_instance_parameters);

			template<class ...TCAdapter>
			void register_component_types(TCAdapter... p_components);

			template<class ...TCAdapter>
			void register_abstract_component_types(TCAdapter... p_components);

			template<class ...TCAdapter>
			void register_actor_controller(TCAdapter... p_controllers);
			
		private:
			template<class TComponent>
			void _register_component_type(const bcCHAR* p_data_driven_name);

			template<class TComponent>
			void _actor_component_creation(const bc_actor& p_actor) const;

			template<class TComponent>
			void _actor_component_initialization(const bc_actor_component_initialize_context& p_context) const;

			template<class TComponent>
			void _actor_component_entity_initialization(const bc_actor_component_initialize_entity_context& p_context) const;

			bc_actor _create_entity(bc_scene& p_scene, const bcCHAR* p_entity_name, const core::bc_matrix4f& p_world_transform, const core::bc_json_key_value* p_instance_parameters);

			core::bc_content_stream_manager& m_content_stream_manager;
			core::bc_query_manager& m_query_manager;
			bc_game_system& m_game_system;
			bc_actor_component_manager m_actor_component_manager;

			bcUINT32 m_actor_component_container_index;
			actor_component_container_array_type m_actor_component_containers;

			core::bc_json_key_value m_empty_parameters;
			component_map_type m_components;
			entity_map_type m_entities;
			controller_map_type m_controllers;
		};

		struct _bc_entity_data
		{
			core::bc_string_program m_entity_name;
			core::bc_string_program m_controller_name;
			core::bc_json_key_value m_parameters;
			core::bc_vector_program<_bc_entity_component_data> m_components;
		};

		struct _bc_entity_component_data
		{
			bc_actor_component_hash m_component_hash;
			core::bc_json_key_value m_component_parameters;
		};

		struct _bc_entity_component_callbacks
		{
			bc_entity_manager::actor_component_create_delegate m_create_delegate;
			bc_entity_manager::actor_component_initialize_delegate m_initialize_delegate;
			bc_entity_manager::actor_component_initialize_entity_delegate m_initialize_entity_delegate;
		};

		inline bc_actor_component_manager_container& bc_entity_manager::get_actor_component_container(const bc_actor& p_actor) noexcept
		{
			const auto l_manager_index = bc_actor_id::decompose_id(p_actor.get_id()).m_manager_index;
			return *m_actor_component_containers[l_manager_index];
		}

		template<class TComponent>
		void bc_entity_manager::create_entity_component(bc_actor& p_actor)
		{
			create_entity_component<TComponent>(p_actor, m_empty_parameters);
		}

		template<class TComponent>
		void bc_entity_manager::create_entity_component(bc_actor& p_actor, const core::bc_json_key_value& p_instance_parameters)
		{
			const auto* l_mediate_component = p_actor.get_component<bc_mediate_component>();

			_actor_component_creation<TComponent>(p_actor);
			_actor_component_initialization<TComponent>(bc_actor_component_initialize_context
			(
				m_empty_parameters,
				p_instance_parameters,
				m_content_stream_manager,
				m_game_system,
				*l_mediate_component->get_scene(),
				p_actor,
				l_mediate_component->get_world_transform()
			));
			_actor_component_entity_initialization<TComponent>(bc_actor_component_initialize_entity_context
			(
				m_empty_parameters,
				p_instance_parameters,
				m_content_stream_manager,
				m_game_system,
				*l_mediate_component->get_scene(),
				p_actor,
				l_mediate_component->get_world_transform()
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
						this->_register_component_type<typename TCAdapter::component_t>(p_components.m_data_driven_name);
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
							static_assert(std::is_default_constructible_v<typename TCAdapter::controller_t>, "actor controller must be default constructable");
							return core::bc_make_unique<typename TCAdapter::controller_t>();
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
			actor_component_create_delegate l_creation_delegate(*this, &bc_entity_manager::_actor_component_creation<TComponent>);
			actor_component_initialize_delegate l_initialization_delegate(*this, &bc_entity_manager::_actor_component_initialization<TComponent>);
			actor_component_initialize_entity_delegate l_entity_initialization_delegate(*this, &bc_entity_manager::_actor_component_entity_initialization<TComponent>);

			_bc_entity_component_callbacks l_component_callbacks;
			l_component_callbacks.m_create_delegate = std::move(l_creation_delegate);
			l_component_callbacks.m_initialize_delegate = std::move(l_initialization_delegate);
			l_component_callbacks.m_initialize_entity_delegate = std::move(l_entity_initialization_delegate);

			m_components.insert(component_map_type::value_type(l_data_driven_hash, std::move(l_component_callbacks)));
		}

		template<class TComponent>
		void bc_entity_manager::_actor_component_creation(const bc_actor& p_actor) const
		{
			auto& l_actor_component_container = const_cast<bc_entity_manager&>(*this).get_actor_component_container(p_actor);
			l_actor_component_container.create_component<TComponent>(p_actor);
		}

		template<class TComponent>
		void bc_entity_manager::_actor_component_initialization(const bc_actor_component_initialize_context& p_context) const
		{
			if (auto* l_component = p_context.m_actor.get_component<TComponent>())
			{
				l_component->initialize(p_context);
			}
		}

		template<class TComponent>
		void bc_entity_manager::_actor_component_entity_initialization(const bc_actor_component_initialize_entity_context& p_context) const
		{
			if (auto* l_component = p_context.m_actor.get_component<TComponent>())
			{
				l_component->initialize_entity(p_context);
			}
		}
	}
}