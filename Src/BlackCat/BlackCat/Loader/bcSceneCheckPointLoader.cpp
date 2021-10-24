// [04/23/2021 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/File/bcJsonDocument.h"
#include "Core/Utility/bcJsonParse.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/bcSceneCheckPoint.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/bcUtility.h"
#include "BlackCat/Loader/bcSceneCheckPointLoader.h"

namespace black_cat
{
	BC_JSON_STRUCTURE(_bc_scene_check_point_actor)
	{
		BC_JSON_VALUE(core::bc_string_frame, entity_name);
		BC_JSON_VALUE(core::bc_vector3f, position);
		BC_JSON_VALUE(core::bc_vector3f, rotation);
		BC_JSON_VALUE_OP(core::bc_json_key_value, parameters);
	};

	BC_JSON_STRUCTURE(_bc_scene_check_point_json)
	{
		BC_JSON_ARRAY(_bc_scene_check_point_actor, actors);
	};
	
	bool bc_scene_check_point_loader::support_offline_processing() const
	{
		return false;
	}

	void bc_scene_check_point_loader::content_processing(core::bc_content_loading_context& p_context) const
	{
		auto* l_scene = const_cast<game::bc_scene*>(p_context.m_instance_parameters.get_value_throw<game::bc_scene*>("scene"));
		
		core::bc_json_document<_bc_scene_check_point_json> l_json_document;
		const core::bc_string_frame l_json_str(p_context.m_file_buffer_size + 1, '\0');
		std::memcpy(const_cast<bcCHAR*>(l_json_str.data()), p_context.m_file_buffer.get(), p_context.m_file_buffer_size);
		
		l_json_document.load(l_json_str.c_str());

		game::bc_scene_check_point l_check_point(*l_scene);
		l_check_point.remove_dynamic_actors();

		core::bc_vector_frame<game::bci_actor_component*> l_actor_components;
		core::bc_vector_frame<game::bc_actor> l_actors;
		l_actors.reserve(l_json_document->m_actors.size());
		
		for (auto& l_json_actor : l_json_document->m_actors)
		{
			auto l_transform = core::bc_matrix4f::identity();
			l_transform.set_translation(*l_json_actor->m_position);
			l_transform.set_rotation(bc_matrix3f_rotation_zyx(*l_json_actor->m_rotation));
			
			game::bc_actor l_actor = l_scene->create_actor(l_json_actor->m_entity_name->c_str(), l_transform);
			l_actor.get_components(std::back_inserter(l_actor_components));

			for (auto* l_actor_component : l_actor_components)
			{
				l_actor_component->load_instance(game::bc_actor_component_load_context(*l_json_actor->m_parameters, l_actor));
			}

			l_actors.push_back(l_actor);
			l_actor_components.clear();
		}

		p_context.set_result(std::move(l_check_point));
	}

	void bc_scene_check_point_loader::content_processing(core::bc_content_saving_context& p_context) const
	{
		auto* l_check_point = static_cast<game::bc_scene_check_point*>(p_context.m_content);
		auto l_dynamic_actors = l_check_point->export_dynamic_actors();

		core::bc_json_document<_bc_scene_check_point_json> l_json_document;
		core::bc_vector_frame<game::bci_actor_component*> l_actor_components;
		
		for (auto& l_actor : l_dynamic_actors)
		{
			auto* l_mediate_component = l_actor.get_component<game::bc_mediate_component>();
			auto& l_json_entry = l_json_document->m_actors.new_entry();

			l_actor.get_components(std::back_inserter(l_actor_components));
			for (auto& l_component : l_actor_components)
			{
				l_component->write_instance(game::bc_actor_component_write_context(*l_json_entry->m_parameters, l_actor));
			}

			const auto& l_world_transform = l_mediate_component->get_world_transform();
			
			*l_json_entry->m_entity_name = l_mediate_component->get_entity_name();
			*l_json_entry->m_position = l_world_transform.get_translation();
			*l_json_entry->m_rotation = bc_matrix4f_decompose_to_euler_angles(l_world_transform);

			l_actor_components.clear();
		}

		const auto l_json = l_json_document.write_pretty();
		p_context.m_file.write(reinterpret_cast<const bcBYTE*>(l_json.c_str()), sizeof(decltype(l_json)::value_type)* l_json.size());
	}
}