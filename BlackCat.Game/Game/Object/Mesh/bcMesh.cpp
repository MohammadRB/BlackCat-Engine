// [06/14/2016 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Mesh/bcMesh.h"

namespace black_cat
{
	namespace game
	{
		// -- bc_mesh_node --------------------------------------------------------------------------------

		bc_mesh_node::bc_mesh_node(core::bc_string p_name, node_indexing p_data_index, bc_mesh_node* p_parent, const bc_render_state_ptr& p_render_state)
			: m_name(std::move(p_name)),
			m_data_index(p_data_index),
			m_parent(p_parent),
			m_render_state(p_render_state)
		{
		}

		bc_mesh_node::node_indexing bc_mesh_node::get_data_idnex() const
		{
			return m_data_index;
		}

		const core::bc_string& bc_mesh_node::get_name() const
		{
			return m_name;
		}

		void bc_mesh_node::_add_child(bc_mesh_node* p_mesh_part)
		{
			m_childs.push_back(p_mesh_part);
		}

		// -- bc_mesh --------------------------------------------------------------------------------

		bc_mesh::bc_mesh(core::bc_string p_name, bcSIZE p_node_count)
			: m_name(p_name)
		{
			// Reserve needed memory for nodes because we use raw pointers in bc_mesh_node for parent and childs
			m_nodes.reserve(p_node_count);
			m_nodes_hash.reserve(p_node_count);
			m_transformations.reserve(p_node_count);
			m_materials.reserve(p_node_count);
			m_vertices.reserve(p_node_count);
			m_indices.reserve(p_node_count);
		}

		bc_index_type bc_mesh::index_layout() const
		{
			return m_index_layout;
		}

		const core::bc_string& bc_mesh::get_name() const
		{
			return m_name;
		}

		const bc_mesh_node* bc_mesh::get_root() const
		{
			return m_root;
		}

		const bc_mesh_node* bc_mesh::find_node(const bcCHAR* p_name) const
		{
			auto l_hash = hash_t()(p_name);
			auto l_entry = m_nodes_hash.find(l_hash);

			if(l_entry == std::end(m_nodes_hash))
			{
				return nullptr;
			}

			return l_entry->second;
		}

		const bc_mesh_node* bc_mesh::get_parent_node(const bc_mesh_node* p_node) const
		{
			return p_node->m_parent;
		}

		const graphic::bc_matrix4f* bc_mesh::get_node_transformation(const bc_mesh_node* p_node) const
		{
			return &m_transformations.at(p_node->m_data_index);
		}

		const bc_mesh_node_material* bc_mesh::get_node_material(const bc_mesh_node* p_node) const
		{
			return &m_materials.at(p_node->m_data_index);
		}

		const bc_render_state_ptr& bc_mesh::get_node_render_state(const bc_mesh_node* p_node) const
		{
			return p_node->m_render_state;
		}

		const core::bc_vector<bc_mesh_node*>& bc_mesh::get_node_childs(const bc_mesh_node* p_node) const
		{
			return p_node->m_childs;
		}

		const core::bc_vector_movale<bc_vertex_pos_tex_nor_tan>& bc_mesh::get_node_vertices(const bc_mesh_node* p_node) const
		{
			return m_vertices.at(p_node->m_data_index);
		}

		const core::bc_vector_movale<bcBYTE>& bc_mesh::get_node_indices(const bc_mesh_node* p_node) const
		{
			return m_indices.at(p_node->m_data_index);
		}

		bc_mesh_node* bc_mesh::_add_node(core::bc_string p_name, 
			bc_mesh_node* p_parent, 
			graphic::bc_matrix4f& p_transformation, 
			bc_mesh_node_material& p_material, 
			const bc_render_state_ptr& p_render_state,
			core::bc_vector_frame<bc_vertex_pos_tex_nor_tan> p_vertices, 
			core::bc_vector_frame<bcBYTE> p_indices)
		{
			if(!p_parent)
			{
				m_nodes.clear();
				m_transformations.clear();
				m_materials.clear();
				m_vertices.clear();
				m_indices.clear();
			}

			auto l_data_index = m_nodes.size();
			auto l_node_hash = hash_t()(p_name.c_str());

			m_nodes.push_back(bc_mesh_node(p_name, l_data_index, p_parent, p_render_state));
			m_nodes_hash.insert(std::make_pair(l_node_hash, &*m_nodes.rbegin()));
			m_transformations.push_back(p_transformation);
			m_materials.push_back(std::move(p_material));
			m_vertices.push_back(core::bc_vector_movale<bc_vertex_pos_tex_nor_tan>(std::begin(p_vertices), std::end(p_vertices)));
			m_indices.push_back(core::bc_vector_movale<bcBYTE>(std::begin(p_indices), std::end(p_indices)));

			auto l_node = &(*m_nodes.rbegin());

			if(!p_parent)
			{
				m_root = l_node;
			}
			else
			{
				p_parent->_add_child(l_node);
			}

			return l_node;
		}
	}
}