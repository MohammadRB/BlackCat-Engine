// [01/26/2019 MRB]

#include "App/AppPCH.h"

#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Core/Concurrency/bcConcurrency.h"
#include "Core/File/bcJsonDocument.h"
#include "Core/Utility/bcJsonParse.h"
#include "Core/Utility/bcLogger.h"
#include "PhysicsImp/Fundation/bcSceneBuilder.h"
#include "Game/Object/Scene/bcEntityManager.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/SceneGraph/bcScenceGraph.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/System/Input/bcInputSystem.h"
#include "Game/System/Input/bcFileSystem.h"
#include "Game/System/Physics/bcPhysicsSystem.h"
#include "Game/System/Render/Material/bcMaterialManager.h"
#include "Game/System/Render/Decal/bcDecalManager.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/bcGameSystem.h"
#include "Game/bcUtility.h"
#include "App/Loader/bcSceneLoader.h"

namespace black_cat
{
	BC_JSON_STRUCTURE(_bc_scene_actor)
	{
		BC_JSON_VALUE(core::bc_string_frame, entity_name);
		BC_JSON_VALUE(core::bc_vector3f, position);
		BC_JSON_VALUE(core::bc_vector3f, rotation);
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
		BC_JSON_ARRAY(core::bc_string_frame, stream_files);
		BC_JSON_ARRAY(core::bc_string_frame, material_files);
		BC_JSON_ARRAY(core::bc_string_frame, decal_files);
		BC_JSON_ARRAY(core::bc_string_frame, entity_files);
		BC_JSON_ARRAY(core::bc_string_frame, streams);
		BC_JSON_OBJECT(_bc_scene_graph_json, scene_graph);
		BC_JSON_ARRAY(_bc_scene_actor, actors);
	};

	bc_scene_loader::bc_scene_loader(game::bc_px_scene_builder_factory p_px_scene_builder_factory, game::bc_scene_graph_node_factory p_scene_graph_factory)
		: m_px_scene_builder_factory(std::move(p_px_scene_builder_factory)),
		m_scene_graph_factory(std::move(p_scene_graph_factory))
	{
	}

	bool bc_scene_loader::support_offline_processing() const
	{
		return false;
	}

	void bc_scene_loader::content_processing(core::bc_content_loading_context& p_context) const
	{
		core::bc_log(core::bc_log_type::info) << "loading scene " << p_context.m_file_path << core::bc_lend;
		
		core::bc_json_document<_bc_scene_json> l_json_document;
		core::bc_string_frame l_json_str(p_context.m_file_buffer_size + 1, '\0');
		std::memcpy(l_json_str.data(), p_context.m_file_buffer.get(), p_context.m_file_buffer_size);
		
		l_json_document.load(l_json_str.data());

		auto& l_game_system = *core::bc_get_service<game::bc_game_system>();
		auto& l_content_stream_manager = *core::bc_get_service<core::bc_content_stream_manager>();
		auto& l_material_manager = l_game_system.get_render_system().get_material_manager();
		auto& l_decal_manager = l_game_system.get_render_system().get_decal_manager();
		auto& l_entity_manager = *core::bc_get_service<game::bc_entity_manager>();
		auto& l_file_system = l_game_system.get_file_system();

		for (core::bc_json_value<core::bc_string_frame>& l_stream_file : l_json_document->m_stream_files)
		{
			auto l_path = core::bc_to_estring_frame(l_stream_file->c_str());
			l_content_stream_manager.read_stream_file(l_file_system.get_content_path(l_path.c_str()).c_str());
		}
		for (core::bc_json_value<core::bc_string_frame>& l_material_file : l_json_document->m_material_files)
		{
			auto l_path = core::bc_to_estring_frame(l_material_file->c_str());
			l_material_manager.read_material_file(l_file_system.get_content_path(l_path.c_str()).c_str());
		}
		for (core::bc_json_value<core::bc_string_frame>& l_decal_file : l_json_document->m_decal_files)
		{
			auto l_path = core::bc_to_estring_frame(l_decal_file->c_str());
			l_decal_manager.read_decal_file(l_file_system.get_content_path(l_path.c_str()).c_str());
		}
		for (core::bc_json_value<core::bc_string_frame>& l_entity_file : l_json_document->m_entity_files)
		{
			auto l_path = core::bc_to_estring_frame(l_entity_file->c_str());
			l_entity_manager.read_entity_file(l_file_system.get_content_path(l_path.c_str()).c_str());
		}
		
		core::bc_vector_frame<core::bc_task<void>> l_stream_tasks;
		l_stream_tasks.reserve(l_json_document->m_streams.size());

		std::transform
		(
			std::begin(l_json_document->m_streams),
			std::end(l_json_document->m_streams),
			std::back_inserter(l_stream_tasks),
			[&](const core::bc_json_value<core::bc_string_frame>& p_stream)
			{
				return l_content_stream_manager.load_content_stream_async(core::bc_alloc_type::unknown, p_stream->c_str());
			}
		);
		core::bc_concurrency::when_all(std::cbegin(l_stream_tasks), std::cend(l_stream_tasks));

		auto l_px_scene = l_game_system.get_physics_system().get_physics().create_scene
		(
			m_px_scene_builder_factory()
		);
		auto l_scene_graph = game::bc_scene_graph
		(
			m_scene_graph_factory
			(
				game::bc_scene_graph_node_factory_parameter
				{
					*l_json_document->m_scene_graph->m_center,
					*l_json_document->m_scene_graph->m_half_extends
				}
			)
		);

		core::bc_vector<core::bc_string> l_stream_file_names;
		l_stream_file_names.reserve(l_json_document->m_stream_files.size());
		std::transform
		(
			std::cbegin(l_json_document->m_stream_files),
			std::cend(l_json_document->m_stream_files),
			std::back_inserter(l_stream_file_names),
			[](const core::bc_json_value<core::bc_string_frame>& p_value)
			{
				return p_value->c_str();
			}
		);

		core::bc_vector<core::bc_string> l_material_file_names;
		l_material_file_names.reserve(l_json_document->m_material_files.size());
		std::transform
		(
			std::cbegin(l_json_document->m_material_files),
			std::cend(l_json_document->m_material_files),
			std::back_inserter(l_material_file_names),
			[](const core::bc_json_value<core::bc_string_frame>& p_value)
			{
				return p_value->c_str();
			}
		);

		core::bc_vector<core::bc_string> l_decal_file_names;
		l_decal_file_names.reserve(l_json_document->m_decal_files.size());
		std::transform
		(
			std::cbegin(l_json_document->m_decal_files),
			std::cend(l_json_document->m_decal_files),
			std::back_inserter(l_decal_file_names),
			[](const core::bc_json_value<core::bc_string_frame>& p_value)
			{
				return p_value->c_str();
			}
		);
		
		core::bc_vector<core::bc_string> l_entity_file_names;
		l_entity_file_names.reserve(l_json_document->m_entity_files.size());
		std::transform
		(
			std::cbegin(l_json_document->m_entity_files),
			std::cend(l_json_document->m_entity_files),
			std::back_inserter(l_entity_file_names),
			[](const core::bc_json_value<core::bc_string_frame>& p_value)
			{
				return p_value->c_str();
			}
		);
		
		core::bc_vector<core::bc_string> l_stream_names;
		l_stream_names.reserve(l_json_document->m_streams.size());
		std::transform
		(
			std::cbegin(l_json_document->m_streams),
			std::cend(l_json_document->m_streams),
			std::back_inserter(l_stream_names),
			[](const core::bc_json_value<core::bc_string_frame>& p_value)
			{
				return p_value->c_str();
			}
		);
		
		p_context.set_result(game::bc_scene
		(
			l_entity_manager,
			l_game_system,
			core::bc_estring(p_context.m_file_path),
			core::bc_string(*l_json_document->m_name),
			l_stream_file_names,
			l_material_file_names,
			l_decal_file_names,
			l_entity_file_names,
			l_stream_names,
			std::move(l_scene_graph),
			std::move(l_px_scene)
		));

		auto& l_scene = *p_context.m_result->get_result<game::bc_scene>();
		l_scene.enable_bulk_loading(l_json_document->m_actors.size());

		core::bc_vector_frame<game::bci_actor_component*> l_actor_components;

		for (auto& l_json_actor : l_json_document->m_actors)
		{
			auto l_transform = core::bc_matrix4f::identity();
			l_transform.set_translation(*l_json_actor->m_position);
			l_transform.set_rotation(bc_matrix3f_rotation_zyx(*l_json_actor->m_rotation));
			
			game::bc_actor l_actor = l_scene.create_actor(l_json_actor->m_entity_name->c_str(), l_transform);
			if(!l_actor.is_valid())
			{
				continue;
			}

			game::bc_actor_load_instance(l_actor_components, game::bc_actor_component_load_context(*l_json_actor->m_parameters, l_actor));
		}

		l_scene.disable_bulk_loading();

		core::bc_log(core::bc_log_type::info) << "scene loaded" << core::bc_lend;
	}

	void bc_scene_loader::content_processing(core::bc_content_saving_context& p_context) const
	{
		core::bc_log(core::bc_log_type::info) << "saving scene " << p_context.m_file_path << core::bc_lend;

		auto& l_scene = static_cast<game::bc_scene&>(p_context.m_content);

		core::bc_json_document<_bc_scene_json> l_json_document{};
		l_json_document.set_max_decimal_places(2);
		*l_json_document->m_name = l_scene.get_name();
		*l_json_document->m_scene_graph->m_center = l_scene.get_scene_graph().get_bound_box().get_center();
		*l_json_document->m_scene_graph->m_half_extends = l_scene.get_scene_graph().get_bound_box().get_half_extends();

		for (const auto& l_stream_file : l_scene.get_stream_files())
		{
			auto& l_entry = l_json_document->m_stream_files.new_entry();
			*l_entry = l_stream_file;
		}
		
		for (const auto& l_material : l_scene.get_material_files())
		{
			auto& l_entry = l_json_document->m_material_files.new_entry();
			*l_entry = l_material;
		}
		
		for (const auto& l_decal : l_scene.get_decal_files())
		{
			auto& l_entry = l_json_document->m_decal_files.new_entry();
			*l_entry = l_decal;
		}
		
		for (const auto& l_entity_file : l_scene.get_entity_files())
		{
			auto& l_entry = l_json_document->m_entity_files.new_entry();
			*l_entry = l_entity_file;
		}
		
		for (const auto& l_stream : l_scene.get_loaded_streams())
		{
			auto& l_entry = l_json_document->m_streams.new_entry();
			*l_entry = l_stream;
		}

		core::bc_vector_frame<game::bci_actor_component*> l_actor_components;
		for (auto& l_actor : l_scene.get_scene_graph())
		{
			const auto* l_mediate_component = l_actor.get_component<game::bc_mediate_component>();
			auto& l_json_entry = l_json_document->m_actors.new_entry();			
			*l_json_entry->m_entity_name = l_mediate_component->get_entity_name();
			*l_json_entry->m_position = l_mediate_component->get_position();
			*l_json_entry->m_rotation = bc_matrix4f_decompose_to_angles_zyx(l_mediate_component->get_world_transform());

			game::bc_actor_write_instance(l_actor_components, game::bc_actor_component_write_context(*l_json_entry->m_parameters, l_actor));
		}

		const auto l_json = l_json_document.write_pretty_frame();
		p_context.m_file.write(l_json.c_str(), sizeof(decltype(l_json)::value_type) * l_json.size());

		core::bc_log(core::bc_log_type::info) << "scene saved" << core::bc_lend;
	}
}