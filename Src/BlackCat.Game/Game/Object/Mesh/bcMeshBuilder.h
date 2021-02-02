// [01/07/2021 MRB]

#pragma once

#include "Core/Math/bcMatrix4f.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "Core/Utility/bcNullable.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "Game/Object/Mesh/bcMesh.h"
#include "Game/Object/Mesh/bcMeshCollider.h"
#include "Game/Object/Mesh/bcRenderMaterial.h"
#include "Game/System/Render/State/bcRenderState.h"
#include "Game/System/Render/State/bcVertexLayout.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		struct bc_mesh_builder_mesh_part
		{
			core::bc_string_frame m_name;
			bc_render_material_ptr m_material;
			core::bc_vector_movable< bc_vertex_pos_tex_nor_tan > m_vertices;
			core::bc_vector_movable< bc_vertex_pos_tex_nor_tan_bon > m_skinned_vertices;
			core::bc_vector_movable< bcBYTE > m_indices;
			physics::bc_bound_box m_bound_box;

			graphic::bc_buffer_ptr m_vertex_buffer;
			graphic::bc_buffer_ptr m_index_buffer;
			bc_render_state_ptr m_render_state;
		};
		
		struct bc_mesh_builder_mesh_node
		{
			core::bc_string_frame m_name;
			bcINT32 m_parent_index;
			core::bc_vector_frame< bcINT32 > m_child_indices;
			core::bc_matrix4f m_transform;
			core::bc_matrix4f m_bone_offset;
			core::bc_vector_frame<bc_mesh_builder_mesh_part> m_mesh_parts;
		};
		
		class BC_GAME_DLL bc_mesh_builder
		{
			friend class bc_mesh;
			
		public:
			bc_mesh_builder() = default;

			bc_mesh_builder(bc_mesh_builder&&) noexcept = default;

			~bc_mesh_builder() = default;

			bc_mesh_builder& operator=(bc_mesh_builder&&) noexcept = default;
			
			bc_mesh_builder& add_node(const bcCHAR* p_parent_name, const bcCHAR* p_name, const core::bc_matrix4f& p_transform);

			bc_mesh_builder& add_bone(const bcCHAR* p_name, const core::bc_matrix4f& p_bone_offset);

			bc_mesh_builder& add_mesh_part(const bcCHAR* p_node_name,
				const bcCHAR* p_name,
				bc_render_material_ptr p_material,
				core::bc_vector_movable< bc_vertex_pos_tex_nor_tan > p_vertices,
				core::bc_vector_movable< bcBYTE > p_indices,
				physics::bc_bound_box& p_bound_box,
				graphic::bc_buffer_ptr p_vertex_buffer,
				graphic::bc_buffer_ptr p_index_buffer,
				bc_render_state_ptr p_render_state);

			bc_mesh_builder& add_skinned_mesh_part(const bcCHAR* p_node_name,
				const bcCHAR* p_name,
				bc_render_material_ptr p_material,
				core::bc_vector_movable< bc_vertex_pos_tex_nor_tan_bon > p_vertices,
				core::bc_vector_movable< bcBYTE > p_indices,
				physics::bc_bound_box& p_bound_box,
				graphic::bc_buffer_ptr p_vertex_buffer,
				graphic::bc_buffer_ptr p_index_buffer,
				bc_render_state_ptr p_render_state);

			bc_mesh_builder& with_auto_scale(bcFLOAT p_scale);

			bc_mesh_builder& with_lod(bc_mesh_ptr p_mesh);

			bc_mesh build(const bcCHAR* p_name, bc_mesh_collider_ptr p_collider);
			
		private:
			core::bc_string m_name;
			core::bc_nullable<bcFLOAT> m_auto_scale;
			bool m_skinned = false;
			bcUINT32 m_node_count = 0;
			bcUINT32 m_mesh_part_count = 0;
			core::bc_vector_frame<bc_mesh_builder_mesh_node> m_nodes;
			core::bc_vector_frame< bc_mesh_ptr > m_level_of_details;
		};
	}
}