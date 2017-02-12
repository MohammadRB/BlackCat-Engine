// [02/08/2017 MRB]

#pragma once

#include "Core/File/bcContentLoader.h"
#include "Game/System/Physics/bcPhysicsSystem.h"
#include "Game/Object/Mesh/bcMeshCollider.h"
#include "BlackCat/bcExport.h"

#include "3rdParty/Assimp/include/assimp/scene.h"

namespace black_cat
{
	class BC_BLACKCAT_DLL bc_mesh_physics_loader : public core::bc_base_content_loader
	{
	public:
		bc_mesh_physics_loader() = default;

		bc_mesh_physics_loader(bc_mesh_physics_loader&&) = default;

		~bc_mesh_physics_loader() = default;

		bc_mesh_physics_loader& operator=(bc_mesh_physics_loader&&) = default;

		static bool is_px_node(const aiNode& p_node);

		static aiNode* find_px_node(const aiNode& p_ainode, const aiMesh& p_ainode_mesh);

		static game::bc_mesh_part_collider convert_px_node(physics::bc_physics& p_physics,
			const aiScene& p_aiscene,
			const aiNode& p_ainode,
			const core::bc_matrix4f& p_parent_transformation);

		static void  convert_nodes(game::bc_physics_system& p_physics_system,
			core::bc_content_loader_context& p_context,
			const aiScene& p_aiscene,
			const aiNode& p_ainode,
			const core::bc_matrix4f& p_parent_transformation,
			game::bc_mesh_collider& p_result);

		void content_offline_processing(core::bc_content_loader_context& p_context) const override;

		void content_processing(core::bc_content_loader_context& p_context) const override;
	};
}