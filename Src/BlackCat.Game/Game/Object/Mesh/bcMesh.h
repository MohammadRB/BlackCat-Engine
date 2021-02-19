// [05/01/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Content/bcContent.h"
#include "Core/Math/bcMatrix4f.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "Game/bcExport.h"
#include "Game/System/Render/State/bcVertexLayout.h"
#include "Game/System/Render/State/bcRenderState.h"
#include "Game/System/Render/Material/bcMeshMaterial.h"
#include "Game/Object/Mesh/bcMeshNode.h"
#include "Game/Object/Mesh/bcMeshCollider.h"
#include "Game/Object/Mesh/bcMeshLevelOfDetail.h"

namespace black_cat
{
	namespace game
	{
		class bc_mesh_builder;

		struct bc_mesh_part_data
		{
			core::bc_string m_name;
			bc_mesh_material_ptr m_material;
			core::bc_vector_movable< bc_vertex_pos_tex_nor_tan > m_vertices;
			core::bc_vector_movable< bc_vertex_pos_tex_nor_tan_bon > m_skinned_vertices;
			core::bc_vector_movable< bcUINT16 > m_16bit_indices;
			core::bc_vector_movable< bcUINT32 > m_32bit_indices;
			physics::bc_bound_box m_bound_box;

			graphic::bc_buffer_ptr m_vertex_buffer;
			graphic::bc_buffer_ptr m_index_buffer;
		};

		class BC_GAME_DLL bc_mesh : public core::bci_content
		{
			BC_CONTENT(mesh)

		private:
			using node_index_t = bc_mesh_node::node_index_t;
			using hash_t = std::hash< const bcCHAR* >;

		public:
			bc_mesh(bc_mesh_builder p_builder, bc_mesh_collider_ptr p_colliders);

			bc_mesh(bc_mesh&& p_other) noexcept;

			~bc_mesh() = default;

			bc_mesh& operator=(bc_mesh&& p_other) noexcept;

			const bcCHAR* get_name() const noexcept;

			bcFLOAT get_auto_scale() const noexcept;
			
			bcFLOAT get_scale() const noexcept;

			bool get_skinned() const noexcept;

			bcSIZE get_node_count() const noexcept;

			bcSIZE get_mesh_count() const noexcept;
			
			const bc_mesh_collider& get_collider() const noexcept;

			const bc_mesh_node* get_root() const noexcept;

			const bc_mesh_node* find_node(bc_mesh_node::node_index_t p_index) const noexcept;
			
			const bc_mesh_node* find_node(const bcCHAR* p_name) const noexcept;

			const bc_mesh_node* get_node_parent(const bc_mesh_node& p_node) const noexcept;

			const core::bc_vector< bc_mesh_node* >& get_node_children(const bc_mesh_node& p_node) const noexcept;

			const core::bc_matrix4f& get_node_transform(const bc_mesh_node& p_node) const noexcept;
			
			const core::bc_matrix4f& get_node_inverse_bind_pose_transform(const bc_mesh_node& p_node) const noexcept;
			
			const core::bc_string& get_node_mesh_name(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			const bc_mesh_material& get_node_mesh_material(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;
			
			bc_mesh_material_ptr get_node_mesh_material_ptr(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			const bc_render_state& get_node_mesh_render_state(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			bc_render_state_ptr get_node_mesh_render_state_ptr(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			const physics::bc_bound_box& get_node_mesh_bound_box(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			const bc_mesh_part_collider& get_node_mesh_colliders(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			bc_mesh_level_of_detail get_level_of_details() const noexcept;
			
		private:
			void _apply_auto_scale(bcFLOAT p_auto_scale);

			void _calculate_inverse_bind_pose();
			
			void _calculate_collider_initial_transforms();
			
			core::bc_string m_name;
			bcFLOAT m_auto_scale;
			bcFLOAT m_scale;
			bool m_skinned;
			bc_mesh_node* m_root;
			core::bc_vector< bc_mesh_node > m_nodes;
			core::bc_unordered_map< hash_t::result_type, bc_mesh_node* > m_nodes_map;
			core::bc_vector_movable< core::bc_matrix4f > m_transformations;
			core::bc_vector_movable< core::bc_matrix4f > m_inverse_bind_poses;
			core::bc_vector< bc_mesh_part_data > m_meshes;
			core::bc_vector< bc_render_state_ptr > m_render_states;
			bc_mesh_collider_ptr m_collider;
			core::bc_vector< const bc_mesh_part_collider* > m_collider_map;

			core::bc_vector< core::bc_content_ptr< bc_mesh > > m_level_of_details;
			core::bc_vector_movable< const bc_mesh* > m_level_of_details_map;
		};

		using bc_mesh_ptr = core::bc_content_ptr< bc_mesh >;

		inline const bcCHAR* bc_mesh::get_name() const noexcept
		{
			return m_name.c_str();
		}

		inline bcFLOAT bc_mesh::get_auto_scale() const noexcept
		{
			return m_auto_scale;
		}
		
		inline bcFLOAT bc_mesh::get_scale() const noexcept
		{
			return m_scale;
		}

		inline bool bc_mesh::get_skinned() const noexcept
		{
			return m_skinned;
		}

		inline bcSIZE bc_mesh::get_node_count() const noexcept
		{
			return m_nodes.size();
		}

		inline bcSIZE bc_mesh::get_mesh_count() const noexcept
		{
			return m_meshes.size();
		}
		
		inline const bc_mesh_collider& bc_mesh::get_collider() const noexcept
		{
			return *m_collider;
		}

		inline const bc_mesh_node* bc_mesh::get_root() const noexcept
		{
			return m_root;
		}
		
		inline const bc_mesh_node* bc_mesh::get_node_parent(const bc_mesh_node& p_node) const noexcept
		{
			return p_node.m_parent;
		}

		inline const core::bc_vector<bc_mesh_node*>& bc_mesh::get_node_children(const bc_mesh_node& p_node) const noexcept
		{
			return p_node.m_children;
		}

		inline const core::bc_matrix4f& bc_mesh::get_node_transform(const bc_mesh_node& p_node) const noexcept
		{
			return m_transformations[p_node.m_transform_index];
		}
		
		inline const core::bc_matrix4f& bc_mesh::get_node_inverse_bind_pose_transform(const bc_mesh_node& p_node) const noexcept
		{
			return m_inverse_bind_poses[p_node.m_transform_index];
		}

		inline bc_mesh_level_of_detail bc_mesh::get_level_of_details() const noexcept
		{
			return bc_mesh_level_of_detail(m_level_of_details_map.data(), m_level_of_details_map.size());
		}
	}
}