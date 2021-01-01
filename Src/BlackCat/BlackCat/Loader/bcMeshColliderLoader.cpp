// [02/08/2017 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/bcUtility.h"
#include "PhysicsImp/Fundation/bcMemoryBuffer.h"
#include "PhysicsImp/Shape/bcShape.h"
#include "Game/System/Render/State/bcVertexLayout.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Physics/bcPhysicsShapeUtility.h"
#include "BlackCat/Loader/bcMeshLoader.h"
#include "BlackCat/Loader/bcMeshColliderLoader.h"

#include "3rdParty/Assimp/Include/Importer.hpp"
#include "3rdParty/Assimp/Include/postprocess.h"
#include "3rdParty/Assimp/Include/scene.h"

namespace black_cat
{
	physics::bc_triangle_mesh_desc _bc_extract_triangle_mesh(const aiMesh* l_mesh, core::bc_vector_frame< bcBYTE >& p_intermediate_buffer);

	bc_mesh_collider_loader::bc_mesh_collider_loader(bool p_high_detail_query_shape)
		: m_high_detail_query_shape(p_high_detail_query_shape)
	{
	}

	bool bc_mesh_collider_loader::is_px_node(const aiNode& p_node)
	{
		const bcCHAR* l_px_str = "px_";
		return std::strncmp(l_px_str, p_node.mName.data, std::strlen(l_px_str)) == 0;
	}

	bool bc_mesh_collider_loader::support_offline_processing() const
	{
		return false;
	}

	void bc_mesh_collider_loader::content_processing(core::bc_content_loading_context& p_context) const
	{
		Assimp::Importer l_importer;

		const aiScene* const * l_scene_value = p_context.m_instance_parameters.get_value<const aiScene*>("aiScene");
		const aiScene* l_scene = l_scene_value ? *l_scene_value : nullptr;
		if (!l_scene)
		{
			l_scene = l_importer.ReadFileFromMemory
			(
				p_context.m_file_buffer.get(),
				p_context.m_file_buffer_size,
				aiProcess_GenSmoothNormals |
				aiProcess_CalcTangentSpace |
				aiProcess_Triangulate |
				aiProcess_JoinIdenticalVertices |
				aiProcess_SortByPType |
				(graphic::bc_render_api_info::is_left_handed() ? aiProcess_ConvertToLeftHanded : 0)
			);
		}

		if (!l_scene || !l_scene->HasMeshes())
		{
			const auto l_error_msg =
				core::bc_string_frame("Content file loading error: ")
				+
				core::bc_to_string_frame(p_context.m_file_path).c_str()
				+
				", "
				+
				l_importer.GetErrorString();
			p_context.set_result(bc_io_exception(l_error_msg.c_str()));

			return;
		}

		const bool l_generate_high_detail_query_shape = m_high_detail_query_shape && bc_null_default(p_context.m_parameters->get_value<bool>("high_detail_query_shape"), false);
		game::bc_game_system& l_game_system = *core::bc_get_service< game::bc_game_system >();
		game::bc_mesh_collider l_result;

		convert_nodes
		(
			l_game_system.get_physics_system().get_physics(), 
			p_context, 
			*l_scene, 
			*l_scene->mRootNode, 
			core::bc_matrix4f::identity(), 
			l_generate_high_detail_query_shape, 
			l_result
		);

		p_context.set_result(std::move(l_result));
	}

	aiNode* bc_mesh_collider_loader::find_px_node(const aiNode& p_ainode, const aiMesh& p_ainode_mesh) const
	{
		aiNode* l_result = nullptr;
		core::bc_string_frame l_px_node_name;
		l_px_node_name.reserve(3 + p_ainode_mesh.mName.length);

		l_px_node_name.append("px_");
		l_px_node_name.append(p_ainode_mesh.mName.data);

		for (bcUINT32 l_index = 0; l_index < p_ainode.mNumChildren; ++l_index)
		{
			if (l_px_node_name == p_ainode.mChildren[l_index]->mName.data)
			{
				l_result = p_ainode.mChildren[l_index];
				break;
			}
		}

		return l_result;
	}

	game::bc_mesh_part_collider bc_mesh_collider_loader::convert_px_node(physics::bc_physics& p_physics,
		const aiScene& p_aiscene,
		const aiNode& p_ainode,
		const core::bc_matrix4f& p_parent_transformation,
		bool p_generate_high_detail_query_shape) const
	{
		game::bc_mesh_part_collider l_result;
		core::bc_string_frame l_mesh_name;
		core::bc_matrix4f l_node_transformation;

		bc_mesh_loader::convert_ai_matrix(p_ainode.mTransformation, l_node_transformation);
		const core::bc_matrix4f l_node_absolute_transformation = l_node_transformation * p_parent_transformation;
		const physics::bc_shape_flag l_shape_flag = p_generate_high_detail_query_shape ?
			core::bc_enum::or({ physics::bc_shape_flag::simulation, physics::bc_shape_flag::visualization }) :
			physics::bc_shape_flag::default;

		for (bcUINT32 l_mesh_index = 0; l_mesh_index < p_ainode.mNumMeshes; ++l_mesh_index)
		{
			const aiMesh* l_aimesh = p_aiscene.mMeshes[p_ainode.mMeshes[l_mesh_index]];
			l_mesh_name.assign(l_aimesh->mName.data);

			if (l_mesh_name == "sphere")
			{
				physics::bc_shape_sphere l_px_sphere = game::bc_extract_sphere_from_points(physics::bc_bounded_strided_typed_data< core::bc_vector3f >
					(
						reinterpret_cast<const core::bc_vector3f*>(l_aimesh->mVertices),
						sizeof(aiVector3D),
						l_aimesh->mNumVertices
					));

				l_result.add_px_shape(l_px_sphere, physics::bc_transform(l_node_absolute_transformation), l_shape_flag);
			}
			else if (l_mesh_name == "box")
			{
				physics::bc_shape_box l_px_box = game::bc_extract_box_from_points(physics::bc_bounded_strided_typed_data< core::bc_vector3f >
					(
						reinterpret_cast<const core::bc_vector3f*>(l_aimesh->mVertices),
						sizeof(aiVector3D),
						l_aimesh->mNumVertices
					));

				l_result.add_px_shape(l_px_box, physics::bc_transform(l_node_absolute_transformation), l_shape_flag);
			}
			else if (l_mesh_name == "capsule")
			{
				physics::bc_shape_capsule l_px_capsule = game::bc_extract_capsule_from_points(physics::bc_bounded_strided_typed_data< core::bc_vector3f >
					(
						reinterpret_cast<const core::bc_vector3f*>(l_aimesh->mVertices),
						sizeof(aiVector3D),
						l_aimesh->mNumVertices
					));

				l_result.add_px_shape(l_px_capsule, physics::bc_transform(l_node_absolute_transformation), l_shape_flag);
			}
			else if (l_mesh_name == "convex")
			{
				physics::bc_convex_mesh_desc l_px_convex_desc = game::bc_extract_convex_from_points(physics::bc_bounded_strided_typed_data< core::bc_vector3f >
					(
						reinterpret_cast<const core::bc_vector3f*>(l_aimesh->mVertices),
						sizeof(aiVector3D),
						l_aimesh->mNumVertices
					));

				physics::bc_memory_buffer l_convex_buffer = p_physics.create_convex_mesh(l_px_convex_desc);
				physics::bc_convex_mesh_ref l_convex = p_physics.create_convex_mesh(l_convex_buffer);

				l_result.add_px_shape(std::move(l_convex), physics::bc_transform(l_node_absolute_transformation), l_shape_flag);
			}
			else if (l_mesh_name == "mesh")
			{
				core::bc_vector_frame< bcBYTE > l_intermediate_buffer;
				physics::bc_triangle_mesh_desc l_px_triangle_desc = _bc_extract_triangle_mesh(l_aimesh, l_intermediate_buffer);
				physics::bc_memory_buffer l_triangle_buffer = p_physics.create_triangle_mesh(l_px_triangle_desc);
				physics::bc_triangle_mesh_ref l_triangle_mesh = p_physics.create_triangle_mesh(l_triangle_buffer);

				l_result.add_px_shape(std::move(l_triangle_mesh), physics::bc_transform(l_node_absolute_transformation), l_shape_flag);
			}
		}

		return l_result;
	}

	void bc_mesh_collider_loader::convert_nodes(physics::bc_physics& p_physics,
		core::bc_content_loading_context& p_context,
		const aiScene& p_aiscene,
		const aiNode& p_ainode,
		const core::bc_matrix4f& p_parent_transformation,
		bool p_generate_high_detail_query_shape,
		game::bc_mesh_collider& p_result) const
	{
		if (is_px_node(p_ainode))
		{
			return;
		}

		core::bc_matrix4f l_node_transformation;
		core::bc_matrix4f l_node_absolute_transformation;

		bc_mesh_loader::convert_ai_matrix(p_ainode.mTransformation, l_node_transformation);
		l_node_absolute_transformation = l_node_transformation * p_parent_transformation;

		for (bcUINT32 i = 0; i < p_ainode.mNumMeshes; ++i)
		{
			game::bc_mesh_part_collider l_mesh_colliders;

			aiMesh* l_aimesh = p_aiscene.mMeshes[p_ainode.mMeshes[i]];
			aiNode* l_px_node = find_px_node(p_ainode, *l_aimesh);

			if (l_px_node)
			{
				l_mesh_colliders = convert_px_node(p_physics, p_aiscene, *l_px_node, l_node_absolute_transformation, p_generate_high_detail_query_shape);
				l_mesh_colliders.shrink_to_fit();
			}

			if (p_generate_high_detail_query_shape)
			{
				core::bc_vector_frame< bcBYTE > p_intermediate_buffer;
				physics::bc_triangle_mesh_desc l_px_triangle_desc = _bc_extract_triangle_mesh(l_aimesh, p_intermediate_buffer);
				physics::bc_memory_buffer l_triangle_buffer = p_physics.create_triangle_mesh(l_px_triangle_desc);
				physics::bc_triangle_mesh_ref l_triangle_mesh = p_physics.create_triangle_mesh(l_triangle_buffer);

				l_mesh_colliders.add_px_shape(std::move(l_triangle_mesh), physics::bc_transform(l_node_absolute_transformation), physics::bc_shape_flag::query);
			}

			p_result.add_mesh_colliders(l_aimesh->mName.C_Str(), std::move(l_mesh_colliders));
		}

		for (bcUINT l_child_index = 0; l_child_index < p_ainode.mNumChildren; ++l_child_index)
		{
			aiNode* l_child = p_ainode.mChildren[l_child_index];

			convert_nodes(p_physics, p_context, p_aiscene, *l_child, l_node_absolute_transformation, p_generate_high_detail_query_shape, p_result);
		}
	}

	physics::bc_triangle_mesh_desc _bc_extract_triangle_mesh(const aiMesh* p_mesh, core::bc_vector_frame< bcBYTE >& p_intermediate_buffer)
	{
		const bool l_need_32bit_indices = p_mesh->mNumFaces * 3 > std::numeric_limits< bcUINT16 >::max();
		core::bc_vector_frame< bcBYTE >& l_indices = p_intermediate_buffer;
		l_indices.reserve(p_mesh->mNumFaces * 3 * (l_need_32bit_indices ? static_cast<bcINT>(game::bc_index_type::i32bit) : static_cast<bcINT>(game::bc_index_type::i16bit)));

		bcUINT16* l_16bit_indices = reinterpret_cast<bcUINT16*>(l_indices.data());
		bcUINT32* l_32bit_indices = reinterpret_cast<bcUINT32*>(l_indices.data());
		bcUINT32 l_index_count = 0;

		for (bcUINT l_face_index = 0; l_face_index < p_mesh->mNumFaces; ++l_face_index)
		{
			for (bcBYTE l_index = 0; l_index < 3; ++l_index)
			{
				if (l_need_32bit_indices)
				{
					*l_32bit_indices = static_cast<bcUINT32>(p_mesh->mFaces[l_face_index].mIndices[l_index]);
					++l_32bit_indices;
				}
				else
				{
					*l_16bit_indices = static_cast<bcUINT16>(p_mesh->mFaces[l_face_index].mIndices[l_index]);
					++l_16bit_indices;
				}

				++l_index_count;
			}
		}

		physics::bc_triangle_mesh_desc l_px_triangle_desc = game::bc_extract_mesh_from_points
		(
			physics::bc_bounded_strided_typed_data< core::bc_vector3f >
			(
				reinterpret_cast<const core::bc_vector3f*>(p_mesh->mVertices),
				sizeof(aiVector3D),
				p_mesh->mNumVertices
			),
			physics::bc_bounded_strided_data
			(
				l_indices.data(),
				l_need_32bit_indices ? static_cast<bcINT>(game::bc_index_type::i32bit) : static_cast<bcINT>(game::bc_index_type::i16bit),
				l_index_count
			)
		);

		return l_px_triangle_desc;
	}
}