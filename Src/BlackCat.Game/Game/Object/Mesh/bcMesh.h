// [05/01/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
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
#include "Game/Object/Mesh/bcMeshCollider.h"
#include "Game/Object/Mesh/bcRenderMaterial.h"
#include "Game/Object/Mesh/bcMeshNode.h"
#include "Game/Object/Mesh/bcMeshBuilder.h"

namespace black_cat
{
	namespace game
	{
		class bc_sub_mesh_transform;

		struct bc_mesh_part_data
		{
			core::bc_string m_name;
			bc_render_material_ptr m_material;
			core::bc_vector_movable< bc_vertex_pos_tex_nor_tan > m_vertices;
			core::bc_vector_movable< bc_vertex_pos_tex_nor_tan_bon > m_skinned_vertices;
			core::bc_vector_movable< bcBYTE > m_indices;
			physics::bc_bound_box m_bound_box;

			graphic::bc_buffer_ptr m_vertex_buffer;
			graphic::bc_buffer_ptr m_index_buffer;
		};

		class BC_GAME_DLL bc_mesh : public core::bc_icontent
		{
			BC_CONTENT(mesh)

		private:
			using node_index_t = bc_mesh_node::node_index_t;
			using hash_t = std::hash< const bcCHAR* >;

		public:
			explicit bc_mesh(bc_mesh_builder p_builder, bc_mesh_collider_ptr p_colliders);

			bc_mesh(bc_mesh&&) = default;

			~bc_mesh() = default;

			bc_mesh& operator=(bc_mesh&&) = default;

			const core::bc_string& get_name() const noexcept;

			const bc_mesh_node* get_root() const noexcept;

			bcFLOAT get_scale() const noexcept;

			const core::bc_vector_movable< core::bc_matrix4f >& get_bone_offsets() const noexcept;
			
			const core::bc_vector_movable< core::bc_matrix4f >& get_inverse_bind_poses() const noexcept;
			
			/**
			 * \brief Return nullptr if no node found
			 * \param p_name 
			 * \return 
			 */
			const bc_mesh_node* find_node(const bcCHAR* p_name) const noexcept;

			const bc_mesh_node* get_node_parent(const bc_mesh_node& p_node) const noexcept;

			const core::bc_vector< bc_mesh_node* >& get_node_children(const bc_mesh_node& p_node) const noexcept;

			const core::bc_matrix4f& get_node_offset_transform(const bc_mesh_node& p_node) const noexcept;
			
			const core::bc_matrix4f& get_node_transform(const bc_mesh_node& p_node) const noexcept;

			const core::bc_string& get_node_mesh_name(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			const bc_render_material& get_node_mesh_material(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			const bc_render_state& get_node_mesh_render_state(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			bc_render_state_ptr get_node_mesh_render_state_ptr(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			const physics::bc_bound_box& get_node_mesh_bound_box(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;

			const bc_mesh_part_collider& get_node_mesh_colliders(const bc_mesh_node& p_node, bcUINT32 p_mesh_index) const;
			
			void calculate_bound_box(const bc_sub_mesh_transform& p_absolute_transforms, physics::bc_bound_box& p_bound_box) const noexcept;
			
			void calculate_absolute_transforms(const core::bc_matrix4f& p_world, bc_sub_mesh_transform& p_transforms, physics::bc_bound_box& p_bound_box) const noexcept;

			template< typename TArg, typename TCallable >
			void iterate_over_nodes(TArg& p_arg, TCallable p_callable, const bc_mesh_node* p_node = nullptr) const noexcept;

		private:			
			void _apply_auto_scale(bcFLOAT p_scale);

			template< typename TArg, typename TCallable >
			void _iterate_over_nodes(TArg& p_arg, TCallable& p_callable, const bc_mesh_node* const* p_begin, const bc_mesh_node* const* p_end) const noexcept;

			core::bc_string m_name;
			bc_mesh_node* m_root;
			core::bc_vector< bc_mesh_node > m_nodes;
			core::bc_unordered_map< hash_t::result_type, bc_mesh_node* > m_nodes_map;
			core::bc_vector_movable< core::bc_matrix4f > m_transformations;
			core::bc_vector_movable< core::bc_matrix4f > m_bone_offsets;
			core::bc_vector_movable< core::bc_matrix4f > m_inverse_bind_poses;
			core::bc_vector< bc_mesh_part_data > m_meshes;
			core::bc_vector< bc_render_state_ptr > m_render_states;
			bcFLOAT m_scale;
			bc_mesh_collider_ptr m_colliders;
			core::bc_vector< const bc_mesh_part_collider* > m_colliders_map;
		};

		using bc_mesh_ptr = core::bc_content_ptr< bc_mesh >;

		inline const core::bc_string& bc_mesh::get_name() const noexcept
		{
			return m_name;
		}

		inline const bc_mesh_node* bc_mesh::get_root() const noexcept
		{
			return m_root;
		}

		inline bcFLOAT bc_mesh::get_scale() const noexcept
		{
			return m_scale;
		}

		inline const core::bc_vector_movable<core::bc_matrix4f>& bc_mesh::get_bone_offsets() const noexcept
		{
			return m_bone_offsets;
		}
		
		inline const core::bc_vector_movable< core::bc_matrix4f >& bc_mesh::get_inverse_bind_poses() const noexcept
		{
			return m_inverse_bind_poses;
		}

		template< typename TArg, typename TCallable >
		void bc_mesh::iterate_over_nodes(TArg& p_arg, TCallable p_callable, const bc_mesh_node* p_node) const noexcept
		{
			const bc_mesh_node* l_root_pointer = p_node ? p_node : get_root();
			_iterate_over_nodes(p_arg, p_callable, &l_root_pointer, &l_root_pointer + 1);
		}

		template< typename TArg, typename TCallable >
		void bc_mesh::_iterate_over_nodes(TArg& p_arg, TCallable& p_callable, const bc_mesh_node* const* p_begin, const bc_mesh_node* const* p_end) const noexcept
		{
			for (; p_begin != p_end; ++p_begin)
			{
				const bc_mesh_node* l_node = *p_begin;
				TArg l_arg = p_callable(*l_node, p_arg);

				const auto& l_node_children = get_node_children(*l_node);
				if (!l_node_children.empty())
				{
					const bc_mesh_node* const* l_begin = &l_node_children.front();
					const bc_mesh_node* const* l_end = &l_node_children.back() + 1;

					_iterate_over_nodes
					(
						l_arg,
						p_callable,
						l_begin,
						l_end
					);
				}
			}
		}
	}
}