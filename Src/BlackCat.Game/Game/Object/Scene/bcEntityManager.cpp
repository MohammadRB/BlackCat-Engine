// [05/22/2016 MRB]

#include "Game/GamePCH.h"

#include "CorePlatform/bcException.h"
#include "Core/Container/bcString.h"
#include "Core/File/bcFileStream.h"
#include "Core/File/bcJsonDocument.h"
#include "Core/Utility/bcParameterPack.h"
#include "Game/Object/Scene/bcActor.h"
#include "Game/Object/Scene/bcActorComponentManager.h"
#include "Game/Object/Scene/bcEntityManager.h"
#include "Game/Object/Scene/Component/bcNameComponent.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"

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
			BC_JSON_ARRAY(_bc_json_entity_components, components);
		};

		BC_JSON_STRUCTURE(_bc_entity_json)
		{
			BC_JSON_ARRAY(_bc_json_entity, entities);
		};

		bc_entity_manager::bc_entity_manager(bc_actor_component_manager& p_actor_manager)
			: m_actor_component_manager(p_actor_manager)
		{
		}

		bc_entity_manager::~bc_entity_manager()
		{
		}

		void bc_entity_manager::read_entity_file(const bcECHAR* p_json_file_path)
		{
			core::bc_file_stream l_json_file;
			core::bc_string_frame l_buffer;

			l_json_file.open_read(p_json_file_path);

			core::bc_string_frame l_line;
			while (core::bc_get_line(l_json_file, l_line))
			{
				l_buffer.append(l_line);
			}

			core::bc_json_document< _bc_entity_json > l_json;
			l_json.load(l_buffer.c_str());

			for (auto& l_entity : l_json->m_entities)
			{
				const core::bc_string_frame& l_entity_name = *l_entity->m_name;
				auto l_entity_name_hash = string_hash()(l_entity_name.c_str());
				entity_map_type::value_type::second_type l_entity_components;

				// Because we used program heap we must reserve needed memory
				l_entity_components.m_components.reserve(l_entity->m_components.size());
				l_entity_components.m_entity_name = l_entity_name.c_str();

				for (auto& l_component : l_entity->m_components)
				{
					const core::bc_string_frame& l_component_name = *l_component->m_name;
					// We have used this function in component name hashing
					const auto l_component_name_hash = bc_run_time_string_hash(l_component_name.c_str(), l_component_name.size());

					_bc_entity_component_data l_component_data;
					l_component_data.m_component_hash = l_component_name_hash;

					auto& l_exp_params = *l_component->m_parameters;

					std::for_each
					(
						std::begin(l_exp_params),
						std::end(l_exp_params),
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

			return l_result;
		}

		bc_actor bc_entity_manager::create_entity(const bcCHAR* p_entity_name)
		{
			const auto l_hash = string_hash()(p_entity_name);
			auto l_entity_entry = m_entities.find(l_hash);

			if(l_entity_entry == std::end(m_entities))
			{
				throw bc_invalid_argument_exception("Invalid entity name");
			}

			bc_actor l_actor = m_actor_component_manager.create_actor();
			l_actor.create_component<bc_mediate_component>();
			l_actor.create_component<bc_name_component>();
			l_actor.get_component<bc_name_component>()->set_entity_name(l_entity_entry->second.m_entity_name.c_str());

			for (auto& l_entity_component_data : l_entity_entry->second.m_components)
			{
				auto l_entity_component_entry = m_components.find(l_entity_component_data.m_component_hash);

				l_entity_component_entry->second.m_create_delegate(l_actor);
				l_entity_component_entry->second.m_initialize_delegate(l_actor, l_entity_component_data.m_component_parameters);
			}

			return l_actor;
		}

		void bc_entity_manager::remove_entity(const bc_actor& p_entity)
		{
			m_actor_component_manager.remove_actor(p_entity);
		}
	}
}