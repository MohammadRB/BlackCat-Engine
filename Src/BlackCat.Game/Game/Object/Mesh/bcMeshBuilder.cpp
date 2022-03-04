// [01/07/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Mesh/bcMesh.h"
#include "Game/Object/Mesh/bcMeshBuilder.h"

namespace black_cat
{
	namespace game
	{
		bc_mesh_builder& bc_mesh_builder::add_node(const bcCHAR* p_parent_name,
			const bcCHAR* p_name,
			const core::bc_matrix4f& p_transform)
		{
			bc_mesh_builder_mesh_node l_node;
			l_node.m_name = p_name;
			l_node.m_parent_index = -1;
			l_node.m_transform = p_transform;
			l_node.m_bone_offset = core::bc_matrix4f::identity();

			m_nodes.push_back(std::move(l_node));
			m_node_count++;
			
			if(p_parent_name == nullptr)
			{
				return *this;
			}

			auto l_my_ite = m_nodes.rbegin().base();
			auto l_parent_ite = std::find_if(std::begin(m_nodes), std::end(m_nodes), [p_parent_name](bc_mesh_builder_mesh_node& p_node)
			{
				return std::strcmp(p_parent_name, p_node.m_name.c_str()) == 0;
			});

			BC_ASSERT(l_parent_ite != std::end(m_nodes));

			l_parent_ite->m_child_indices.push_back(std::distance(std::begin(m_nodes), l_my_ite));
			l_my_ite->m_parent_index = std::distance(std::begin(m_nodes), l_parent_ite);
			
			return *this;
		}

		bc_mesh_builder& bc_mesh_builder::add_bone(const bcCHAR* p_name, const core::bc_matrix4f& p_bone_offset)
		{
			const auto l_node_ite = std::find_if(std::begin(m_nodes), std::end(m_nodes), [p_name](bc_mesh_builder_mesh_node& p_node)
			{
				return std::strcmp(p_name, p_node.m_name.c_str()) == 0;
			});

			BC_ASSERT(l_node_ite != std::end(m_nodes));

			l_node_ite->m_bone_offset = p_bone_offset;
			
			return *this;
		}

		bc_mesh_builder& bc_mesh_builder::add_mesh_part(const bcCHAR* p_node_name,
			const bcCHAR* p_name,
			bc_mesh_material_ptr p_material,
			core::bc_vector_movable<bc_vertex_pos_tex_nor_tan> p_vertices,
			core::bc_vector_movable<bcUINT16> p_16bit_indices,
			core::bc_vector_movable<bcUINT32> p_32bit_indices,
			physics::bc_bound_box& p_bound_box,
			graphic::bc_buffer_ref p_vertex_buffer,
			graphic::bc_buffer_ref p_index_buffer,
			bc_render_state_ptr p_render_state)
		{
			auto l_node_ite = std::find_if(std::begin(m_nodes), std::end(m_nodes), [p_node_name](bc_mesh_builder_mesh_node& p_node)
			{
				return std::strcmp(p_node_name, p_node.m_name.c_str()) == 0;
			});

			BC_ASSERT(l_node_ite != std::end(m_nodes));

			bc_mesh_builder_mesh_part l_mesh_part;
			l_mesh_part.m_name = p_name;
			l_mesh_part.m_material = std::move(p_material);
			l_mesh_part.m_vertices = std::move(p_vertices);
			l_mesh_part.m_16bit_indices = std::move(p_16bit_indices);
			l_mesh_part.m_32bit_indices = std::move(p_32bit_indices);
			l_mesh_part.m_bound_box = p_bound_box;
			l_mesh_part.m_vertex_buffer = std::move(p_vertex_buffer);
			l_mesh_part.m_index_buffer = std::move(p_index_buffer);
			l_mesh_part.m_render_state = std::move(p_render_state);

			l_node_ite->m_mesh_parts.push_back(std::move(l_mesh_part));
			m_mesh_part_count++;
			
			return *this;
		}

		bc_mesh_builder& bc_mesh_builder::add_skinned_mesh_part(const bcCHAR* p_node_name,
			const bcCHAR* p_name,
			bc_mesh_material_ptr p_material,
			core::bc_vector_movable<bc_vertex_pos_tex_nor_tan_bon> p_vertices,
			core::bc_vector_movable<bcUINT16> p_16bit_indices,
			core::bc_vector_movable<bcUINT32> p_32bit_indices,
			physics::bc_bound_box& p_bound_box,
			graphic::bc_buffer_ref p_vertex_buffer,
			graphic::bc_buffer_ref p_index_buffer,
			bc_render_state_ptr p_render_state)
		{
			auto l_node_ite = std::find_if(std::begin(m_nodes), std::end(m_nodes), [p_node_name](bc_mesh_builder_mesh_node& p_node)
			{
				return std::strcmp(p_node_name, p_node.m_name.c_str()) == 0;
			});

			BC_ASSERT(l_node_ite != std::end(m_nodes));

			bc_mesh_builder_mesh_part l_mesh_part;
			l_mesh_part.m_name = p_name;
			l_mesh_part.m_material = std::move(p_material);
			l_mesh_part.m_skinned_vertices = std::move(p_vertices);
			l_mesh_part.m_16bit_indices = std::move(p_16bit_indices);
			l_mesh_part.m_32bit_indices = std::move(p_32bit_indices);
			l_mesh_part.m_bound_box = p_bound_box;
			l_mesh_part.m_vertex_buffer = std::move(p_vertex_buffer);
			l_mesh_part.m_index_buffer = std::move(p_index_buffer);
			l_mesh_part.m_render_state = std::move(p_render_state);

			l_node_ite->m_mesh_parts.push_back(std::move(l_mesh_part));
			m_mesh_part_count++;
			m_skinned = true;
			
			return *this;
		}

		bc_mesh_builder& bc_mesh_builder::with_auto_scale(bcFLOAT p_scale)
		{
			m_auto_scale = p_scale;
			return *this;
		}

		bc_mesh_builder& bc_mesh_builder::with_lod(bc_mesh_ptr p_mesh)
		{
			m_level_of_details.push_back(std::move(p_mesh));
			return *this;
		}
		
		bc_mesh bc_mesh_builder::build(core::bc_estring_view p_name, bc_mesh_collider_ptr p_collider)
		{
			m_name = p_name;
			return bc_mesh(std::move(*this), std::move(p_collider));
		}
	}	
}