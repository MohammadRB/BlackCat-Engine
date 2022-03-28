// [02/08/2017 MRB]

#pragma once

#include "Core/Content/bcContentLoader.h"
#include "Game/System/Physics/bcPhysicsSystem.h"
#include "Game/Object/Mesh/bcMeshColliderBuilder.h"
#include "App/bcExport.h"

struct aiScene;
struct aiNode;
struct aiMesh;

namespace black_cat
{
	class BC_DLL bc_mesh_collider_loader : public core::bc_base_content_loader
	{
	public:
		bc_mesh_collider_loader() = default;

		bc_mesh_collider_loader(bc_mesh_collider_loader&&) = default;

		~bc_mesh_collider_loader() override = default;

		bc_mesh_collider_loader& operator=(bc_mesh_collider_loader&&) = default;

		bool support_offline_processing() const override;

		void content_processing(core::bc_content_loading_context& p_context) const override;

	private:
		aiNode* find_px_node(const aiNode& p_ai_node, const aiMesh& p_ai_node_mesh) const;

		void convert_px_node(physics::bc_physics& p_physics,
			const core::bc_unordered_map_frame<core::bc_string_view, bcUINT32>& p_node_mapping,
			const aiScene& p_ai_scene,
			const bcCHAR* p_attached_mesh_name,
			const aiNode& p_attached_node,
			const aiNode& p_px_node,
			bool p_high_detail_query_shape,
			bool p_skinned,
			game::bc_mesh_collider_builder& p_builder) const;

		void convert_nodes(core::bc_content_loading_context& p_context,
			physics::bc_physics& p_physics,
			const core::bc_unordered_map_frame<core::bc_string_view, bcUINT32>& p_node_mapping,
			const core::bc_unordered_map_frame<core::bc_string_view, core::bc_vector_frame<const aiNode*>>& p_px_node_mapping,
			const aiScene& p_ai_scene,
			const aiNode& p_ai_node,
			bool p_high_detail_query_shape,
			bool p_skinned,
			game::bc_mesh_collider_builder& p_builder) const;
	};
}