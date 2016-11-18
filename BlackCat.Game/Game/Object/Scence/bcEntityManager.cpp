// [05/22/2016 MRB]

#include "Game/GamePCH.h"

#include "CorePlatform/bcException.h"
#include "Core/Container/bcString.h"
#include "Core/File/bcFileStream.h"
#include "Core/File/bcJsonDocument.h"
#include "Core/Utility/bcParameterPack.h"
#include "Game/Object/Scence/bcActor.h"
#include "Game/Object/Scence/bcActorComponentManager.h"
#include "Game/Object/Scence/bcEntityManager.h"

namespace black_cat
{
	namespace game
	{
		/*BC_JSON_STRUCTURE(_bc_json_entity_component_parameters)
		{
			BC_JSON_VALUE(core::bc_string_frame, name);
			BC_JSON_VALUE(core::bc_parameter_pack, value);
		};*/

		BC_JSON_STRUCTURE(_bc_json_entity_components)
		{
			BC_JSON_VALUE(core::bc_string_frame, name);
			//BC_JSON_ARRAY(_bc_json_entity_component_parameters, parameters);
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

		bc_entity_manager::bc_entity_manager()
			: m_actor_component_manager(core::bc_service_manager::get().get_service< bc_actor_component_manager >())
		{
		}

		bc_entity_manager::bc_entity_manager(bc_entity_manager&& p_other)
			: m_actor_component_manager(p_other.m_actor_component_manager),
			m_components(std::move(p_other.m_components)),
			m_entities(std::move(p_other.m_entities))
		{
		}

		bc_entity_manager::~bc_entity_manager()
		{
		}

		bc_entity_manager& bc_entity_manager::operator=(bc_entity_manager&& p_other)
		{
			m_actor_component_manager= p_other.m_actor_component_manager;
			m_components = std::move(p_other.m_components);
			m_entities = std::move(p_other.m_entities);

			return *this;
		}

		void bc_entity_manager::read_entity_file(const bcECHAR* p_json_file_path)
		{
			core::bc_file_stream l_json_file;
			core::bc_string_frame l_buffer;

			l_json_file.open_read(p_json_file_path);

			core::bc_string_frame l_line;
			while (core::bc_get_line(l_json_file, &l_line))
			{
				l_buffer.append(l_line);
			}

			core::bc_json_document< _bc_entity_json > l_json;
			l_json.parse(l_buffer.c_str());

			for (auto& l_entity : *l_json->m_entities)
			{
				core::bc_string_frame& l_entity_name = *l_entity->m_name;
				auto l_entity_name_hash = string_hash()(l_entity_name.c_str());
				entity_map_type::value_type::second_type l_entity_components;

				// Because we used program heap we must reserve needed memory
				l_entity_components.reserve(l_entity->m_components->size());

				for (auto& l_component : *l_entity->m_components)
				{
					core::bc_string_frame& l_component_name = *l_component->m_name;
					// We used this function in component name hashing
					auto l_component_name_hash = bc_run_time_string_hash(l_component_name.c_str(), l_component_name.size());

					_bc_entity_component_data l_component_data;
					l_component_data.m_component_hash = l_component_name_hash;

					auto* l_exp_params = &*l_component->m_parameters;

					std::for_each
					(
						std::begin(l_exp_params->m_key_values),
						std::end(l_exp_params->m_key_values),
						[&](core::bc_json_key_value::key_value_array_t::value_type& p_parameter)
						{
							core::bc_any l_value;

							auto* l_exp_param = p_parameter.second.as< core::bc_expression_parameter >();
							if (l_exp_param != nullptr)
							{
								l_value = l_exp_param->evaluate();
							}
							else
							{
								l_value = p_parameter.second;
							}

							l_component_data.m_component_parameters.add_value(p_parameter.first.c_str(), std::move(l_value));
						}
					);

					l_entity_components.push_back(std::move(l_component_data));
				}

				entity_map_type::value_type l_entity_value(l_entity_name_hash, std::move(l_entity_components));
				m_entities.insert(std::move(l_entity_value));
			}
		}

		bc_actor bc_entity_manager::create_entity(const const bcCHAR* p_entity_name)
		{
			auto l_hash = string_hash()(p_entity_name);
			auto l_entity_entry = m_entities.find(l_hash);

			if(l_entity_entry == std::end(m_entities))
			{
				throw bc_invalid_argument_exception("Invalid entity name");
			}

			bc_actor l_actor = m_actor_component_manager->create_actor();

			for (auto& l_entity_component_data : l_entity_entry->second)
			{
				auto l_entity_component_entry = m_components.find(l_entity_component_data.m_component_hash);

				l_entity_component_entry->second.m_create_delegate(l_actor);
				l_entity_component_entry->second.m_initialize_delegate(l_actor, l_entity_component_data.m_component_parameters);
			}

			return l_actor;
		}

		void bc_entity_manager::remove_entity(const bc_actor& p_entity)
		{
			m_actor_component_manager->remove_actor(p_entity);
		}
	}
}
