// [01/26/2019 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Core/Concurrency/bcConcurrency.h"
#include "Core/File/bcJsonDocument.h"
#include "Core/Utility/bcJsonParse.h"
#include "PhysicsImp/Fundation/bcSceneBuilder.h"
#include "Game/Object/Scene/bcEntityManager.h"
#include "Game/Object/Scene/SceneGraph/bcScenceGraph.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/bcNameComponent.h"
#include "Game/Object/Scene/Component/bcRigidBodyComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/System/Physics/bcPhysicsSystem.h"
#include "Game/System/bcGameSystem.h"
#include "BlackCat/Loader/bcSceneLoader.h"

namespace black_cat
{
	BC_JSON_STRUCTURE(_bc_scene_actor)
	{
		BC_JSON_VALUE(core::bc_string_frame, entity_name);
		BC_JSON_VALUE(core::bc_vector3f, position);
		BC_JSON_VALUE_OP(core::bc_json_key_value, parameters);
	};

	BC_JSON_STRUCTURE(_bc_scene_graph_json)
	{
		BC_JSON_VALUE(core::bc_vector3f, center);
		BC_JSON_VALUE(core::bc_vector3f, half_extends);
	};

	BC_JSON_STRUCTURE(_bc_scene_json)
	{
		BC_JSON_VALUE(core::bc_string_frame, name);
		BC_JSON_ARRAY(core::bc_string_frame, streams);
		BC_JSON_OBJECT(_bc_scene_graph_json, scene_graph);
		BC_JSON_ARRAY(_bc_scene_actor, actors);
	};

	bc_scene_loader::bc_scene_loader(game::bc_iscene_graph_node_factory p_scene_graph_factory)
	{
		m_scene_graph_factory = p_scene_graph_factory;
	}

	void bc_scene_loader::content_offline_processing(core::bc_content_loading_context& p_context) const
	{
	}

	void bc_scene_loader::content_processing(core::bc_content_loading_context& p_context) const
	{
		core::bc_json_document< _bc_scene_json > l_json;
		core::bc_string_frame l_json_str(p_context.m_buffer.size() + 1, '\0');
		std::memcpy(const_cast< bcCHAR* >(l_json_str.data()), p_context.m_buffer.data(), p_context.m_buffer.size());

		l_json.load(l_json_str.data());

		auto* l_game_system = core::bc_get_service< game::bc_game_system >();
		auto* l_content_stream_manager = core::bc_get_service< core::bc_content_stream_manager >();
		auto* l_entity_manager = core::bc_get_service< game::bc_entity_manager >();

		core::bc_vector_frame< core::bc_task< void > > l_stream_tasks;
		l_stream_tasks.reserve(l_json->m_streams.size());

		std::transform
		(
			std::begin(l_json->m_streams),
			std::end(l_json->m_streams),
			std::back_inserter(l_stream_tasks),
			[=](const core::bc_json_value< core::bc_string_frame >& p_stream)
			{
				return l_content_stream_manager->load_content_stream_async(core::bc_alloc_type::unknown, p_stream->c_str());
			}
		);
		core::bc_concurrency::when_all(std::cbegin(l_stream_tasks), std::cend(l_stream_tasks));

		auto l_px_scene_builder = std::move(physics::bc_scene_builder().enable_ccd());
		auto l_px_scene = l_game_system->get_physics_system().get_physics().create_scene(std::move(l_px_scene_builder));
		auto l_scene_graph = game::bc_scene_graph(m_scene_graph_factory(game::bc_scene_graph_node_factory_parameter
		{
			*l_json->m_scene_graph->m_center,
			*l_json->m_scene_graph->m_half_extends
		}));

		core::bc_vector< core::bc_string > l_stream_names;
		std::transform
		(
			std::cbegin(l_json->m_streams),
			std::cend(l_json->m_streams),
			std::back_inserter(l_stream_names),
			[](const core::bc_json_value< core::bc_string_frame >& p_value)
			{
				return p_value->c_str();
			}
		);
		
		auto l_scene = game::bc_scene
		(
			core::bc_estring(p_context.m_file_path.c_str()),
			core::bc_string(l_json->m_name->c_str()), 
			l_stream_names,
			std::move(l_scene_graph), 
			std::move(l_px_scene)
		);

		core::bc_vector< game::bc_iactor_component* > l_actor_components;
		for (auto& l_json_actor : l_json->m_actors)
		{
			game::bc_actor l_actor = l_entity_manager->create_entity(l_json_actor->m_entity_name->c_str());
			game::bc_mediate_component* l_mediate_component = l_actor.get_component< game::bc_mediate_component >();
			l_mediate_component->set_world_position(*l_json_actor->m_position);

			l_actor.get_components(std::back_inserter(l_actor_components));

			for (auto l_actor_component : l_actor_components)
			{
				l_actor_component->load_instance(l_actor, *l_json_actor->m_parameters);
			}

			l_actor_components.clear();
			l_scene.add_actor(l_actor);
		}

		p_context.set_result(std::move(l_scene));
	}

	void bc_scene_loader::content_processing(core::bc_content_saving_context& p_context) const
	{
		auto* l_scene = static_cast<game::bc_scene*>(p_context.m_content);

		core::bc_json_document< _bc_scene_json > l_json_document;

		*l_json_document->m_name = l_scene->get_name().c_str();
		*l_json_document->m_scene_graph->m_center = l_scene->get_scene_graph().get_bound_box().get_center();
		*l_json_document->m_scene_graph->m_half_extends = l_scene->get_scene_graph().get_bound_box().get_half_extends();
		
		for (auto& l_stream : l_scene->get_loaded_streams())
		{
			auto& l_stream_entry = l_json_document->m_streams.new_entry();
			*l_stream_entry = l_stream.c_str();
		}

		core::bc_vector_frame< game::bc_iactor_component* > l_actor_components;
		for (auto& l_actor : l_scene->get_scene_graph())
		{
			auto& l_actor_entry = l_json_document->m_actors.new_entry();

			l_actor.get_components(std::back_inserter(l_actor_components));
			for (auto& l_component : l_actor_components)
			{
				l_component->write_instance(l_actor, *l_actor_entry->m_parameters);
			}

			*l_actor_entry->m_entity_name = *l_actor_entry->m_parameters->find(game::bc_name_component::s_entity_name_json_key)->second.as_throw<const bcCHAR*>();
			*l_actor_entry->m_position = *l_actor_entry->m_parameters->find(game::bc_mediate_component::s_position_json_key)->second.as_throw<core::bc_vector3f>();

			l_actor_entry->m_parameters->remove(game::bc_name_component::s_entity_name_json_key);
			l_actor_entry->m_parameters->remove(game::bc_mediate_component::s_position_json_key);

			l_actor_components.clear();
		}

		auto l_json = l_json_document.write();
		p_context.m_file->write(l_json.c_str(), l_json.size());
	}
}