// [01/18/2021 MRB]

#include "App/AppPCH.h"
#include "App/Loader/bcMeshLoaderUtility.h"
#include "3rdParty/Assimp/Include/assimp/matrix4x4.h"
#include "3rdParty/Assimp/Include/assimp/material.h"
#include "3rdParty/Assimp/Include/assimp/mesh.h"
#include "3rdParty/Assimp/Include/assimp/scene.h"

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
		const core::bc_string_view l_px_str = bc_mesh_loader_utility::s_px_node_prefix;
		const auto l_length_to_compare = l_px_str.size();
		return l_px_str.compare(0, l_length_to_compare, &p_ai_node.mName.data[0], l_length_to_compare) == 0;
	}

	bool bc_mesh_loader_utility::is_px_joint_node(const aiNode& p_ai_node)
	{
		const core::bc_string_view l_px_str = bc_mesh_loader_utility::s_px_joint_node_prefix;
		const auto l_length_to_compare = l_px_str.size();
		return l_px_str.compare(0, l_length_to_compare, &p_ai_node.mName.data[0], l_length_to_compare) == 0;
	}

	void bc_mesh_loader_utility::calculate_node_mapping(const aiNode& p_ai_node, core::bc_unordered_map_frame<core::bc_string_view, bcUINT32>& p_node_mapping)
	{
		if (bc_mesh_loader_utility::is_px_node(p_ai_node))
		{
			return;
		}

		p_node_mapping.insert(std::make_pair(core::bc_string_view(p_ai_node.mName.C_Str()), p_node_mapping.size()));

		for (bcUINT32 l_child_ite = 0; l_child_ite < p_ai_node.mNumChildren; ++l_child_ite)
		{
			calculate_node_mapping(*p_ai_node.mChildren[l_child_ite], p_node_mapping);
		}
	}

	void bc_mesh_loader_utility::calculate_px_node_mapping(const aiScene& p_ai_scene,
		const aiNode& p_ai_node,
		core::bc_unordered_map_frame<core::bc_string_view, core::bc_vector_frame<const aiNode*>>& p_px_node_mapping)
	{
		if (is_px_node(p_ai_node) || is_px_joint_node(p_ai_node))
		{
			return;
		}

		core::bc_string_frame l_px_node_name;
		l_px_node_name.reserve(3 + p_ai_node.mName.length);
		l_px_node_name.append(bc_mesh_loader_utility::s_px_node_prefix);
		l_px_node_name.append(p_ai_node.mName.data);

		auto l_ai_node_colliders = core::bc_vector_frame<const aiNode*>();

		for (bcUINT32 l_i = 0U; l_i < p_ai_node.mNumChildren; ++l_i)
		{
			const auto* l_ai_child_node = p_ai_node.mChildren[l_i];
			if (std::strncmp(l_ai_child_node->mName.data, l_px_node_name.data(), l_px_node_name.size()) == 0)
			{
				l_ai_node_colliders.push_back(l_ai_child_node);
			}
		}

		l_px_node_name = core::bc_string_frame();

		if (!l_ai_node_colliders.empty())
		{
			p_px_node_mapping.insert(std::make_pair(core::bc_string_view(p_ai_node.mName.C_Str()), std::move(l_ai_node_colliders)));
		}
		
		for (bcUINT32 l_child_ite = 0; l_child_ite < p_ai_node.mNumChildren; ++l_child_ite)
		{
			calculate_px_node_mapping(p_ai_scene , *p_ai_node.mChildren[l_child_ite], p_px_node_mapping);
		}
	}

	void bc_mesh_loader_utility::calculate_skinned_px_node_mapping(const aiScene& p_ai_scene,
		const aiNode& p_ai_node,
		core::bc_unordered_map_frame<core::bc_string_view, core::bc_vector_frame<const aiNode*>>& p_px_node_mapping)
	{
		if (is_px_node(p_ai_node) || is_px_joint_node(p_ai_node))
		{
			return;
		}

		core::bc_string_frame l_px_node_name;
		l_px_node_name.reserve(3 + p_ai_node.mName.length);
		l_px_node_name.append(bc_mesh_loader_utility::s_px_node_prefix);
		l_px_node_name.append(p_ai_node.mName.data);

		core::bc_vector_frame<const aiNode*> l_ai_node_colliders;
		
		for(bcUINT32 l_child_ite = 0; l_child_ite < p_ai_scene.mRootNode->mNumChildren; ++l_child_ite)
		{
			const auto* l_child_node = p_ai_scene.mRootNode->mChildren[l_child_ite];
			if (std::strcmp(l_child_node->mName.data, l_px_node_name.data()) == 0)
			{
				l_ai_node_colliders.push_back(l_child_node);
			}
		}

		l_px_node_name = core::bc_string_frame();
		
		if(!l_ai_node_colliders.empty())
		{
			p_px_node_mapping.insert(std::make_pair(core::bc_string_view(p_ai_node.mName.C_Str()), std::move(l_ai_node_colliders)));
		}

		for (bcUINT32 l_child_ite = 0; l_child_ite < p_ai_node.mNumChildren; ++l_child_ite)
		{
			calculate_skinned_px_node_mapping(p_ai_scene , *p_ai_node.mChildren[l_child_ite], p_px_node_mapping);
		}
	}

	void bc_mesh_loader_utility::calculate_px_joint_mapping(const aiScene& p_ai_scene, core::bc_vector<std::tuple<core::bc_string_view, core::bc_string_view, physics::bc_transform>>& p_px_joint_mapping)
	{
		for (bcUINT32 l_child_ite = 0; l_child_ite < p_ai_scene.mRootNode->mNumChildren; ++l_child_ite)
		{
			const aiNode* l_child_node = p_ai_scene.mRootNode->mChildren[l_child_ite];
			if(!is_px_joint_node(*l_child_node))
			{
				continue;
			}

			core::bc_string_view l_node_name = l_child_node->mName.data;
			l_node_name = l_node_name.substr(9);

			const auto l_split_pos = l_node_name.find(bc_mesh_loader_utility::s_px_joint_node_split);
			const auto l_node1 = l_node_name.substr(0, l_split_pos);
			const auto l_node2 = l_node_name.substr(l_split_pos + 1, l_node_name.size() - l_split_pos);

			core::bc_matrix4f l_transform;
			convert_ai_matrix(l_child_node->mTransformation, l_transform);
			
			p_px_joint_mapping.push_back(std::make_tuple(l_node1, l_node2, physics::bc_transform(l_transform)));
		}
	}

	void bc_mesh_loader_utility::store_skinned_vertex_weights(core::bc_vector4i& p_indices,
		core::bc_vector4f& p_weights,
		bcUINT32 p_bone_index,
		bcFLOAT p_bone_weight)
	{
		auto* l_vertex_ids_array = &p_indices.x;
		auto* l_vertex_weights_array = &p_weights.x;

		auto l_free_weight_slot = -1;
		for (auto l_ite = 0U; l_ite < 4; ++l_ite)
		{
			if (l_vertex_weights_array[l_ite] <= 0)
			{
				l_free_weight_slot = l_ite;
				l_vertex_ids_array[l_ite] = p_bone_index;
				l_vertex_weights_array[l_ite] = p_bone_weight;
				break;
			}
		}

		// All weight slots are occupied, replace the one with lowest weight
		if (l_free_weight_slot == -1)
		{
			auto l_lowest_weight_index = 0;
			for (auto l_ite = 1U; l_ite < 4; ++l_ite)
			{
				if (l_vertex_weights_array[l_ite] < l_vertex_weights_array[l_lowest_weight_index])
				{
					l_lowest_weight_index = l_ite;
				}
			}

			l_vertex_ids_array[l_lowest_weight_index] = p_bone_index;
			l_vertex_weights_array[l_lowest_weight_index] = p_bone_weight;
		}
	}

	void bc_mesh_loader_utility::clean_skinned_vertex_weights(core::bc_vector4i& p_indices, core::bc_vector4f& p_weights, bcFLOAT p_weight_threshold)
	{
		auto* l_vertex_ids_array = &p_indices.x;
		auto* l_vertex_weights_array = &p_weights.x;
		
		for (auto l_ite = 0U; l_ite < 4; ++l_ite)
		{
			if(l_vertex_weights_array[l_ite] <= p_weight_threshold)
			{
				l_vertex_weights_array[l_ite] = 0;
				l_vertex_ids_array[l_ite] = 0;
			}
		}
	}

	void bc_mesh_loader_utility::sort_skinned_vertex_weights(core::bc_vector4i& p_indices, core::bc_vector4f& p_weights)
	{
		auto* l_vertex_ids_array = &p_indices.x;
		auto* l_vertex_weights_array = &p_weights.x;

		for (auto l_ite = 0U; l_ite < 3; ++l_ite)
		{
			if (l_vertex_weights_array[l_ite] < l_vertex_weights_array[l_ite + 1])
			{
				const auto l_temp_weight = l_vertex_weights_array[l_ite];
				const auto l_temp_index = l_vertex_ids_array[l_ite];

				l_vertex_weights_array[l_ite] = l_vertex_weights_array[l_ite + 1];
				l_vertex_ids_array[l_ite] = l_vertex_ids_array[l_ite + 1];
				l_vertex_weights_array[l_ite + 1] = l_temp_weight;
				l_vertex_ids_array[l_ite + 1] = l_temp_index;
			}
		}
	}
}