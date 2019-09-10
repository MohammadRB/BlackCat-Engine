// [06/07/2016 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "Game/Object/Mesh/bcMesh.h"
#include "Game/Object/Mesh/bcSubMeshTransformation.h"

namespace black_cat
{
	namespace game
	{
		/**
		 * \brief Wrap a mesh so that every mesh parts of a mesh can be an individual entity
		 */
		class BC_GAME_DLL bc_sub_mesh
		{
		public:
			bc_sub_mesh();

			bc_sub_mesh(bc_mesh_ptr p_mesh);

			bc_sub_mesh(bc_mesh_ptr p_mesh, const bcCHAR* p_node);

			bc_sub_mesh(bc_sub_mesh&&) noexcept;

			~bc_sub_mesh();

			bc_sub_mesh& operator=(bc_sub_mesh&&) noexcept;

			bc_mesh_ptr get_mesh() const;

			const bc_mesh_node* get_root_node() const;

			const bc_mesh_node* get_parent_node(const bc_mesh_node* p_node) const;

			const core::bc_vector< bc_mesh_node* >& get_node_children(const bc_mesh_node* p_node) const;

			const core::bc_matrix4f& get_node_transformation(const bc_mesh_node* p_node) const;

			const core::bc_string& get_node_mesh_name(const bc_mesh_node* p_node, bcUINT32 p_mesh_index) const;

			const bc_render_material& get_node_mesh_material(const bc_mesh_node* p_node, bcUINT32 p_mesh_index) const;

			const bc_render_state& get_node_mesh_render_state(const bc_mesh_node* p_node, bcUINT32 p_mesh_index) const;

			const physics::bc_bound_box& get_node_mesh_bound_box(const bc_mesh_node* p_node, bcUINT32 p_mesh_index) const;

			const bc_mesh_part_collider& get_node_mesh_colliders(const bc_mesh_node* p_node, bcUINT32 p_mesh_index) const;

			const core::bc_matrix4f& get_node_absolute_transformation(const bc_mesh_node* p_node, const bc_sub_mesh_transformation& p_transformations) const;

			void calculate_absolute_transformations(const core::bc_matrix4f& p_world, bc_sub_mesh_transformation& p_result, physics::bc_bound_box& p_bound_box) const;

		protected:

		private:
			bc_mesh_ptr m_mesh;
			const bc_mesh_node* m_root_node;
		};

		inline bc_mesh_ptr bc_sub_mesh::get_mesh() const
		{
			return m_mesh;
		}

		inline const bc_mesh_node* bc_sub_mesh::get_root_node() const
		{
			return m_root_node;
		}

		inline const bc_mesh_node* bc_sub_mesh::get_parent_node(const bc_mesh_node* p_node) const
		{
			return m_mesh->get_node_parent(p_node);
		}
		
		inline const core::bc_vector<bc_mesh_node*>& bc_sub_mesh::get_node_children(const bc_mesh_node* p_node) const
		{
			return m_mesh->get_node_children(p_node);
		}

		inline const core::bc_matrix4f& bc_sub_mesh::get_node_transformation(const bc_mesh_node* p_node) const
		{
			return m_mesh->get_node_transformation(p_node);
		}

		inline const core::bc_string& bc_sub_mesh::get_node_mesh_name(const bc_mesh_node* p_node, bcUINT32 p_mesh_index) const
		{
			return m_mesh->get_node_mesh_name(p_node, p_mesh_index);
		}

		inline const bc_render_material& bc_sub_mesh::get_node_mesh_material(const bc_mesh_node* p_node, bcUINT32 p_mesh_index) const
		{
			return m_mesh->get_node_mesh_material(p_node, p_mesh_index);
		}

		inline const bc_render_state& bc_sub_mesh::get_node_mesh_render_state(const bc_mesh_node* p_node, bcUINT32 p_mesh_index) const
		{
			return m_mesh->get_node_mesh_render_state(p_node, p_mesh_index);
		}

		inline const physics::bc_bound_box& bc_sub_mesh::get_node_mesh_bound_box(const bc_mesh_node* p_node, bcUINT32 p_mesh_index) const
		{
			return m_mesh->get_node_mesh_bound_box(p_node, p_mesh_index);
		}

		inline const bc_mesh_part_collider& bc_sub_mesh::get_node_mesh_colliders(const bc_mesh_node* p_node, bcUINT32 p_mesh_index) const
		{
			return m_mesh->get_node_mesh_colliders(p_node, p_mesh_index);
		}

		inline const core::bc_matrix4f& bc_sub_mesh::get_node_absolute_transformation(const bc_mesh_node* p_node, const bc_sub_mesh_transformation& p_transformations) const
		{
			return p_transformations.get_node_transform(*p_node);
		}

		inline void bc_sub_mesh::calculate_absolute_transformations(const core::bc_matrix4f& p_world, bc_sub_mesh_transformation& p_result, physics::bc_bound_box& p_bound_box) const
		{
			m_mesh->calculate_absolute_transformations(p_world, p_result, p_bound_box);
		}
	}
}