// [05/21/2016 MRB]

#pragma once

#include "Core/bcConstant.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcDelegate.hpp"
#include "Core/Utility/bcParameterPack.h"
#include "Core/Utility/bcDataDrivenParameter.h"
#include "Game/bcExport.h"
#include "Game/Object/Scence/bcActor.hpp"
#include "Game/Object/Scence/bcActorComponentManager.h"

namespace black_cat
{
	namespace game
	{
		struct _bc_entity_component_callbacks;

		struct _bc_entity_component_data
		{
			bc_actor_component_hash m_component_hash;
			core::bc_data_driven_parameter m_component_parameters;
		};

		// Make bcActorComponentManager data driven
		class BC_GAME_DLL bc_entity_manager : public core::bc_iservice
		{
			BC_SERVICE(entity_manager)

		private:
			friend struct _bc_entity_component_callbacks;
			using string_hash = std::hash< const bcCHAR* >;
			using actor_component_create_delegate = core::bc_delegate< void(const bc_actor&) >;
			using actor_component_initialize_delegate = core::bc_delegate< void(bc_actor&, const core::bc_data_driven_parameter&) >;
			using component_map_type = core::bc_unordered_map_program< bc_actor_component_hash, _bc_entity_component_callbacks >;
			using entity_map_type = core::bc_unordered_map_program< string_hash::result_type, core::bc_vector_program< _bc_entity_component_data > >;

		public:
			explicit bc_entity_manager();

			bc_entity_manager(bc_entity_manager&&);

			~bc_entity_manager();

			bc_entity_manager& operator=(bc_entity_manager&&);

			template< class ...TComponent >
			void register_component_types();

			void read_entity_file(const bcECHAR* p_json_file_path);

			// Create an uninitialized actor along with it's components
			bc_actor create_entity(const bcCHAR* p_entity_name);

			// Remove actor and all of it's components
			void remove_entity(const bc_actor& p_entity);

		protected:

		private:
			template< class TComponent >
			void _register_component_type();

			template< class TComponent >
			void _actor_component_initialization(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters) const;

			bc_actor_component_manager* m_actor_component_manager;

			component_map_type m_components;
			entity_map_type m_entities;
		};

		struct _bc_entity_component_callbacks
		{
			bc_entity_manager::actor_component_create_delegate m_create_delegate;
			bc_entity_manager::actor_component_initialize_delegate m_initialize_delegate;
		};

		template< class ...TComponent >
		void bc_entity_manager::register_component_types()
		{
			m_actor_component_manager->register_component_types<TComponent...>();

			auto l_expansion_list =
				{
					(
						[this]()
						{
							this->_register_component_type< TComponent >();
							return true;
						}()
					)...
				};
		}

		template< class TComponent >
		void bc_entity_manager::_register_component_type()
		{
			auto* l_actor_component_manager = core::bc_service_manager::get().get_service< bc_actor_component_manager >();

			bc_actor_component_hash l_hash = bc_actor_component_traits< TComponent >::component_hash();
			actor_component_create_delegate l_creatation_delegate(l_actor_component_manager, &bc_actor_component_manager::create_component< TComponent >);
			actor_component_initialize_delegate l_initialization_delegate(this, &bc_entity_manager::_actor_component_initialization< TComponent >);

			_bc_entity_component_callbacks l_component_callbacks;
			l_component_callbacks.m_create_delegate = std::move(l_creatation_delegate);
			l_component_callbacks.m_initialize_delegate = std::move(l_initialization_delegate);

			m_components.insert(component_map_type::value_type(l_hash, std::move(l_component_callbacks)));
		}

		template< class TComponent >
		void bc_entity_manager::_actor_component_initialization(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters) const
		{
			TComponent* l_component = p_actor.get_component< TComponent >();

			if (l_component)
			{
				l_component->initialize(p_parameters);
			}
		}
	}
}
