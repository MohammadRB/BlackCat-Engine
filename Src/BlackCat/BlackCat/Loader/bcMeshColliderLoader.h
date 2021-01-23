// [02/08/2017 MRB]

#pragma once

#include "Core/Content/bcContentLoader.h"
#include "Game/System/Physics/bcPhysicsSystem.h"
#include "Game/Object/Mesh/bcMeshCollider.h"
#include "BlackCat/bcExport.h"

struct aiNode;
struct aiScene;
struct aiMesh;

namespace black_cat
{
	class BC_DLL bc_mesh_collider_loader : public core::bc_base_content_loader
	{
	public:
		explicit bc_mesh_collider_loader(bool p_high_detail_collision_shape);

		bc_mesh_collider_loader(bc_mesh_collider_loader&&) = default;

		~bc_mesh_collider_loader() = default;

		bc_mesh_collider_loader& operator=(bc_mesh_collider_loader&&) = default;

		bool support_offline_processing() const override;

		void content_processing(core::bc_content_loading_context& p_context) const override;

	private:
		aiNode* find_px_node(const aiNode& p_ai_node, const aiMesh& p_ai_node_mesh) const;

		void convert_px_node(physics::bc_physics& p_physics,
			const aiScene& p_ai_scene,
			const aiNode& p_attached_node,
			const aiNode& p_px_node,
			bool p_high_detail_query_shape,
			const core::bc_unordered_map_frame<const bcCHAR*, bcUINT32>& p_node_mapping,
			game::bc_mesh_part_collider& p_result) const;

		void convert_nodes(physics::bc_physics& p_physics,
			core::bc_content_loading_context& p_context,
			const aiScene& p_ai_scene,
			const aiNode& p_ai_node,
			bool p_high_detail_query_shape,
			bool p_skinned,
			const core::bc_unordered_map_frame<const bcCHAR*, bcUINT32>& p_node_mapping,
			const core::bc_unordered_map_frame<const bcCHAR*, core::bc_vector_frame<const aiNode*>>& p_px_node_mapping,
			game::bc_mesh_collider& p_result) const;

		bool m_high_detail_query_shape;
	};
}