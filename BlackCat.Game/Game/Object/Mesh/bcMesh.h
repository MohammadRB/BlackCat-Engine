// [05/01/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "CorePlatform/Utility/bcNoCopy.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/File/bcContent.h"
#include "Graphic/Math/bcVector4f.h"
#include "Graphic/Math/bcMatrix4f.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "Game/bcExport.h"
#include "Game/System/Render/bcVertexLayout.h"
#include "Game/System/Render/bcRenderState.h"

namespace black_cat
{
	namespace game
	{
		class bc_mesh;

		struct bc_mesh_node_material
		{
		public:
			~bc_mesh_node_material()
			{
				
			}

			graphic::bc_vector4f m_diffuse;
			bcFLOAT m_specular_intency;
			bcFLOAT m_specular_power;

			graphic::bc_texture2d_content_ptr m_diffuse_map;
			graphic::bc_texture2d_content_ptr m_specular_map;
			graphic::bc_texture2d_content_ptr m_normal_map;
		};

		struct bc_mesh_part_cbuffer
		{
			BC_CBUFFER_ALIGN graphic::bc_vector4f m_diffuse;
			BC_CBUFFER_ALIGN bcFLOAT m_specular_intency;
			bcFLOAT m_specular_power;
		};

		class BC_GAME_DLL bc_mesh_node : public core_platform::bc_no_copy
		{
		public:
			friend class bc_mesh;
			using node_indexing = core::bc_vector< void* >::size_type;

		public:
			bc_mesh_node(core::bc_string p_name, node_indexing p_data_index, bc_mesh_node* p_parent, const bc_render_state_ptr& p_render_state);

			bc_mesh_node(bc_mesh_node&&) = default;

			~bc_mesh_node() = default;

			bc_mesh_node& operator=(bc_mesh_node&&) = default;

			node_indexing get_data_idnex() const;

			const core::bc_string& get_name() const;

		protected:

		private:
			void _add_child(bc_mesh_node* p_mesh_part);

			core::bc_string m_name;
			node_indexing m_data_index;
			bc_mesh_node* m_parent;
			core::bc_vector< bc_mesh_node* > m_childs; 
			bc_render_state_ptr m_render_state;
		};

		class BC_GAME_DLL bc_mesh : public core::bc_icontent
		{
		public:
			friend class bc_mesh_node;
			using hash_t = std::hash< const bcCHAR* >;

		private:
			using node_indexing = bc_mesh_node::node_indexing;

		public:
			explicit bc_mesh(core::bc_string p_name, bcSIZE p_node_count);

			bc_mesh(bc_mesh&&) = default;

			~bc_mesh() = default;

			bc_mesh& operator=(bc_mesh&&) = default;

			bc_index_type index_layout() const;

			const core::bc_string& get_name() const;

			const bc_mesh_node* get_root() const;

			// Return nullptr if no node found
			const bc_mesh_node* find_node(const bcCHAR* p_name) const;

			const bc_mesh_node* get_parent_node(const bc_mesh_node* p_node) const;

			const graphic::bc_matrix4f* get_node_transformation(const bc_mesh_node* p_node) const;

			const bc_mesh_node_material* get_node_material(const bc_mesh_node* p_node) const;

			const bc_render_state_ptr& get_node_render_state(const bc_mesh_node* p_node) const;

			const core::bc_vector< bc_mesh_node* >& get_node_childs(const bc_mesh_node* p_node) const;

			const core::bc_vector_movale< bc_vertex_pos_tex_nor_tan >& get_node_vertices(const bc_mesh_node* p_node) const;

			const core::bc_vector_movale< bcBYTE >& get_node_indices(const bc_mesh_node* p_node) const;

			// Add a mesh part to node hierarchy of mesh. Adding a node with null parent will override root node of mesh
			bc_mesh_node* _add_node(core::bc_string p_name,
				bc_mesh_node* p_parent,
				graphic::bc_matrix4f& p_transformation,
				bc_mesh_node_material&& p_material,
				const bc_render_state_ptr& p_render_state,
				core::bc_vector_frame<bc_vertex_pos_tex_nor_tan>& p_vertices,
				core::bc_vector_frame<bcBYTE>& p_indices);

			static const bcCHAR* content_name()
			{
				return core::g_cnt_mesh;
			}

		protected:

		private:
			bc_index_type m_index_layout;
			core::bc_string m_name;
			bc_mesh_node* m_root;
			core::bc_vector< bc_mesh_node > m_nodes; // Don't use movable memory due to raw pointers in bc_mesh_node
			core::bc_unordered_map< hash_t::result_type, bc_mesh_node* > m_nodes_hash;
			core::bc_vector_movale< graphic::bc_matrix4f > m_transformations;
			core::bc_vector_movale< bc_mesh_node_material > m_materials;
			core::bc_vector< core::bc_vector_movale< bc_vertex_pos_tex_nor_tan > > m_vertices;
			core::bc_vector< core::bc_vector_movale< bcBYTE > > m_indices;
		};

		using bc_mesh_ptr = core::bc_content_ptr< bc_mesh >;
	}
}