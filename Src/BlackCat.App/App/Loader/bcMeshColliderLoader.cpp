// [02/08/2017 MRB]

#include "App/AppPCH.h"

#include "Core/Container/bcVector.h"
#include "Core/Container/bcStringStream.h"
#include "Core/File/bcPath.h"
#include "Core/bcUtility.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "PhysicsImp/Fundation/bcMemoryBuffer.h"
#include "PhysicsImp/Shape/bcShape.h"
#include "Game/System/Render/State/bcVertexLayout.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Physics/bcPhysicsShapeUtility.h"
#include "Game/bcJsonParse.h"
#include "Game/bcConstant.h"
#include "App/Loader/bcMeshColliderLoader.h"
#include "App/Loader/bcMeshLoaderUtility.h"
#include "3rdParty/Assimp/Include/assimp/Importer.hpp"
#include "3rdParty/Assimp/Include/assimp/postprocess.h"
#include "3rdParty/Assimp/Include/assimp/scene.h"

namespace black_cat
{
	std::tuple<core::bc_unique_ptr<bcBYTE>, physics::bc_triangle_mesh_desc> _bc_extract_triangle_mesh(const aiMesh& p_ai_mesh);

	void _bc_extract_skinned_mesh_collider(const core::bc_unordered_map_frame<core::bc_string_view, bcUINT32>& p_node_mapping,
		const aiMesh& p_ai_mesh,
		game::bc_mesh_collider_builder& p_builder);

	bool bc_mesh_collider_loader::support_offline_processing() const
	{
		return false;
	}

	void bc_mesh_collider_loader::content_processing(core::bc_content_loading_context& p_context) const
	{
		Assimp::Importer l_importer;

		const aiScene* const* l_scene_value = nullptr;
		json_parse::bc_load(p_context.m_instance_parameters, "aiScene", l_scene_value);

		const auto* l_scene = l_scene_value ? *l_scene_value : nullptr;
		if (!l_scene)
		{
			l_scene = l_importer.ReadFileFromMemory
			(
				p_context.m_file_buffer.get(),
				p_context.m_file_buffer_size,
				aiProcess_Triangulate |
				aiProcess_JoinIdenticalVertices |
				aiProcess_SortByPType |
				(graphic::bc_render_api_info::use_left_handed() ? aiProcess_ConvertToLeftHanded : 0)
			);
		}
		
		if (!l_scene || !l_scene->HasMeshes())
		{
			const auto l_error_msg = core::bc_string_stream_frame() << "Error in loading mesh collider '" << p_context.m_file_path.data() << "'. " << l_importer.GetErrorString();
			throw bc_io_exception(l_error_msg.str().c_str());
		}

		/*if(core::bc_path(p_context.m_file_path).get_filename() == bcL("Palm_Tree_Large_d_collider.fbx"))
		{
			BC_DEBUG_BREAK()
		}*/
		
		core::bc_unordered_map_frame<core::bc_string_view, bcUINT32> l_node_mapping;
		core::bc_unordered_map_frame<core::bc_string_view, core::bc_vector_frame<const aiNode*>> l_px_node_mapping;
		core::bc_vector<std::tuple<core::bc_string_view, core::bc_string_view, physics::bc_transform>> l_px_joint_mapping;
		
		bool l_is_skinned = false;
		json_parse::bc_load(p_context.m_parameters, constant::g_param_mesh_skinned, l_is_skinned);
		
		if(l_is_skinned)
		{
			bc_mesh_loader_utility::calculate_node_mapping(*l_scene->mRootNode, l_node_mapping);
			bc_mesh_loader_utility::calculate_skinned_px_node_mapping(*l_scene, *l_scene->mRootNode, l_px_node_mapping);
			bc_mesh_loader_utility::calculate_px_joint_mapping(*l_scene, l_px_joint_mapping);
		}
		else
		{
			bc_mesh_loader_utility::calculate_node_mapping(*l_scene->mRootNode, l_node_mapping);
			bc_mesh_loader_utility::calculate_px_node_mapping(*l_scene, *l_scene->mRootNode, l_px_node_mapping);
		}
		
		bool l_generate_high_detail_query_shape = true;
		json_parse::bc_load(p_context.m_parameters, constant::g_param_high_detail_query_shape, l_generate_high_detail_query_shape);

		auto& l_game_system = *core::bc_get_service<game::bc_game_system>();
		game::bc_mesh_collider_builder l_builder;

		convert_nodes
		(
			p_context,
			l_game_system.get_physics_system().get_physics(),
			l_node_mapping,
			l_px_node_mapping, 
			*l_scene, 
			*l_scene->mRootNode,
			l_generate_high_detail_query_shape,
			l_is_skinned,
			l_builder
		);

		for(auto& l_joint : l_px_joint_mapping)
		{
			l_builder.add_px_joint(std::get<0>(l_joint), std::get<1>(l_joint), std::get<2>(l_joint));
		}
		
		p_context.set_result(l_builder.build());
	}

	aiNode* bc_mesh_collider_loader::find_px_node(const aiNode& p_ai_node, const aiMesh& p_ai_node_mesh) const
	{
		aiNode* l_result = nullptr;
		core::bc_string_frame l_px_node_name;
		l_px_node_name.reserve(3 + p_ai_node_mesh.mName.length);

		l_px_node_name.append(bc_mesh_loader_utility::s_px_node_prefix);
		l_px_node_name.append(p_ai_node_mesh.mName.data);

		for (bcUINT32 l_index = 0; l_index < p_ai_node.mNumChildren; ++l_index)
		{
			auto* l_child = p_ai_node.mChildren[l_index];
			if (l_px_node_name.compare(0, l_px_node_name.size(), l_child->mName.data) == 0)
			{
				l_result = l_child;
				break;
			}
		}

		return l_result;
	}

	void bc_mesh_collider_loader::convert_px_node(physics::bc_physics& p_physics,
		const core::bc_unordered_map_frame<core::bc_string_view, bcUINT32>& p_node_mapping,
		const aiScene& p_ai_scene,
		const aiNode& p_attached_node,
		const aiNode& p_px_node,
		game::bc_mesh_collider_builder& p_builder) const
	{
		core::bc_matrix4f l_node_transformation;
		const auto l_attached_node_index = p_node_mapping.find(p_attached_node.mName.C_Str())->second;

		bc_mesh_loader_utility::convert_ai_matrix(p_px_node.mTransformation, l_node_transformation);
		constexpr auto l_shape_flag = physics::bc_shape_flag::default_v;

		core::bc_string_frame l_mesh_name;
		for (bcUINT32 l_mesh_index = 0; l_mesh_index < p_px_node.mNumMeshes; ++l_mesh_index)
		{
			const aiMesh* l_ai_mesh = p_ai_scene.mMeshes[p_px_node.mMeshes[l_mesh_index]];
			l_mesh_name.assign(l_ai_mesh->mName.data);

			if (l_mesh_name.compare(0, sizeof("px_sphere") - 1, "px_sphere") == 0)
			{
				physics::bc_shape_sphere l_px_sphere = game::bc_extract_sphere_from_points(physics::bc_bounded_strided_typed_data<core::bc_vector3f>
				(
					reinterpret_cast<const core::bc_vector3f*>(l_ai_mesh->mVertices),
					sizeof(aiVector3D),
					l_ai_mesh->mNumVertices
				));

				p_builder.add_px_shape
				(
					p_px_node.mName.C_Str(),
					p_attached_node.mName.C_Str(),
					l_attached_node_index,
					l_px_sphere,
					physics::bc_transform(l_node_transformation),
					l_shape_flag
				);
			}
			else if (l_mesh_name.compare(0, sizeof("px_box") - 1, "px_box") == 0)
			{
				physics::bc_shape_box l_px_box = game::bc_extract_box_from_points(physics::bc_bounded_strided_typed_data<core::bc_vector3f>
				(
					reinterpret_cast<const core::bc_vector3f*>(l_ai_mesh->mVertices),
					sizeof(aiVector3D),
					l_ai_mesh->mNumVertices
				));

				p_builder.add_px_shape
				(
					p_px_node.mName.C_Str(),
					p_attached_node.mName.C_Str(),
					l_attached_node_index,
					l_px_box,
					physics::bc_transform(l_node_transformation),
					l_shape_flag
				);
			}
			else if (l_mesh_name.compare(0, sizeof("px_capsule") - 1, "px_capsule") == 0)
			{
				physics::bc_shape_capsule l_px_capsule = game::bc_extract_capsule_from_points(physics::bc_bounded_strided_typed_data<core::bc_vector3f>
				(
					reinterpret_cast<const core::bc_vector3f*>(l_ai_mesh->mVertices),
					sizeof(aiVector3D),
					l_ai_mesh->mNumVertices
				));

				p_builder.add_px_shape
				(
					p_px_node.mName.C_Str(),
					p_attached_node.mName.C_Str(),
					l_attached_node_index,
					l_px_capsule,
					physics::bc_transform(l_node_transformation),
					l_shape_flag
				);
			}
			else if (l_mesh_name.compare(0, sizeof("px_convex") - 1, "px_convex") == 0)
			{
				physics::bc_convex_mesh_desc l_px_convex_desc = game::bc_extract_convex_from_points(physics::bc_bounded_strided_typed_data<core::bc_vector3f>
				(
					reinterpret_cast<const core::bc_vector3f*>(l_ai_mesh->mVertices),
					sizeof(aiVector3D),
					l_ai_mesh->mNumVertices
				));

				physics::bc_memory_buffer l_convex_buffer = p_physics.create_convex_mesh(l_px_convex_desc);
				physics::bc_convex_mesh_ref l_convex = p_physics.create_convex_mesh(l_convex_buffer);

				p_builder.add_px_shape
				(
					p_px_node.mName.C_Str(),
					p_attached_node.mName.C_Str(),
					l_attached_node_index,
					std::move(l_convex),
					physics::bc_transform(l_node_transformation),
					l_shape_flag
				);
			}
			else if (l_mesh_name.compare(0, sizeof("px_mesh") - 1, "px_mesh") == 0)
			{
				auto l_triangle_mesh_data = _bc_extract_triangle_mesh(*l_ai_mesh);
				physics::bc_memory_buffer l_triangle_buffer = p_physics.create_triangle_mesh(std::get<physics::bc_triangle_mesh_desc>(l_triangle_mesh_data));
				physics::bc_triangle_mesh_ref l_triangle_mesh = p_physics.create_triangle_mesh(l_triangle_buffer);

				p_builder.add_px_shape
				(
					p_px_node.mName.C_Str(),
					p_attached_node.mName.C_Str(),
					l_attached_node_index,
					std::move(l_triangle_mesh),
					physics::bc_transform(l_node_transformation),
					l_shape_flag
				);
			}
		}
	}

	void bc_mesh_collider_loader::convert_nodes(core::bc_content_loading_context& p_context,
		physics::bc_physics& p_physics,
		const core::bc_unordered_map_frame<core::bc_string_view, bcUINT32>& p_node_mapping,
		const core::bc_unordered_map_frame<core::bc_string_view, core::bc_vector_frame<const aiNode*>>& p_px_node_mapping,
		const aiScene& p_ai_scene,
		const aiNode& p_ai_node,
		bool p_high_detail_query_shape,
		bool p_skinned,
		game::bc_mesh_collider_builder& p_builder) const
	{
		if (bc_mesh_loader_utility::is_px_node(p_ai_node) || bc_mesh_loader_utility::is_px_joint_node(p_ai_node))
		{
			return;
		}
		
		if(p_skinned)
		{
			const auto* l_ai_node_name = p_ai_node.mName.C_Str();
			const auto l_px_nodes = p_px_node_mapping.find(l_ai_node_name);

			if (l_px_nodes != std::end(p_px_node_mapping))
			{
				for (const aiNode* l_px_node : l_px_nodes->second)
				{
					convert_px_node
					(
						p_physics,
						p_node_mapping,
						p_ai_scene,
						p_ai_node,
						*l_px_node,
						p_builder
					);
				}
			}

			if (p_high_detail_query_shape)
			{
				for (bcUINT32 l_i = 0; l_i < p_ai_node.mNumMeshes; ++l_i)
				{
					const auto* l_ai_mesh = p_ai_scene.mMeshes[p_ai_node.mMeshes[l_i]];
					_bc_extract_skinned_mesh_collider(p_node_mapping, *l_ai_mesh, p_builder);
				}
			}
		}
		else
		{
			const auto* l_ai_node_name = p_ai_node.mName.C_Str();
			const auto l_px_nodes = p_px_node_mapping.find(l_ai_node_name);

			if (l_px_nodes != std::end(p_px_node_mapping))
			{
				for (const aiNode* l_px_node : l_px_nodes->second)
				{
					convert_px_node
					(
						p_physics,
						p_node_mapping,
						p_ai_scene,
						p_ai_node,
						*l_px_node,
						p_builder
					);
				}
			}

			if (p_high_detail_query_shape)
			{
				for (bcUINT32 l_i = 0; l_i < p_ai_node.mNumMeshes; ++l_i)
				{
					const auto* l_ai_mesh = p_ai_scene.mMeshes[p_ai_node.mMeshes[l_i]];
					const auto l_node_index = p_node_mapping.find(p_ai_node.mName.C_Str())->second;
					auto l_triangle_mesh_data = _bc_extract_triangle_mesh(*l_ai_mesh);
					auto l_triangle_buffer = p_physics.create_triangle_mesh(std::get<physics::bc_triangle_mesh_desc>(l_triangle_mesh_data));
					auto l_triangle_mesh = p_physics.create_triangle_mesh(l_triangle_buffer);

					// Avoid double transform by parent node by passing identity transform
					p_builder.add_px_shape
					(
						l_ai_mesh->mName.C_Str(),
						l_ai_node_name,
						l_node_index,
						std::move(l_triangle_mesh),
						physics::bc_transform::identity(),
						physics::bc_shape_flag::query,
						true
					);
				}
			}
		}

		for (bcUINT l_child_index = 0; l_child_index <p_ai_node.mNumChildren; ++l_child_index)
		{
			aiNode* l_child = p_ai_node.mChildren[l_child_index];

			convert_nodes
			(
				p_context,
				p_physics,
				p_node_mapping,
				p_px_node_mapping,
				p_ai_scene,
				*l_child,
				p_high_detail_query_shape,
				p_skinned,
				p_builder
			);
		}
	}

	std::tuple<core::bc_unique_ptr<bcBYTE>, physics::bc_triangle_mesh_desc> _bc_extract_triangle_mesh(const aiMesh& p_ai_mesh)
	{
		const bool l_need_32bit_indices = p_ai_mesh.mNumFaces * 3> std::numeric_limits<bcUINT16>::max();
		const auto l_bytes_needed_for_indices = p_ai_mesh.mNumFaces * 3 * (l_need_32bit_indices ? sizeof(bcINT32) : sizeof(bcINT16));
		core::bc_unique_ptr<bcBYTE> l_indices_buffer(static_cast<bcBYTE*>(BC_ALLOC(l_bytes_needed_for_indices, core::bc_alloc_type::frame)));

		auto* l_16bit_indices = reinterpret_cast<bcUINT16*>(l_indices_buffer.get());
		auto* l_32bit_indices = reinterpret_cast<bcUINT32*>(l_indices_buffer.get());
		bcUINT32 l_index_count = 0;

		for (bcUINT l_face_index = 0; l_face_index <p_ai_mesh.mNumFaces; ++l_face_index)
		{
			for (bcBYTE l_index = 0; l_index <3; ++l_index)
			{
				if (l_need_32bit_indices)
				{
					*l_32bit_indices = static_cast<bcUINT32>(p_ai_mesh.mFaces[l_face_index].mIndices[l_index]);
					++l_32bit_indices;
				}
				else
				{
					*l_16bit_indices = static_cast<bcUINT16>(p_ai_mesh.mFaces[l_face_index].mIndices[l_index]);
					++l_16bit_indices;
				}

				++l_index_count;
			}
		}

		physics::bc_triangle_mesh_desc l_px_triangle_desc = game::bc_extract_mesh_from_points
		(
			physics::bc_bounded_strided_typed_data<core::bc_vector3f>
			(
				reinterpret_cast<const core::bc_vector3f*>(p_ai_mesh.mVertices),
				sizeof(aiVector3D),
				p_ai_mesh.mNumVertices
			),
			physics::bc_bounded_strided_data
			(
				l_indices_buffer.get(),
				l_need_32bit_indices ? sizeof(bcUINT32) : sizeof(bcUINT16),
				l_index_count
			)
		);

		return std::make_tuple(std::move(l_indices_buffer), std::move(l_px_triangle_desc));
	}

	void _bc_extract_skinned_mesh_collider(const core::bc_unordered_map_frame<core::bc_string_view, bcUINT32>& p_node_mapping,
		const aiMesh& p_ai_mesh,
		game::bc_mesh_collider_builder& p_builder)
	{
		const bool l_need_32bit_indices = p_ai_mesh.mNumFaces * 3> std::numeric_limits<bcUINT16>::max();

		core::bc_vector_frame<game::bc_skinned_mesh_collider_vertex> l_vertices(p_ai_mesh.mNumVertices);
		core::bc_vector_frame<bcUINT16> l_16bit_indices;
		core::bc_vector_frame<bcUINT32> l_32bit_indices;
		
		if (l_need_32bit_indices)
		{
			l_32bit_indices.resize(p_ai_mesh.mNumFaces * 3);
		}
		else
		{
			l_16bit_indices.resize(p_ai_mesh.mNumFaces * 3);
		}
		
		for (bcUINT l_vertex = 0; l_vertex <p_ai_mesh.mNumVertices; ++l_vertex)
		{
			auto& l_ai_vertex = p_ai_mesh.mVertices[l_vertex];
			l_vertices[l_vertex].m_position = core::bc_vector3f(l_ai_vertex.x, l_ai_vertex.y, l_ai_vertex.z);
		}

		for (auto l_bone_ite = 0U; l_bone_ite <p_ai_mesh.mNumBones; ++l_bone_ite)
		{
			const auto* l_ai_bone = p_ai_mesh.mBones[l_bone_ite];

			for (auto l_bone_weight_ite = 0U; l_bone_weight_ite <l_ai_bone->mNumWeights; ++l_bone_weight_ite)
			{
				const auto& l_ai_bone_weight = l_ai_bone->mWeights[l_bone_weight_ite];
				const auto l_bone_index = p_node_mapping.find(l_ai_bone->mName.C_Str())->second;
				const auto l_bone_weight = l_ai_bone_weight.mWeight;

				auto& l_vertex_ids = l_vertices[l_ai_bone_weight.mVertexId].m_indices;
				auto& l_vertex_weights = l_vertices[l_ai_bone_weight.mVertexId].m_weights;

				bc_mesh_loader_utility::store_skinned_vertex_weights(l_vertex_ids, l_vertex_weights, l_bone_index, l_bone_weight);
				bc_mesh_loader_utility::sort_skinned_vertex_weights(l_vertex_ids, l_vertex_weights);
			}
		}

		for(auto& l_vertex : l_vertices)
		{
			bc_mesh_loader_utility::clean_skinned_vertex_weights(l_vertex.m_indices, l_vertex.m_weights, 0.05f);
		}

		auto* l_16bit_index = l_16bit_indices.data();
		auto* l_32bit_index = l_32bit_indices.data();

		for (bcUINT l_face_index = 0; l_face_index <p_ai_mesh.mNumFaces; ++l_face_index)
		{
			for (bcBYTE l_index = 0; l_index <3; ++l_index)
			{
				if (l_need_32bit_indices)
				{
					*l_32bit_index = static_cast<bcUINT32>(p_ai_mesh.mFaces[l_face_index].mIndices[l_index]);
					++l_32bit_index;
				}
				else
				{
					*l_16bit_index = static_cast<bcUINT16>(p_ai_mesh.mFaces[l_face_index].mIndices[l_index]);
					++l_16bit_index;
				}
			}
		}

		if (l_need_32bit_indices)
		{
			p_builder.add_skinned_mesh_collider(l_vertices.data(), l_32bit_indices.data(), l_vertices.size(), p_ai_mesh.mNumFaces);
		}
		else
		{
			p_builder.add_skinned_mesh_collider(l_vertices.data(), l_16bit_indices.data(), l_vertices.size(), p_ai_mesh.mNumFaces);
		}
	}
}
