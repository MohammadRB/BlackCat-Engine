// [06/07/2016 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "Game/Object/Mesh/bcMesh.h"
#include "Game/Object/Mesh/bcMeshLevelOfDetail.h"
#include "Game/Object/Mesh/bcMeshRenderState.h"
#include "Game/Object/Mesh/bcMeshUtility.h"
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

			bc_sub_mesh(bc_mesh_ptr p_mesh, bc_mesh_node::node_index_t p_node_index);
			
			bc_sub_mesh(bc_mesh_ptr p_mesh, core::bc_string_view p_node_name);

			bc_sub_mesh(bc_sub_mesh&&) noexcept;

			~bc_sub_mesh();

			bc_sub_mesh& operator=(bc_sub_mesh&&) noexcept;

			const bc_mesh_ptr& get_mesh() const noexcept;
			
			bcFLOAT get_mesh_auto_scale() const noexcept;
			
			bcFLOAT get_mesh_scale() const noexcept;

			bool get_skinned() const noexcept;
			
			const bc_mesh_collider& get_mesh_collider() const noexcept;

			bc_mesh_level_of_detail get_mesh_level_of_detail() const noexcept;

			const bc_mesh_node* get_root_node() const noexcept;

			const bc_mesh_node* find_node(bc_mesh_node::node_index_t p_index) const noexcept;
			
			const bc_mesh_node* find_node(core::bc_string_view p_name) const noexcept;
			
			const bc_mesh_node* get_node_parent(const bc_mesh_node& p_node) const noexcept;

			const core::bc_vector<bc_mesh_node*>& get_node_children(const bc_mesh_node& p_node) const noexcept;

			const core::bc_matrix4f& get_node_transform(const bc_mesh_node& p_node) const noexcept;

			const core::bc_matrix4f& get_node_bind_pose_transform(const bc_mesh_node& p_node) const noexcept;
			
			const core::bc_matrix4f& get_node_inverse_bind_pose_transform(const bc_mesh_node& p_node) const noexcept;
			
			core::bc_string_view get_node_mesh_name(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			const bc_mesh_material& get_node_mesh_material(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;
			
			bc_mesh_material_ptr get_node_mesh_material_ptr(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			const bc_render_state& get_node_mesh_render_state(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			bc_render_state_ptr get_node_mesh_render_state_ptr(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			const physics::bc_bound_box& get_node_mesh_bound_box(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			core::bc_const_span<bc_mesh_part_collider_entry> get_node_mesh_colliders(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			const core::bc_matrix4f& get_node_absolute_transform(const bc_mesh_node& p_node, const bc_sub_mesh_mat4_transform& p_transforms) const noexcept;

			bc_mesh_render_state create_render_states(bc_render_system& p_render_system, const bcCHAR* p_mesh_prefix = nullptr) const noexcept;
			
			bc_mesh_render_state create_render_states(bc_render_system& p_render_system, const core::bc_json_key_value& p_mesh_materials, const bcCHAR* p_mesh_prefix = nullptr) const noexcept;
			
			void calculate_bound_box(const bc_sub_mesh_mat4_transform& p_world_transforms, physics::bc_bound_box& p_bound_box) const noexcept;
			
			void calculate_absolute_transforms(const core::bc_matrix4f& p_world, bc_sub_mesh_mat4_transform& p_world_transforms, physics::bc_bound_box& p_bound_box) const noexcept;

			void calculate_mesh_collider_transforms(const bc_sub_mesh_mat4_transform& p_model_transforms, bc_sub_mesh_px_transform& p_transforms) const noexcept;
			
			void calculate_skinned_mesh_collider_transforms(const bc_sub_mesh_mat4_transform& p_model_transforms, bc_sub_mesh_px_transform& p_transforms) const noexcept;

			/**
			 * \brief
			 * \tparam TArg
			 * \tparam TVisitFunc
			 * \tparam TUnVisitFunc
			 * \param p_arg Root argument
			 * \param p_visit_func Callable object (const bc_mesh_node&, TArg&) -> TArg
			 * \param p_unvisit_func Callable object (const bc_mesh_node&, const TArg&) -> void
			 */
			template<typename TArg, typename TVisitFunc, typename TUnVisitFunc>
			void iterate_over_nodes(TArg& p_arg, TVisitFunc p_visit_func, TUnVisitFunc p_unvisit_func) const noexcept;
			
		private:
			bc_mesh_ptr m_mesh;
			const bc_mesh_node* m_root_node;
		};

		inline const bc_mesh_ptr& bc_sub_mesh::get_mesh() const noexcept
		{
			return m_mesh;
		}

		inline bcFLOAT bc_sub_mesh::get_mesh_auto_scale() const noexcept
		{
			return m_mesh->get_auto_scale();
		}

		inline bcFLOAT bc_sub_mesh::get_mesh_scale() const noexcept
		{
			return m_mesh->get_scale();
		}

		inline bool bc_sub_mesh::get_skinned() const noexcept
		{
			return m_mesh->get_skinned();
		}

		inline const bc_mesh_collider& bc_sub_mesh::get_mesh_collider() const noexcept
		{
			return m_mesh->get_collider();
		}

		inline bc_mesh_level_of_detail bc_sub_mesh::get_mesh_level_of_detail() const noexcept
		{
			return m_mesh->get_level_of_details();
		}
		
		inline const bc_mesh_node* bc_sub_mesh::get_root_node() const noexcept
		{
			return m_root_node;
		}

		inline const bc_mesh_node* bc_sub_mesh::find_node(bc_mesh_node::node_index_t p_index) const noexcept
		{
			return m_mesh->find_node(p_index);
		}
		
		inline const bc_mesh_node* bc_sub_mesh::find_node(core::bc_string_view p_name) const noexcept
		{
			const auto* l_mesh_node = m_mesh->find_node(p_name);
			if(!l_mesh_node || l_mesh_node->get_index() < m_root_node->get_index())
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

		inline const core::bc_matrix4f& bc_sub_mesh::get_node_bind_pose_transform(const bc_mesh_node& p_node) const noexcept
		{
			return m_mesh->get_node_bind_pose_transform(p_node);
		}

		inline const core::bc_matrix4f& bc_sub_mesh::get_node_inverse_bind_pose_transform(const bc_mesh_node& p_node) const noexcept
		{
			return m_mesh->get_node_inverse_bind_pose_transform(p_node);
		}

		inline const core::bc_matrix4f& bc_sub_mesh::get_node_transform(const bc_mesh_node& p_node) const noexcept
		{
			return m_mesh->get_node_transform(p_node);
		}

		inline core::bc_string_view bc_sub_mesh::get_node_mesh_name(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const
		{
			return m_mesh->get_node_mesh_name(p_node, p_mesh_index);
		}

		inline const bc_mesh_material& bc_sub_mesh::get_node_mesh_material(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const
		{
			return m_mesh->get_node_mesh_material(p_node, p_mesh_index);
		}

		inline bc_mesh_material_ptr bc_sub_mesh::get_node_mesh_material_ptr(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const
		{
			return m_mesh->get_node_mesh_material_ptr(p_node, p_mesh_index);
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

		inline core::bc_const_span<bc_mesh_part_collider_entry> bc_sub_mesh::get_node_mesh_colliders(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const
		{
			return m_mesh->get_node_mesh_colliders(p_node, p_mesh_index);
		}

		inline const core::bc_matrix4f& bc_sub_mesh::get_node_absolute_transform(const bc_mesh_node& p_node, const bc_sub_mesh_mat4_transform& p_transforms) const noexcept
		{
			return p_transforms.get_node_transform(p_node);
		}

		inline bc_mesh_render_state bc_sub_mesh::create_render_states(bc_render_system& p_render_system, const bcCHAR* p_mesh_prefix) const noexcept
		{
			return bc_mesh_utility::create_mesh_render_states(p_render_system, *m_mesh, *m_root_node, p_mesh_prefix);
		}

		inline bc_mesh_render_state bc_sub_mesh::create_render_states(bc_render_system& p_render_system, const core::bc_json_key_value& p_mesh_materials, const bcCHAR* p_mesh_prefix) const noexcept
		{
			return bc_mesh_utility::create_mesh_render_states(p_render_system, *m_mesh, *m_root_node, p_mesh_materials, p_mesh_prefix);
		}
		
		inline void bc_sub_mesh::calculate_bound_box(const bc_sub_mesh_mat4_transform& p_world_transforms, physics::bc_bound_box& p_bound_box) const noexcept
		{
			bc_mesh_utility::calculate_bound_box(*m_mesh, p_world_transforms, p_bound_box);
		}

		inline void bc_sub_mesh::calculate_absolute_transforms(const core::bc_matrix4f& p_world, bc_sub_mesh_mat4_transform& p_world_transforms, physics::bc_bound_box& p_bound_box) const noexcept
		{
			bc_mesh_utility::calculate_absolute_transforms(*m_mesh, p_world, p_world_transforms, p_bound_box);
		}

		inline void bc_sub_mesh::calculate_mesh_collider_transforms(const bc_sub_mesh_mat4_transform& p_model_transforms, bc_sub_mesh_px_transform& p_transforms) const noexcept
		{
			BC_ASSERT(m_mesh->get_skinned());
			bc_mesh_utility::calculate_mesh_collider_transforms(*m_mesh, p_model_transforms, p_transforms);
		}
		
		inline void bc_sub_mesh::calculate_skinned_mesh_collider_transforms(const bc_sub_mesh_mat4_transform& p_model_transforms, bc_sub_mesh_px_transform& p_transforms) const noexcept
		{
			BC_ASSERT(m_mesh->get_skinned());
			bc_mesh_utility::calculate_skinned_mesh_collider_transforms(*m_mesh, p_model_transforms, p_transforms);
		}
		
		template<typename TArg, typename TVisitFunc, typename TUnVisitFunc>
		void bc_sub_mesh::iterate_over_nodes(TArg& p_arg, TVisitFunc p_visit_func, TUnVisitFunc p_unvisit_func) const noexcept
		{
			bc_mesh_utility::iterate_over_nodes(*m_mesh, p_arg, p_visit_func, p_unvisit_func, m_root_node);
		}
	}
}