// [02/08/2017 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "PhysicsImp/Fundation/bcMeshBuffer.h"
#include "Game/System/Render/bcVertexLayout.h"
#include "Game/System/bcGameSystem.h"
#include "BlackCat/bcUtility.h"
#include "BlackCat/Loader/bcMeshLoader.h"
#include "BlackCat/Loader/bcMeshPhysicsLoader.h"

#include "3rdParty/Assimp/include/assimp/postprocess.h"
#include "3rdParty/Assimp/include/assimp/Importer.hpp"

namespace black_cat
{
	bool bc_mesh_physics_loader::is_px_node(const aiNode& p_node)
	{
		core::bc_string_frame l_node_name(p_node.mName.data);
		const bcCHAR* l_px_str = "px_";

		if (l_node_name.compare(0, std::strlen(l_px_str), l_px_str) == 0)
		{
			return true;
		}

		return false;
	}

	aiNode* bc_mesh_physics_loader::find_px_node(const aiNode& p_ainode, const aiMesh& p_ainode_mesh)
	{
		aiNode* l_result = nullptr;
		core::bc_string_frame l_px_node_name;
		l_px_node_name.reserve(3 + p_ainode_mesh.mName.length);

		l_px_node_name.append("px_");
		l_px_node_name.append(p_ainode_mesh.mName.data);

		for (bcUINT32 l_index = 0; l_index < p_ainode.mNumChildren; ++l_index)
		{
			if (l_px_node_name.compare(p_ainode.mChildren[l_index]->mName.data) == 0)
			{
				l_result = p_ainode.mChildren[l_index];
				break;
			}
		}

		return l_result;
	}

	game::bc_mesh_part_collider bc_mesh_physics_loader::convert_px_node(physics::bc_physics& p_physics,
		const aiScene& p_aiscene,
		const aiNode& p_ainode,
		const core::bc_matrix4f& p_parent_transformation)
	{
		game::bc_mesh_part_collider l_result;
		core::bc_string_frame l_mesh_name;
		core::bc_matrix4f l_node_transformation;

		bc_mesh_loader::convert_aimatrix(p_ainode.mTransformation, l_node_transformation);
		core::bc_matrix4f l_node_absolute_transformation = l_node_transformation * p_parent_transformation;

		for (bcUINT32 l_mesh_index = 0; l_mesh_index < p_ainode.mNumMeshes; ++l_mesh_index)
		{
			const aiMesh* l_mesh = p_aiscene.mMeshes[p_ainode.mMeshes[l_mesh_index]];
			l_mesh_name.assign(l_mesh->mName.data);

			if (l_mesh_name.compare("sphere") == 0)
			{
				physics::bc_shape_sphere l_px_sphere = bc_extract_sphere_from_points(physics::bc_bounded_strided_typed_data< core::bc_vector3f >
				(
					reinterpret_cast< const core::bc_vector3f* >(l_mesh->mVertices),
					sizeof(aiVector3D),
					l_mesh->mNumVertices
				));

				l_result.add_px_shape(l_px_sphere, l_node_transformation);
			}
			else if (l_mesh_name.compare("box") == 0)
			{
				physics::bc_shape_box l_px_box = bc_extract_box_from_points(physics::bc_bounded_strided_typed_data< core::bc_vector3f >
				(
					reinterpret_cast< const core::bc_vector3f* >(l_mesh->mVertices),
					sizeof(aiVector3D),
					l_mesh->mNumVertices
				));

				l_result.add_px_shape(l_px_box, l_node_transformation);
			}
			else if (l_mesh_name.compare("capsule") == 0)
			{
				physics::bc_shape_capsule l_px_capsule = bc_extract_capsule_from_points(physics::bc_bounded_strided_typed_data< core::bc_vector3f >
				(
					reinterpret_cast< const core::bc_vector3f* >(l_mesh->mVertices),
					sizeof(aiVector3D),
					l_mesh->mNumVertices
				));

				l_result.add_px_shape(l_px_capsule, l_node_transformation);
			}
			else if (l_mesh_name.compare("convex") == 0)
			{
				physics::bc_convex_mesh_desc l_px_convex_desc = bc_extract_convex_from_points(physics::bc_bounded_strided_typed_data< core::bc_vector3f >
				(
					reinterpret_cast< const core::bc_vector3f* >(l_mesh->mVertices),
					sizeof(aiVector3D),
					l_mesh->mNumVertices
				));

				physics::bc_mesh_buffer l_convex_buffer = p_physics.create_convex_mesh(l_px_convex_desc);
				physics::bc_convex_mesh_ref l_convex = p_physics.create_convex_mesh(l_convex_buffer);

				l_result.add_px_shape(std::move(l_convex), l_node_transformation);
			}
			else if (l_mesh_name.compare("mesh") == 0)
			{
				bool l_need_32bit_indices = l_mesh->mNumFaces * 3 > std::numeric_limits< bcUINT16 >::max();
				core::bc_vector_frame< bcBYTE > l_indices;
				l_indices.reserve(l_mesh->mNumFaces * 3 * (l_need_32bit_indices ? static_cast< bcINT >(game::bc_index_type::i32bit) : static_cast< bcINT >(game::bc_index_type::i16bit)));

				bcUINT16* l_16bit_indices = reinterpret_cast< bcUINT16* >(l_indices.data());
				bcUINT32* l_32bit_indices = reinterpret_cast< bcUINT32* >(l_indices.data());
				bcUINT32 l_index_count = 0;

				for (bcUINT l_face_index = 0; l_face_index < l_mesh->mNumFaces; ++l_face_index)
				{
					for (bcBYTE l_index = 0; l_index < 3; ++l_index)
					{
						if (l_need_32bit_indices)
						{
							*l_32bit_indices = static_cast< bcUINT32 >(l_mesh->mFaces[l_face_index].mIndices[l_index]);
							++l_32bit_indices;
						}
						else
						{
							*l_16bit_indices = static_cast< bcUINT16 >(l_mesh->mFaces[l_face_index].mIndices[l_index]);
							++l_16bit_indices;
						}

						++l_index_count;
					}
				}

				physics::bc_triangle_mesh_desc l_px_triangle_desc = bc_extract_mesh_from_points
				(
					physics::bc_bounded_strided_typed_data< core::bc_vector3f >
					(
						reinterpret_cast< const core::bc_vector3f* >(l_mesh->mVertices),
						sizeof(aiVector3D),
						l_mesh->mNumVertices
					),
					physics::bc_bounded_strided_data
					(
						l_indices.data(),
						l_need_32bit_indices ? static_cast< bcINT >(game::bc_index_type::i32bit) : static_cast< bcINT >(game::bc_index_type::i16bit),
						l_index_count
					)
				);

				physics::bc_mesh_buffer l_triangle_buffer = p_physics.create_triangle_mesh(l_px_triangle_desc);
				physics::bc_triangle_mesh_ref l_triangle_mesh = p_physics.create_triangle_mesh(l_triangle_buffer);

				physics::bc_transform l_transform = l_node_absolute_transformation;
				l_transform = l_transform.get_normalized();
				l_result.add_px_shape(std::move(l_triangle_mesh), l_transform);
			}
		}

		return l_result;
	}

	void bc_mesh_physics_loader::convert_nodes(game::bc_physics_system& p_physics_system,
		core::bc_content_loader_context& p_context,
		const aiScene& p_aiscene,
		const aiNode& p_ainode,
		const core::bc_matrix4f& p_parent_transformation,
		game::bc_mesh_collider& p_result)
	{
		if (is_px_node(p_ainode))
		{
			return;
		}

		core::bc_matrix4f l_node_transformation;
		core::bc_matrix4f l_node_absolute_transformation;

		bc_mesh_loader::convert_aimatrix(p_ainode.mTransformation, l_node_transformation);
		l_node_absolute_transformation = l_node_transformation * p_parent_transformation;

		for (bcUINT32 i = 0; i < p_ainode.mNumMeshes; ++i)
		{
			game::bc_mesh_part_collider l_mesh_colliders;

			aiMesh* l_aimesh = p_aiscene.mMeshes[p_ainode.mMeshes[i]];
			aiNode* l_px_node = find_px_node(p_ainode, *l_aimesh);

			if (l_px_node)
			{
				l_mesh_colliders = convert_px_node(p_physics_system.get_physics(), p_aiscene, *l_px_node, l_node_absolute_transformation);
				l_mesh_colliders.shrink_to_fit();
			}

			p_result.add_mesh_colliders(l_aimesh->mName.C_Str(), std::move(l_mesh_colliders));
		}

		for (bcUINT l_child_index = 0; l_child_index < p_ainode.mNumChildren; ++l_child_index)
		{
			aiNode* l_child = p_ainode.mChildren[l_child_index];

			convert_nodes(p_physics_system, p_context, p_aiscene, *l_child, l_node_absolute_transformation, p_result);
		}
	}

	void bc_mesh_physics_loader::content_offline_processing(core::bc_content_loader_context& p_context) const
	{
	}

	void bc_mesh_physics_loader::content_processing(core::bc_content_loader_context& p_context) const
	{
		Assimp::Importer l_importer;

		const aiScene* l_scene = l_importer.ReadFileFromMemory
		(
			p_context.m_data.data(),
			p_context.m_data.size(),
			aiProcess_GenNormals |
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |
			aiProcessPreset_TargetRealtime_Fast |
			graphic::bc_render_api_info::is_left_handed() ? aiProcess_ConvertToLeftHanded : 0
		);

		if (!l_scene || !l_scene->HasMeshes())
		{
			auto l_error_msg = core::bc_string_frame("Content file doesn't include mesh data") + core::bc_to_exclusive_string(p_context.m_file_path.c_str()).c_str();
			p_context.set_result(bc_io_exception(l_error_msg.c_str()));
			return;
		}

		game::bc_game_system& l_game_system = *core::bc_service_manager::get().get_service< game::bc_game_system >();
		game::bc_mesh_collider l_result;

		convert_nodes(l_game_system.get_physics_system(), p_context, *l_scene, *l_scene->mRootNode, core::bc_matrix4f::identity(), l_result);

		p_context.set_result(std::move(l_result));
	}
}