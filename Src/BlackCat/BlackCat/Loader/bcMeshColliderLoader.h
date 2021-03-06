// [02/08/2017 MRB]

#pragma once

#include "Core/Content/bcContentLoader.h"
#include "Game/System/Physics/bcPhysicsSystem.h"
#include "Game/Object/Mesh/bcMeshCollider.h"
#include "Game/Object/Mesh/bcSkinnedMeshCollider.h"
#include "BlackCat/bcExport.h"

struct aiNode;
struct aiScene;
struct aiMesh;

namespace black_cat
{
	class BC_DLL bc_mesh_collider_loader : public core::bc_base_content_loader
	{
	public:
		bc_mesh_collider_loader() = default;

		bc_mesh_collider_loader(bc_mesh_collider_loader&&) = default;

		~bc_mesh_collider_loader() = default;

		bc_mesh_collider_loader& operator=(bc_mesh_collider_loader&&) = default;

		bool support_offline_processing() const override;

		void content_processing(core::bc_content_loading_context& p_context) const override;

	private:
		aiNode* find_px_node(const aiNode& p_ai_node, const aiMesh& p_ai_node_mesh) const;

		void convert_px_node(physics::bc_physics& p_physics,
			const core::bc_unordered_map_frame<const bcCHAR*, bcUINT32>& p_node_mapping,
			const aiScene& p_ai_scene,
			const aiNode& p_attached_node,
			const aiNode& p_px_node,
			bool p_high_detail_query_shape,
			bool p_skinned,
			game::bc_mesh_part_collider& p_collider) const;

		void convert_nodes(core::bc_content_loading_context& p_context,
			physics::bc_physics& p_physics,
			const core::bc_unordered_map_frame<const bcCHAR*, bcUINT32>& p_node_mapping,
			const core::bc_unordered_map_frame<const bcCHAR*, core::bc_vector_frame<const aiNode*>>& p_px_node_mapping,
			const aiScene& p_ai_scene,
			const aiNode& p_ai_node,
			bool p_high_detail_query_shape,
			bool p_skinned,
			game::bc_mesh_collider& p_collider,
			game::bc_skinned_mesh_collider& p_skinned_collider) const;
	};
}