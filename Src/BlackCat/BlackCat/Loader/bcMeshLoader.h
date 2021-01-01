// [05/04/2016 MRB]

#pragma once

#include "Core/Content/bcContent.h"
#include "Core/Content/bcContentLoader.h"
#include "Game/System/Render/bcMaterialManager.h"
#include "Game/Object/Mesh/bcMesh.h"
#include "BlackCat/bcExport.h"

#include "3rdParty/Assimp/Include/matrix4x4.h"

struct aiMaterial;
struct aiNode;
struct aiScene;
struct aiMesh;

namespace black_cat
{
	namespace game
	{
		class bc_render_system;
	}
	
	class BC_DLL bc_mesh_loader : public core::bc_base_content_loader
	{
	public:
		bc_mesh_loader() = default;

		bc_mesh_loader(bc_mesh_loader&&) = default;

		~bc_mesh_loader() = default;

		bc_mesh_loader& operator=(bc_mesh_loader&&) = default;

		static void calculate_node_mesh_count(const aiNode& p_node, bcSIZE& p_node_count, bcSIZE& p_mesh_count, core::bc_unordered_map_frame<const bcCHAR*, bcUINT32>& p_node_indices);

		static void convert_ai_matrix(const aiMatrix4x4& p_ai_matrix, core::bc_matrix4f& p_matrix);

		static void convert_ai_material(core::bc_content_loading_context& p_context, const aiMaterial& p_ai_material, game::bc_render_material_description& p_material);

		static void convert_ai_mesh(game::bc_render_system& p_render_system,
			core::bc_content_loading_context& p_context,
			const aiScene& p_ai_scene,
			const core::bc_unordered_map_frame<const bcCHAR*, bcUINT32>& p_node_indices,
			const aiNode& p_ai_node,
			const aiMesh& p_ai_mesh,
			game::bc_mesh_part_data& p_mesh,
			game::bc_render_state_ptr& p_mesh_render_state);

		static void convert_ai_nodes(game::bc_render_system& p_render_system,
			core::bc_content_loading_context& p_context,
			const aiScene& p_ai_scene,
			const core::bc_unordered_map_frame<const bcCHAR*, bcUINT32>& p_node_indices,
			const aiNode& p_ai_node,
			game::bc_mesh& p_mesh,
			game::bc_mesh_node* p_parent);

		bool support_offline_processing() const override;

		void content_processing(core::bc_content_loading_context& p_context) const override;
	};
}