// [01/18/2021 MRB]

#include "BlackCat/BlackCatPCH.h"
#include "BlackCat/Loader/bcMeshLoaderUtility.h"

#include "3rdParty/Assimp/Include/matrix4x4.h"
#include "3rdParty/Assimp/Include/material.h"
#include "3rdParty/Assimp/Include/mesh.h"
#include "3rdParty/Assimp/Include/scene.h"

namespace black_cat
{
	void bc_mesh_loader_utility::convert_ai_matrix(const aiMatrix4x4& p_ai_matrix, core::bc_matrix4f& p_matrix)
	{
		p_matrix[0] = p_ai_matrix.a1;
		p_matrix[1] = p_ai_matrix.b1;
		p_matrix[2] = p_ai_matrix.c1;
		p_matrix[3] = p_ai_matrix.d1;
		p_matrix[4] = p_ai_matrix.a2;
		p_matrix[5] = p_ai_matrix.b2;
		p_matrix[6] = p_ai_matrix.c2;
		p_matrix[7] = p_ai_matrix.d2;
		p_matrix[8] = p_ai_matrix.a3;
		p_matrix[9] = p_ai_matrix.b3;
		p_matrix[10] = p_ai_matrix.c3;
		p_matrix[11] = p_ai_matrix.d3;
		p_matrix[12] = p_ai_matrix.a4;
		p_matrix[13] = p_ai_matrix.b4;
		p_matrix[14] = p_ai_matrix.c4;
		p_matrix[15] = p_ai_matrix.d4;
	}
	
	bool bc_mesh_loader_utility::is_px_node(const aiNode& p_ai_node)
	{
		const bcCHAR* l_px_str = "px.";
		return std::strncmp(l_px_str, p_ai_node.mName.data, std::strlen(l_px_str)) == 0;
	}
	
	void bc_mesh_loader_utility::calculate_node_mapping(const aiNode& p_ai_node, core::bc_unordered_map_frame<const bcCHAR*, bcUINT32>& p_node_mapping)
	{
		if (bc_mesh_loader_utility::is_px_node(p_ai_node))
		{
			return;
		}

		p_node_mapping.insert(std::make_pair(p_ai_node.mName.C_Str(), p_node_mapping.size()));

		for (bcUINT32 l_child_ite = 0; l_child_ite < p_ai_node.mNumChildren; ++l_child_ite)
		{
			calculate_node_mapping(*p_ai_node.mChildren[l_child_ite], p_node_mapping);
		}
	}

	void bc_mesh_loader_utility::calculate_px_node_mapping(const aiScene& p_ai_scene,
		const aiNode& p_ai_node,
		core::bc_unordered_map_frame<const bcCHAR*, core::bc_vector_frame<const aiNode*>>& p_px_node_mapping)
	{
		if (bc_mesh_loader_utility::is_px_node(p_ai_node))
		{
			return;
		}
		
		for (bcUINT32 l_i = 0; l_i < p_ai_node.mNumMeshes; ++l_i)
		{
			aiMesh* l_ai_mesh = p_ai_scene.mMeshes[p_ai_node.mMeshes[l_i]];
			core::bc_vector_frame<const aiNode*> l_ai_mesh_colliders;

			core::bc_string_frame l_px_node_name;
			l_px_node_name.reserve(3 + l_ai_mesh->mName.length);
			l_px_node_name.append("px.");
			l_px_node_name.append(l_ai_mesh->mName.data);

			for (bcUINT32 l_index = 0; l_index < p_ai_node.mNumChildren; ++l_index)
			{
				if (l_px_node_name.compare(0, l_px_node_name.size(), p_ai_node.mChildren[l_index]->mName.data) == 0)
				{
					l_ai_mesh_colliders.push_back(p_ai_node.mChildren[l_index]);
				}
			}

			if(!l_ai_mesh_colliders.empty())
			{
				p_px_node_mapping.insert(std::make_pair(l_ai_mesh->mName.data, std::move(l_ai_mesh_colliders)));
			}
		}

		for (bcUINT32 l_child_ite = 0; l_child_ite < p_ai_node.mNumChildren; ++l_child_ite)
		{
			calculate_px_node_mapping(p_ai_scene , *p_ai_node.mChildren[l_child_ite], p_px_node_mapping);
		}
	}

	void bc_mesh_loader_utility::calculate_skinned_px_node_mapping(const aiScene& p_ai_scene,
		const aiNode& p_ai_node,
		core::bc_unordered_map_frame<const bcCHAR*, core::bc_vector_frame<const aiNode*>>& p_px_node_mapping)
	{
		if (bc_mesh_loader_utility::is_px_node(p_ai_node))
		{
			return;
		}

		core::bc_string_frame l_px_node_name;
		l_px_node_name.reserve(3 + p_ai_node.mName.length);
		l_px_node_name.append("px.");
		l_px_node_name.append(p_ai_node.mName.data);

		core::bc_vector_frame<const aiNode*> l_ai_node_colliders;
		
		for(bcUINT32 l_child_ite = 0; l_child_ite < p_ai_scene.mRootNode->mNumChildren; ++l_child_ite)
		{
			const aiNode* l_child_node = p_ai_scene.mRootNode->mChildren[l_child_ite];

			if(l_px_node_name.compare(0, l_px_node_name.size(), l_child_node->mName.C_Str()) == 0)
			{
				l_ai_node_colliders.push_back(l_child_node);
			}

			//// Search RootNode and RootNode children
			//for (bcUINT32 l_lvl2_child_ite = 0; l_lvl2_child_ite < l_child_node->mNumChildren; ++l_lvl2_child_ite)
			//{
			//	const aiNode* l_lvl2_child_node = l_child_node->mChildren[l_lvl2_child_ite];

			//	if (l_px_node_name.compare(0, l_px_node_name.size(), l_lvl2_child_node->mName.C_Str()) == 0)
			//	{
			//		l_ai_node_colliders.push_back(l_lvl2_child_node);
			//	}
			//}
		}

		if(!l_ai_node_colliders.empty())
		{
			p_px_node_mapping.insert(std::make_pair(p_ai_node.mName.C_Str(), std::move(l_ai_node_colliders)));
		}

		for (bcUINT32 l_child_ite = 0; l_child_ite < p_ai_node.mNumChildren; ++l_child_ite)
		{
			calculate_skinned_px_node_mapping(p_ai_scene , *p_ai_node.mChildren[l_child_ite], p_px_node_mapping);
		}
	}
}