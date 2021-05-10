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
			BC_JSON_VALUE(core::bc_string_frame, name);
			BC_JSON_VALUE_OP(core::bc_string_frame, controller);
			BC_JSON_VALUE_OP(core::bc_json_key_value, parameters);
			BC_JSON_ARRAY(_bc_json_entity_components, components);
		};

		BC_JSON_STRUCTURE(_bc_entity_json)
		{
			BC_JSON_ARRAY(_bc_json_entity, entities);
		};

		bc_entity_manager::bc_entity_manager(core::bc_content_stream_manager& p_content_stream_manager, bc_actor_component_manager& p_actor_manager, bc_game_system& p_game_system)
			: m_content_stream_manager(p_content_stream_manager),
			m_actor_component_manager(p_actor_manager),
			m_game_system(p_game_system)
		{
		}

		bc_entity_manager::~bc_entity_manager()
		{
		}

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
				const auto& l_entity_name = *l_entity->m_name;
				const auto& l_controller_name = *l_entity->m_controller;
				auto l_entity_name_hash = string_hash()(l_entity_name.c_str());
				entity_map_type::value_type::second_type l_entity_components;

				// Because we have used program heap we must reserve needed memory
				l_entity_components.m_entity_name = l_entity_name.c_str();
				l_entity_components.m_controller_name = l_controller_name.c_str();

				std::for_each
				(
					std::begin(*l_entity->m_parameters),
					std::end(*l_entity->m_parameters),
					[&](core::bc_json_key_value::value_type& p_parameter)
					{
						l_entity_components.m_parameters.add_value(p_parameter.first.c_str(), std::move(p_parameter.second));
					}
				);
				
				l_entity_components.m_components.reserve(l_entity->m_components.size());
				for (auto& l_component : l_entity->m_components)
				{
					const auto& l_component_name = *l_component->m_name;
					const auto l_component_name_hash = string_hash()(l_component_name.c_str());
					auto& l_component_parameters = *l_component->m_parameters;

					_bc_entity_component_data l_component_data;
					l_component_data.m_component_hash = l_component_name_hash;

					std::for_each
					(
						std::begin(l_component_parameters),
						std::end(l_component_parameters),
						[&](core::bc_json_key_value::value_type& p_parameter)
						{
							l_component_data.m_component_parameters.add_value(p_parameter.first.c_str(), std::move(p_parameter.second));
						}
					);

					l_entity_components.m_components.push_back(std::move(l_component_data));
				}

				entity_map_type::value_type l_entity_value(l_entity_name_hash, std::move(l_entity_components));
				m_entities.insert(std::move(l_entity_value));
			}
		}

		core::bc_vector_frame<const bcCHAR*> bc_entity_manager::get_entity_names() const
		{
			core::bc_vector_frame<const bcCHAR*> l_result;
			l_result.reserve(m_entities.size());

			for (const auto& l_entity : m_entities)
			{
				l_result.push_back(l_entity.second.m_entity_name.c_str());
			}

			std::sort(std::begin(l_result), std::end(l_result), [](const bcCHAR* p_first, const bcCHAR* p_second)
			{
				return std::strcmp(p_first, p_second) <= 0;
			});
			
			return l_result;
		}

		bc_actor bc_entity_manager::create_entity(bc_scene& p_scene, const bcCHAR* p_entity_name)
		{
			const auto l_hash = string_hash()(p_entity_name);
			auto l_entity_entry = m_entities.find(l_hash);

			if(l_entity_entry == std::end(m_entities))
			{
				throw bc_invalid_argument_exception("Invalid entity name");
			}

			bc_actor l_actor = m_actor_component_manager.create_actor();
			
			try
			{
				m_actor_component_manager.create_component<bc_mediate_component>(l_actor);
				auto* l_mediate_component = l_actor.get_component<bc_mediate_component>();
				l_mediate_component->set_entity_name(l_entity_entry->second.m_entity_name.c_str());
				l_mediate_component->initialize
				(
					bc_actor_component_initialize_context
					(
						l_entity_entry->second.m_parameters,
						m_content_stream_manager,
						m_game_system,
						p_scene,
						l_actor
					)
				);
				
				for (auto& l_entity_component_data : l_entity_entry->second.m_components)
				{
					auto l_entity_component_entry = m_components.find(l_entity_component_data.m_component_hash);
					if(l_entity_component_entry == std::end(m_components))
					{
						throw bc_key_not_found_exception("There is no component registered with specified name");
					}

					l_entity_component_entry->second.m_create_delegate(l_actor);
					l_entity_component_entry->second.m_initialize_delegate
					(
						bc_actor_component_initialize_context
						(
							l_entity_component_data.m_component_parameters,
							m_content_stream_manager,
							m_game_system,
							p_scene,
							l_actor
						)
					);
				}

				for (auto& l_entity_component_data : l_entity_entry->second.m_components)
				{
					auto l_entity_component_entry = m_components.find(l_entity_component_data.m_component_hash);

					l_entity_component_entry->second.m_initialize_entity_delegate
					(
						bc_actor_component_initialize_entity_context
						(
							l_entity_component_data.m_component_parameters,
							m_content_stream_manager,
							m_game_system,
							p_scene,
							l_actor
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

					l_mediate_component->set_controller
					(
						l_controller_ite->second(),
						bc_actor_component_initialize_context
						(
							l_entity_entry->second.m_parameters,
							m_content_stream_manager,
							m_game_system,
							p_scene,
							l_actor
						)
					);
				}
			}
			catch (const std::exception& p_exception)
			{
				m_actor_component_manager.remove_actor(l_actor);

				auto l_msg = core::bc_string_frame("Error in creation of entity '") + p_entity_name + "': " + p_exception.what();
				core::bc_log(core::bc_log_type::error, core::bc_to_estring_frame(l_msg).c_str());

				l_actor = bc_actor();
				//throw;
			}

			return l_actor;
		}

		void bc_entity_manager::remove_entity(const bc_actor& p_entity)
		{
			m_actor_component_manager.remove_actor(p_entity);
		}
	}
}