// [01/18/2021 MRB]

#pragma once

#include "Core/Math/bcMatrix4f.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Container/bcVector.h"
#include "BlackCat/bcExport.h"

template<typename TReal>
class aiMatrix4x4t;
struct aiMaterial;
struct aiNode;
struct aiScene;
struct aiMesh;

namespace black_cat
{
	class BC_DLL bc_mesh_loader_utility
	{
	public:
		static void convert_ai_matrix(const aiMatrix4x4t<float>& p_ai_matrix, core::bc_matrix4f& p_matrix);
		
		static bool is_px_node(const aiNode& p_ai_node);
		
		/**
		 * \brief Create a map between nodes and their hierarchy index which can be used in transforms array indexing
		 * \param p_ai_node 
		 * \param p_node_mapping 
		 */
		static void calculate_node_mapping(const aiNode& p_ai_node, core::bc_unordered_map_frame<const bcCHAR*, bcUINT32>& p_node_mapping);

		/**
		 * \brief Find px-nodes associated with a particular mesh
		 * \param p_ai_scene
		 * \param p_ai_node 
		 * \param p_px_node_mapping 
		 */
		static void calculate_px_node_mapping(const aiScene& p_ai_scene, 
			const aiNode& p_ai_node, 
			core::bc_unordered_map_frame<const bcCHAR*, core::bc_vector_frame<const aiNode*>>& p_px_node_mapping);

		/**
		 * \brief Find px-nodes associated with a particular node in a skinned mesh
		 * \param p_ai_scene 
		 * \param p_ai_node 
		 * \param p_px_node_mapping 
		 */
		static void calculate_skinned_px_node_mapping(const aiScene& p_ai_scene,
			const aiNode& p_ai_node,
			core::bc_unordered_map_frame<const bcCHAR*, core::bc_vector_frame<const aiNode*>>& p_px_node_mapping);
	};
}