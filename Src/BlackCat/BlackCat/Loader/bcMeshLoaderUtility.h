// [01/18/2021 MRB]

#pragma once

#include "Core/Math/bcMatrix4f.h"
#include "Core/Container/bcUnorderedMap.h"
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
		static bool is_px_node(const aiNode& p_ai_node);
		
		static void calculate_node_mapping(const aiNode& p_node, core::bc_unordered_map_frame<const bcCHAR*, bcUINT32>& p_bone_mapping);

		static void convert_ai_matrix(const aiMatrix4x4t<float>& p_ai_matrix, core::bc_matrix4f& p_matrix);
	};
}