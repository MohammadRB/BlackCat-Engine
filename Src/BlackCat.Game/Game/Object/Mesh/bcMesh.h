// [05/01/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "CorePlatform/Utility/bcNoCopy.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Content/bcContent.h"
#include "Core/Math/bcVector4f.h"
#include "Core/Math/bcMatrix4f.h"
#include "Graphic/bcRenderApiInfo.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "Game/bcExport.h"
#include "Game/System/Render/State/bcVertexLayout.h"
#include "Game/System/Render/State/bcRenderState.h"
#include "Game/Object/Mesh/bcMeshCollider.h"
#include "Game/Object/Mesh/bcRenderMaterial.h"

namespace black_cat
{
	namespace game
	{
		class bc_sub_mesh_transformation;
		class bc_mesh;

		struct bc_mesh_part_cbuffer
		{
			BC_CBUFFER_ALIGN core::bc_vector4f m_diffuse;
			BC_CBUFFER_ALIGN bcFLOAT m_specular_intensity;
			bcFLOAT m_specular_power;
		};

		struct bc_mesh_part_data
		{
		public:
			core::bc_string m_name;
			bc_render_material_ptr m_material;
			core::bc_vector_movale< bc_vertex_pos_tex_nor_tan > m_vertices;
			core::bc_vector_movale< bcBYTE > m_indices;

			graphic::bc_buffer_ptr m_vertex_buffer;
			graphic::bc_buffer_ptr m_index_buffer;

			physics::bc_bound_box m_bound_box;
		};

		class BC_GAME_DLL bc_mesh_node : public core_platform::bc_no_copy
		{
		public:
			friend class bc_mesh;
			using node_index = core::bc_vector< void* >::size_type;
			const static node_index s_invalid_index = node_index(-1);

		public:
			bc_mesh_node(core::bc_string p_name,
				bc_mesh_node* p_parent,
				node_index p_transformation_index,
				node_index p_first_mesh_index,
				bcSIZE p_mesh_count);

			bc_mesh_node(bc_mesh_node&&) = default;

			~bc_mesh_node() = default;

			bc_mesh_node& operator=(bc_mesh_node&&) = default;

			node_index get_transformation_index() const;

			node_index get_mesh_count() const;

			bcSIZE get_children_count() const;

			bcSIZE get_all_children_count() const;

			const core::bc_string& get_name() const;

		protected:

		private:
			void _add_child(bc_mesh_node* p_mesh_node);

			core::bc_string m_name;
			bc_mesh_node* m_parent;
			core::bc_vector< bc_mesh_node* > m_children;
			node_index m_transformation_index;
			node_index m_first_mesh_index;
			bcSIZE m_mesh_count;
		};

		class BC_GAME_DLL bc_mesh : public core::bc_icontent
		{
			BC_CONTENT(mesh)

		public:
			friend class bc_mesh_node;

		private:
			using node_index = bc_mesh_node::node_index;
			using hash_t = std::hash< const bcCHAR* >;

		public:
			explicit bc_mesh(core::bc_string p_name, bcSIZE p_node_count, bcSIZE p_mesh_count, bc_mesh_collider_ptr p_colliders);

			bc_mesh(bc_mesh&&) = default;

			~bc_mesh() = default;

			bc_mesh& operator=(bc_mesh&&) = default;

			const core::bc_string& get_name() const;

			const bc_mesh_node* get_root() const;

			/**
			 * \brief Return nullptr if no node found
			 * \param p_name 
			 * \return 
			 */
			const bc_mesh_node* find_node(const bcCHAR* p_name) const;

			const bc_mesh_node* get_node_parent(const bc_mesh_node& p_node) const;

			const core::bc_vector< bc_mesh_node* >& get_node_children(const bc_mesh_node& p_node) const;

			const core::bc_matrix4f& get_node_transformation(const bc_mesh_node& p_node) const;

			const core::bc_string& get_node_mesh_name(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			const bc_render_material& get_node_mesh_material(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			const bc_render_state& get_node_mesh_render_state(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			bc_render_state_ptr get_node_mesh_render_state_ptr(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			const physics::bc_bound_box& get_node_mesh_bound_box(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			const bc_mesh_part_collider& get_node_mesh_colliders(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			void calculate_absolute_transformations(const core::bc_matrix4f& p_world, bc_sub_mesh_transformation& p_result, physics::bc_bound_box& p_bound_box) const;

			/**
			 * \brief Add a mesh part to node hierarchy of mesh. Adding a node with null parent will override root node of mesh
			 * \param p_name 
			 * \param p_parent 
			 * \param p_transformation 
			 * \param p_meshes 
			 * \return 
			 */
			bc_mesh_node* _add_node(core::bc_string p_name,
				bc_mesh_node* p_parent,
				core::bc_matrix4f& p_transformation,
				const core::bc_vector_frame<std::tuple<bc_mesh_part_data, bc_render_state_ptr>>& p_meshes);

			void _apply_auto_scale(bcFLOAT p_scale);

		private:
			void _calculate_absolute_transformations(const core::bc_matrix4f& p_parent_transformation,
				const bc_mesh_node* p_begin,
				const bc_mesh_node* p_end,
				bc_sub_mesh_transformation& p_result,
				physics::bc_bound_box& p_bound_box) const;

			core::bc_string m_name;
			bc_mesh_node* m_root;
			core::bc_vector< bc_mesh_node > m_nodes;										// Don't use movable memory due to raw pointers in bc_mesh_node
			core::bc_vector< bc_render_state_ptr > m_render_states;						// Place render states along with nodes
			core::bc_vector_movale< core::bc_matrix4f > m_transformations;
			bc_mesh_collider_ptr m_colliders;
			core::bc_vector< const bc_mesh_part_collider* > m_colliders_map;				// Used to fetch mesh colliders without need to hash looking in bc_mesh_collider
			core::bc_unordered_map< hash_t::result_type, bc_mesh_node* > m_nodes_map;
			core::bc_vector< bc_mesh_part_data > m_meshes;
			bcFLOAT m_scale;
		};

		using bc_mesh_ptr = core::bc_content_ptr< bc_mesh >;
	}
}