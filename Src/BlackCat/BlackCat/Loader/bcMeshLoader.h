// [05/04/2016 MRB]

#pragma once

#include "Core/Content/bcContent.h"
#include "Core/Content/bcContentLoader.h"
#include "Game/System/Render/Material/bcMeshMaterial.h"
#include "Game/Object/Mesh/bcMesh.h"
#include "Game/Object/Mesh/bcMeshBuilder.h"
#include "BlackCat/bcExport.h"

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
	
	/**
	 * \brief Input mesh must have scale 1
	 */
	class BC_DLL bc_mesh_loader : public core::bc_base_content_loader
	{
	public:
		bc_mesh_loader() = default;

		bc_mesh_loader(bc_mesh_loader&&) = default;

		~bc_mesh_loader() = default;

		bc_mesh_loader& operator=(bc_mesh_loader&&) = default;

		bool support_offline_processing() const override;

		void content_processing(core::bc_content_loading_context& p_context) const override;

	private:
		static void fill_skinned_vertices(const aiMesh& p_ai_mesh,
			const core::bc_unordered_map_frame<std::string_view, bcUINT32>& p_node_mapping,
			core::bc_vector_movable<game::bc_vertex_pos_tex_nor_tan_bon>& p_vertices,
			game::bc_mesh_builder& p_builder);

		static void convert_ai_material(core::bc_content_loading_context& p_context,
			const aiMaterial& p_ai_material,
			game::bc_mesh_material_description& p_material);

		static void convert_ai_mesh(game::bc_render_system& p_render_system,
			core::bc_content_loading_context& p_context,
			const aiScene& p_ai_scene,
			const core::bc_unordered_map_frame<std::string_view, bcUINT32>& p_node_mapping,
			const aiNode& p_ai_node,
			const aiMesh& p_ai_mesh,
			game::bc_mesh_builder& p_builder);

		static void convert_ai_nodes(game::bc_render_system& p_render_system,
			core::bc_content_loading_context& p_context,
			const aiScene& p_ai_scene,
			const core::bc_unordered_map_frame<std::string_view, bcUINT32>& p_node_mapping,
			const aiNode& p_ai_node,
			game::bc_mesh_builder& p_builder);
	};
}