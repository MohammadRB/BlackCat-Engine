// [01/18/2021 MRB]

#include "BlackCat/BlackCatPCH.h"
#include "BlackCat/Loader/bcMeshLoaderUtility.h"

#include "3rdParty/Assimp/Include/matrix4x4.h"
#include "3rdParty/Assimp/Include/material.h"
#include "3rdParty/Assimp/Include/mesh.h"
#include "3rdParty/Assimp/Include/scene.h"

namespace black_cat
{
	bool bc_mesh_loader_utility::is_px_node(const aiNode& p_ai_node)
	{
		const bcCHAR* l_px_str = "px.";
		return std::strncmp(l_px_str, p_ai_node.mName.data, std::strlen(l_px_str)) == 0;
	}
	
	void bc_mesh_loader_utility::calculate_node_mapping(const aiNode& p_node, core::bc_unordered_map_frame<const bcCHAR*, bcUINT32>& p_bone_mapping)
	{
		if (bc_mesh_loader_utility::is_px_node(p_node))
		{
			return;
		}

		p_bone_mapping.insert(std::make_pair(p_node.mName.C_Str(), p_bone_mapping.size()));

		for (bcUINT32 l_child_ite = 0; l_child_ite < p_node.mNumChildren; ++l_child_ite)
		{
			calculate_node_mapping(*p_node.mChildren[l_child_ite], p_bone_mapping);
		}
	}

	void bc_mesh_loader_utility::convert_ai_matrix(const aiMatrix4x4& p_ai_matrix, core::bc_matrix4f& p_matrix)
	{
		aiMatrix4x4 l_ai_matrix = p_ai_matrix;
		l_ai_matrix.Transpose();

		p_matrix[0] = l_ai_matrix.a1;
		p_matrix[1] = l_ai_matrix.a2;
		p_matrix[2] = l_ai_matrix.a3;
		p_matrix[3] = l_ai_matrix.a4;
		p_matrix[4] = l_ai_matrix.b1;
		p_matrix[5] = l_ai_matrix.b2;
		p_matrix[6] = l_ai_matrix.b3;
		p_matrix[7] = l_ai_matrix.b4;
		p_matrix[8] = l_ai_matrix.c1;
		p_matrix[9] = l_ai_matrix.c2;
		p_matrix[10] = l_ai_matrix.c3;
		p_matrix[11] = l_ai_matrix.c4;
		p_matrix[12] = l_ai_matrix.d1;
		p_matrix[13] = l_ai_matrix.d2;
		p_matrix[14] = l_ai_matrix.d3;
		p_matrix[15] = l_ai_matrix.d4;
	}
}