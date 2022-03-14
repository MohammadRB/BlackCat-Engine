// [05/22/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/Container/bcString.h"
#include "Core/File/bcFileStream.h"
#include "Core/File/bcJsonDocument.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Core/Utility/bcParameterPack.h"
#include "Core/Utility/bcLogger.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/bcEntityManager.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/System/bcGameSystem.h"
#include "Game/bcException.h"

namespace black_cat
{
	namespace game
	{
		BC_JSON_STRUCTURE(_bc_json_entity_components)
		{
			BC_JSON_VALUE(core::bc_string_frame, name);
			BC_JSON_VALUE(core::bc_json_key_value, parameters);
		};

		BC_JSON_STRUCTURE(_bc_json_entity)
		{
			BC_JSON_VALUE_OP(core::bc_string_frame, inherit);
			BC_JSON_VALUE(core::bc_string_frame, name);
			BC_JSON_VALUE_OP(core::bc_string_frame, controller);
			BC_JSON_VALUE_OP(core::bc_json_key_value, parameters);
			BC_JSON_ARRAY(_bc_json_entity_components, components);
		};

		BC_JSON_STRUCTURE(_bc_entity_json)
		{
			BC_JSON_ARRAY(_bc_json_entity, entities);
		};

		template<typename THash>
		static void _read_entity_data(const _bc_json_entity& p_json, _bc_entity_data& p_entity, const _bc_entity_data* p_inherit)
		{
			auto l_hasher = THash();
			
			p_entity.m_entity_name = *p_json.m_name;
			if(p_json.m_controller.has_value())
			{
				p_entity.m_controller_name = *p_json.m_controller;
			}
			else if(p_inherit)
			{
				p_entity.m_controller_name = p_inherit->m_controller_name;
			}

			if(p_inherit)
			{
				p_entity.m_parameters = p_inherit->m_parameters;
			}
			
			std::for_each
			(
				std::begin(*p_json.m_parameters),
				std::end(*p_json.m_parameters),
				[&](core::bc_json_key_value::value_type& p_parameter)
				{
					p_entity.m_parameters.add_or_update(p_parameter.first.c_str(), std::move(p_parameter.second));
				}
			);

			// Because we have used program heap we must reserve needed memory
			const auto l_components_count = (p_inherit ? p_inherit->m_components.size() : 0) + p_json.m_components.size();
			p_entity.m_components.reserve(l_components_count);

			if(p_inherit)
			{
				std::copy
				(
					std::begin(p_inherit->m_components), 
					std::end(p_inherit->m_components),
					std::back_inserter(p_entity.m_components)
				);
			}
			
			for (auto& l_json_component : p_json.m_components)
			{
				const auto l_component_name_hash = l_hasher(l_json_component->m_name->c_str());
				auto l_component_ite = std::find_if
				(
					std::begin(p_entity.m_components),
					std::end(p_entity.m_components),
					[=](const _bc_entity_component_data& p_data)
					{
						return p_data.m_component_hash == l_component_name_hash;
					}
				);
				if(l_component_ite == std::end(p_entity.m_components))
				{
					_bc_entity_component_data l_component_data;
					l_component_data.m_component_hash = l_component_name_hash;
					
					p_entity.m_components.push_back(l_component_data);
					l_component_ite = std::rbegin(p_entity.m_components).base();
				}

				auto& l_component_parameters = l_component_ite->m_component_parameters;
				auto& l_json_component_parameters = *l_json_component->m_parameters;
				
				std::for_each
				(
					std::begin(l_json_component_parameters),
					std::end(l_json_component_parameters),
					[&](core::bc_json_key_value::value_type& p_parameter)
					{
						l_component_parameters.add_or_update(p_parameter.first.c_str(), std::move(p_parameter.second));
					}
				);
			}
		}
		
		bc_entity_manager::bc_entity_manager(core::bc_content_stream_manager& p_content_stream_manager, bc_actor_component_manager& p_actor_manager, bc_game_system& p_game_system)
			: m_content_stream_manager(p_content_stream_manager),
			m_actor_component_manager(p_actor_manager),
			m_game_system(p_game_system)
		{
		}

		bc_entity_manager::~bc_entity_manager() = default;

		void bc_entity_manager::read_entity_file(const bcECHAR* p_json_file_path)
		{
			core::bc_file_stream l_json_file;
			core::bc_string_frame l_json_file_buffer;

			l_json_file.open_read(p_json_file_path);
			core::bc_read_all_lines(l_json_file, l_json_file_buffer);

			core::bc_json_document<_bc_entity_json> l_json;
			l_json.load(l_json_file_buffer.c_str());

			for (auto& l_entity : l_json->m_entities)
			{
				auto l_entity_name_hash = string_hash()(l_entity->m_name->c_str());
				
				_bc_entity_data l_entity_data;
				_bc_entity_data* p_inherit = nullptr;

				if(l_entity->m_inherit.has_value())
				{
					auto l_inherit_entity_name_hash = string_hash()(l_entity->m_inherit->c_str());
					const auto l_inherit_ite = m_entities.find(l_inherit_entity_name_hash);
					if(l_inherit_ite != std::end(m_entities))
					{
						p_inherit = &l_inherit_ite->second;
					}
				}

				_read_entity_data<string_hash>(*l_entity, l_entity_data, p_inherit);
				
				entity_map_type::value_type l_entity_value(l_entity_name_hash, std::move(l_entity_data));
				m_entities.insert(std::move(l_entity_value));
			}
		}

		core::bc_vector_frame<core::bc_string_view> bc_entity_manager::get_entity_names() const
		{
			core::bc_vector_frame<core::bc_string_view> l_result;
			l_result.reserve(m_entities.size());

			for (const auto& l_entity : m_entities)
			{
				l_result.push_back(l_entity.second.m_entity_name);
			}

			std::sort(std::begin(l_result), std::end(l_result));
			
			return l_result;
		}

		bc_actor bc_entity_manager::create_entity(bc_scene& p_scene, const bcCHAR* p_entity_name, const core::bc_matrix4f& p_world_transform)
		{
			return _create_entity(p_scene, p_entity_name, p_world_transform, nullptr);
		}

		bc_actor bc_entity_manager::create_entity(bc_scene& p_scene, const bcCHAR* p_entity_name, const core::bc_matrix4f& p_world_transform, const core::bc_data_driven_parameter& p_instance_parameters)
		{
			return _create_entity(p_scene, p_entity_name, p_world_transform , &p_instance_parameters);
		}

		void bc_entity_manager::remove_entity(const bc_actor& p_entity)
		{
			m_actor_component_manager.remove_actor(p_entity);
		}

		bc_actor bc_entity_manager::_create_entity(bc_scene& p_scene, const bcCHAR* p_entity_name, const core::bc_matrix4f& p_world_transform, const core::bc_data_driven_parameter* p_instance_parameters)
		{
			const auto l_hash = string_hash()(p_entity_name);
			const auto l_entity_entry = m_entities.find(l_hash);
			const auto& l_instance_parameters = p_instance_parameters ? *p_instance_parameters : m_empty_parameters;
			bc_actor l_actor;

			try
			{
				if (l_entity_entry == std::end(m_entities))
				{
					throw bc_invalid_argument_exception("Invalid entity name");
				}

				l_actor = m_actor_component_manager.create_actor();
				m_actor_component_manager.create_component<bc_mediate_component>(l_actor);

				for (auto& l_entity_component_data : l_entity_entry->second.m_components)
				{
					auto l_entity_component_entry = m_components.find(l_entity_component_data.m_component_hash);
					if (l_entity_component_entry == std::end(m_components))
					{
						throw bc_key_not_found_exception("There is no registered component with specified name");
					}

					l_entity_component_entry->second.m_create_delegate(l_actor);
				}

				auto* l_mediate_component = l_actor.get_component<bc_mediate_component>();
				l_mediate_component->set_entity_name(l_entity_entry->second.m_entity_name.c_str());
				l_mediate_component->initialize
				(
					bc_actor_component_initialize_context
					(
						l_entity_entry->second.m_parameters,
						l_instance_parameters,
						m_content_stream_manager,
						m_game_system,
						p_scene,
						l_actor,
						p_world_transform
					)
				);

				for (auto& l_entity_component_data : l_entity_entry->second.m_components)
				{
					const auto l_entity_component_entry = m_components.find(l_entity_component_data.m_component_hash);
					l_entity_component_entry->second.m_initialize_delegate
					(
						bc_actor_component_initialize_context
						(
							l_entity_component_data.m_component_parameters,
							l_instance_parameters,
							m_content_stream_manager,
							m_game_system,
							p_scene,
							l_actor,
							p_world_transform
						)
					);
				}

				l_mediate_component->initialize_entity
				(
					bc_actor_component_initialize_entity_context
					(
						l_entity_entry->second.m_parameters,
						l_instance_parameters,
						m_content_stream_manager,
						m_game_system,
						p_scene,
						l_actor,
						p_world_transform
					)
				);

				for (auto& l_entity_component_data : l_entity_entry->second.m_components)
				{
					const auto l_entity_component_entry = m_components.find(l_entity_component_data.m_component_hash);
					l_entity_component_entry->second.m_initialize_entity_delegate
					(
						bc_actor_component_initialize_entity_context
						(
							l_entity_component_data.m_component_parameters,
							l_instance_parameters,
							m_content_stream_manager,
							m_game_system,
							p_scene,
							l_actor,
							p_world_transform
						)
					);
				}

				if (!l_entity_entry->second.m_controller_name.empty())
				{
					const auto l_controller_hash = string_hash()(l_entity_entry->second.m_controller_name.c_str());
					const auto l_controller_ite = m_controllers.find(l_controller_hash);
					if (l_controller_ite == std::end(m_controllers))
					{
						throw bc_invalid_argument_exception("Invalid actor controller name");
					}

					auto l_controller = l_controller_ite->second();
					l_mediate_component->set_controller
					(
						std::move(l_controller),
						bc_actor_component_initialize_context
						(
							l_entity_entry->second.m_parameters,
							l_instance_parameters,
							m_content_stream_manager,
							m_game_system,
							p_scene,
							l_actor,
							p_world_transform
						)
					);
				}
			}
			catch (const std::exception & p_exception)
			{
				if (l_actor.is_valid())
				{
					m_actor_component_manager.remove_actor(l_actor);
					l_actor = bc_actor();
				}

				core::bc_log(core::bc_log_type::error) << "Error in entity creation: '" << p_entity_name << "': " << p_exception.what() << core::bc_lend;
			}

			return l_actor;
		}
	}
}