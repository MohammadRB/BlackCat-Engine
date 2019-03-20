// [05/04/2016 MRB]

#pragma once

#include "Core/Container/bcString.h"
#include "Core/Content/bcContent.h"
#include "Core/Content/bcContentLoader.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/Object/Mesh/bcMesh.h"
#include "BlackCat/bcExport.h"

#include "3rdParty/Assimp/Include/Importer.hpp"
#include "3rdParty/Assimp/Include/postprocess.h"
#include "3rdParty/Assimp/Include/scene.h"

namespace black_cat
{
	class BC_BLACKCAT_DLL bc_mesh_loader : public core::bc_base_content_loader
	{
	public:
		bc_mesh_loader() = default;

		bc_mesh_loader(bc_mesh_loader&&) = default;

		~bc_mesh_loader() = default;

		bc_mesh_loader& operator=(bc_mesh_loader&&) = default;

		static void calculate_node_mesh_count(const aiNode& p_node, bcSIZE& p_node_count, bcSIZE& p_mesh_count);

		static void convert_aimatrix(const aiMatrix4x4& p_aimatrix, core::bc_matrix4f& p_matrix);

		static void convert_aimaterial(core::bc_content_loading_context& p_context, const aiMaterial& p_aimaterial, game::bc_render_material& p_material);

		static void convert_aimesh(game::bc_render_system& p_render_system,
			core::bc_content_loading_context& p_context,
			const aiScene& p_aiscene,
			const aiNode& p_ainode,
			const aiMesh& p_aimesh,
			game::bc_mesh_part_data& p_mesh,
			game::bc_render_state_ptr& p_mesh_render_state);

		static void convert_ainodes(game::bc_render_system& p_render_system,
			core::bc_content_loading_context& p_context,
			const aiScene& p_aiscene,
			const aiNode& p_ainode,
			game::bc_mesh& p_mesh,
			game::bc_mesh_node* p_parent);

		bool support_offline_processing() const override;

		void content_processing(core::bc_content_loading_context& p_context) const override;

	protected:

	private:
	};
}