// [06/07/2016 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "Game/Object/Mesh/bcMesh.h"
#include "Game/Object/Mesh/bcSubMeshTransform.h"

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
			bc_sub_mesh() noexcept;

			explicit bc_sub_mesh(bc_mesh_ptr p_mesh) noexcept;

			bc_sub_mesh(bc_mesh_ptr p_mesh, const bcCHAR* p_node);

			bc_sub_mesh(bc_sub_mesh&&) noexcept;

			~bc_sub_mesh();

			bc_sub_mesh& operator=(bc_sub_mesh&&) noexcept;

			const bc_mesh_ptr& get_mesh() const noexcept;

			bcFLOAT get_mesh_scale() const noexcept;
			
			const bc_mesh_collider& get_mesh_collider() const noexcept;

			const bc_mesh_node* get_root_node() const noexcept;

			const bc_mesh_node* find_node(const bcCHAR* p_name) const noexcept;
			
			const bc_mesh_node* get_node_parent(const bc_mesh_node& p_node) const noexcept;

			const core::bc_vector< bc_mesh_node* >& get_node_children(const bc_mesh_node& p_node) const noexcept;

			const core::bc_matrix4f& get_node_transform(const bc_mesh_node& p_node) const noexcept;

			const core::bc_matrix4f& get_node_offset_transform(const bc_mesh_node& p_node) const noexcept;

			const core::bc_matrix4f& get_node_inverse_bind_pose_transform(const bc_mesh_node& p_node) const noexcept;

			const core::bc_string& get_node_mesh_name(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			const bc_render_material& get_node_mesh_material(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			const bc_render_state& get_node_mesh_render_state(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			bc_render_state_ptr get_node_mesh_render_state_ptr(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			const physics::bc_bound_box& get_node_mesh_bound_box(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			const bc_mesh_part_collider& get_node_mesh_colliders(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			const core::bc_matrix4f& get_node_absolute_transform(const bc_mesh_node& p_node, const bc_sub_mesh_transform& p_transforms) const noexcept;

			void calculate_bound_box(const bc_sub_mesh_transform& p_transforms, physics::bc_bound_box& p_bound_box) const noexcept;
			
			void calculate_absolute_transforms(const core::bc_matrix4f& p_world, bc_sub_mesh_transform& p_transforms, physics::bc_bound_box& p_bound_box) const noexcept;

			template< typename TArg, typename TCallable >
			void iterate_over_nodes(TArg& p_arg, TCallable p_callable) const noexcept;
			
		private:
			bc_mesh_ptr m_mesh;
			const bc_mesh_node* m_root_node;
		};

		inline const bc_mesh_ptr& bc_sub_mesh::get_mesh() const noexcept
		{
			return m_mesh;
		}

		inline bcFLOAT bc_sub_mesh::get_mesh_scale() const noexcept
		{
			return m_mesh->get_scale();
		}

		inline const bc_mesh_collider& bc_sub_mesh::get_mesh_collider() const noexcept
		{
			return m_mesh->get_collider();
		}
		
		inline const bc_mesh_node* bc_sub_mesh::get_root_node() const noexcept
		{
			return m_root_node;
		}

		inline const bc_mesh_node* bc_sub_mesh::find_node(const bcCHAR* p_name) const noexcept
		{
			const auto* l_mesh_node = m_mesh->find_node(p_name);
			if(!l_mesh_node || l_mesh_node->get_transform_index() < m_root_node->get_transform_index())
			{
				return nullptr;
			}

			return l_mesh_node;
		}
		
		inline const bc_mesh_node* bc_sub_mesh::get_node_parent(const bc_mesh_node& p_node) const noexcept
		{
			return m_mesh->get_node_parent(p_node);
		}
		
		inline const core::bc_vector<bc_mesh_node*>& bc_sub_mesh::get_node_children(const bc_mesh_node& p_node) const noexcept
		{
			return m_mesh->get_node_children(p_node);
		}

		inline const core::bc_matrix4f& bc_sub_mesh::get_node_offset_transform(const bc_mesh_node& p_node) const noexcept
		{
			return m_mesh->get_node_offset_transform(p_node);
		}

		inline const core::bc_matrix4f& bc_sub_mesh::get_node_inverse_bind_pose_transform(const bc_mesh_node& p_node) const noexcept
		{
			return m_mesh->get_node_inverse_bind_pose_transform(p_node);
		}

		inline const core::bc_matrix4f& bc_sub_mesh::get_node_transform(const bc_mesh_node& p_node) const noexcept
		{
			return m_mesh->get_node_transform(p_node);
		}

		inline const core::bc_string& bc_sub_mesh::get_node_mesh_name(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const
		{
			return m_mesh->get_node_mesh_name(p_node, p_mesh_index);
		}

		inline const bc_render_material& bc_sub_mesh::get_node_mesh_material(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const
		{
			return m_mesh->get_node_mesh_material(p_node, p_mesh_index);
		}

		inline const bc_render_state& bc_sub_mesh::get_node_mesh_render_state(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const
		{
			return m_mesh->get_node_mesh_render_state(p_node, p_mesh_index);
		}

		inline bc_render_state_ptr bc_sub_mesh::get_node_mesh_render_state_ptr(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const
		{
			return m_mesh->get_node_mesh_render_state_ptr(p_node, p_mesh_index);
		}

		inline const physics::bc_bound_box& bc_sub_mesh::get_node_mesh_bound_box(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const
		{
			return m_mesh->get_node_mesh_bound_box(p_node, p_mesh_index);
		}

		inline const bc_mesh_part_collider& bc_sub_mesh::get_node_mesh_colliders(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const
		{
			return m_mesh->get_node_mesh_colliders(p_node, p_mesh_index);
		}

		inline const core::bc_matrix4f& bc_sub_mesh::get_node_absolute_transform(const bc_mesh_node& p_node, const bc_sub_mesh_transform& p_transforms) const noexcept
		{
			return p_transforms.get_node_transform(p_node);
		}

		inline void bc_sub_mesh::calculate_bound_box(const bc_sub_mesh_transform& p_transforms, physics::bc_bound_box& p_bound_box) const noexcept
		{
			m_mesh->calculate_bound_box(p_transforms, p_bound_box);
		}
		
		inline void bc_sub_mesh::calculate_absolute_transforms(const core::bc_matrix4f& p_world, bc_sub_mesh_transform& p_transforms, physics::bc_bound_box& p_bound_box) const noexcept
		{
			m_mesh->calculate_absolute_transforms(p_world, p_transforms, p_bound_box);
		}

		template< typename TArg, typename TCallable >
		void bc_sub_mesh::iterate_over_nodes(TArg& p_arg, TCallable p_callable) const noexcept
		{
			m_mesh->iterate_over_nodes(p_arg, p_callable, m_root_node);
		}
	}
}