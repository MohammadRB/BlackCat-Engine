// [23/04/2021 MRB]

#include "App/AppPCH.h"

#include "Core/File/bcJsonDocument.h"
#include "Core/Utility/bcJsonParse.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/bcSceneCheckPoint.h"
#include "Game/bcUtility.h"
#include "App/Loader/bcSceneCheckPointLoader.h"

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

	core::bc_vector_frame<game::bc_actor> bc_scene_checkpoint_loader_base::load_json_checkpoint(game::bc_scene& p_scene, core::bc_string_view p_file_content) const
	{
		core::bc_json_document<_bc_scene_check_point_json> l_json_document;
		l_json_document.load(p_file_content.data());

		core::bc_vector_frame<game::bci_actor_component*> l_actor_components;
		core::bc_vector_frame<game::bc_actor> l_actors;
		l_actors.reserve(l_json_document->m_actors.size());

		for (auto& l_json_actor : l_json_document->m_actors)
		{
			auto l_transform = core::bc_matrix4f::identity();
			l_transform.set_translation(*l_json_actor->m_position);
			l_transform.set_rotation(bc_matrix3f_rotation_zyx(*l_json_actor->m_rotation));

			game::bc_actor l_actor = p_scene.create_actor(l_json_actor->m_entity_name->c_str(), l_transform);
			l_actor.get_components(std::back_inserter(l_actor_components));

			for (auto* l_actor_component : l_actor_components)
			{
				l_actor_component->load_instance(game::bc_actor_component_load_context(*l_json_actor->m_parameters, l_actor));
			}

			l_actors.push_back(l_actor);
			l_actor_components.clear();
		}

		p_scene.get_decal_manager().clear_temp_decals();

		return l_actors;
	}

	core::bc_string_frame bc_scene_checkpoint_loader_base::save_json_checkpoint(const core::bc_vector_frame<game::bc_actor>& p_actors) const
	{
		core::bc_json_document<_bc_scene_check_point_json> l_json_document;
		core::bc_vector_frame<game::bci_actor_component*> l_actor_components;

		for (game::bc_actor& l_actor : p_actors)
		{
			const auto* l_mediate_component = l_actor.get_component<game::bc_mediate_component>();
			auto& l_json_entry = l_json_document->m_actors.new_entry();

			l_actor.get_components(std::back_inserter(l_actor_components));
			for (const auto& l_component : l_actor_components)
			{
				l_component->write_instance(game::bc_actor_component_write_context(*l_json_entry->m_parameters, l_actor));
			}

			const auto& l_world_transform = l_mediate_component->get_world_transform();

			*l_json_entry->m_entity_name = l_mediate_component->get_entity_name();
			*l_json_entry->m_position = l_world_transform.get_translation();
			*l_json_entry->m_rotation = bc_matrix4f_decompose_to_angles_zyx(l_world_transform);

			l_actor_components.clear();
		}

		auto l_json = l_json_document.write_pretty_frame();
		return l_json;
	}
}
