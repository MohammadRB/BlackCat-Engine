// [02/08/2017 MRB]

#pragma once

#include "Core/Content/bcContentLoader.h"
#include "Game/System/Physics/bcPhysicsSystem.h"
#include "Game/Object/Mesh/bcMeshCollider.h"
#include "BlackCat/bcExport.h"

#include "3rdParty/Assimp/include/assimp/scene.h"

namespace black_cat
{
	class BC_BLACKCAT_DLL bc_mesh_collider_loader : public core::bc_base_content_loader
	{
	public:
		explicit bc_mesh_collider_loader(bool p_high_detail_collision_shape);

		bc_mesh_collider_loader(bc_mesh_collider_loader&&) = default;

		~bc_mesh_collider_loader() = default;

		bc_mesh_collider_loader& operator=(bc_mesh_collider_loader&&) = default;

		bool static is_px_node(const aiNode& p_node);

		bool support_offline_processing() const override;

		void content_processing(core::bc_content_loading_context& p_context) const override;

	protected:
		aiNode* find_px_node(const aiNode& p_ainode, const aiMesh& p_ainode_mesh) const;

		game::bc_mesh_part_collider convert_px_node(physics::bc_physics& p_physics,
			const aiScene& p_aiscene,
			const aiNode& p_ainode,
			const core::bc_matrix4f& p_parent_transformation,
			bool p_generate_high_detail_query_shape) const;

		void convert_nodes(physics::bc_physics& p_physics,
			core::bc_content_loading_context& p_context,
			const aiScene& p_aiscene,
			const aiNode& p_ainode,
			const core::bc_matrix4f& p_parent_transformation,
			bool p_generate_high_detail_query_shape,
			game::bc_mesh_collider& p_result) const;

	protected:
		bool m_high_detail_query_shape;
	};
}