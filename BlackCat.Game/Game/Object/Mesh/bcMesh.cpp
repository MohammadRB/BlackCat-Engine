// [06/14/2016 MRB]

#include "Game/GamePCH.h"
#include "Game/bcException.h"
#include "Game/Object/Mesh/bcMesh.h"

namespace black_cat
{
	namespace game
	{
		// -- bc_mesh_node --------------------------------------------------------------------------------

		bc_mesh_node::bc_mesh_node(core::bc_string p_name,
			bc_mesh_node* p_parent,
			node_indexing p_transformation,
			node_indexing p_first_mesh,
			bcUINT32 p_mesh_count)
			: m_name(std::move(p_name)),
			m_parent(p_parent),
			m_transformation_index(p_transformation),
			m_first_mesh_index(p_first_mesh),
			m_mesh_count(p_mesh_count)
		{
		}

		bc_mesh_node::node_indexing bc_mesh_node::get_transformation_index() const
		{
			return m_transformation_index;
		}

		bc_mesh_node::node_indexing bc_mesh_node::get_mesh_count() const
		{
			return m_mesh_count;
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

		bc_mesh::bc_mesh(core::bc_string p_name, bcSIZE p_node_count, bcSIZE p_mesh_count)
			: m_name(p_name)
		{
			// Reserve needed memory for nodes because we use raw pointers in bc_mesh_node for parent and childs
			m_nodes.reserve(p_node_count);
			m_nodes_hash.reserve(p_node_count);
			m_transformations.reserve(p_node_count);
			m_meshes.reserve(p_mesh_count);
			m_render_states.reserve(p_mesh_count);
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

		const bc_mesh_node* bc_mesh::get_node_parent(const bc_mesh_node* p_node) const
		{
			return p_node->m_parent;
		}

		const core::bc_vector<bc_mesh_node*>& bc_mesh::get_node_childs(const bc_mesh_node* p_node) const
		{
			return p_node->m_childs;
		}

		const graphic::bc_matrix4f* bc_mesh::get_node_transformation(const bc_mesh_node* p_node) const
		{
			return &m_transformations.at(p_node->m_transformation_index);
		}

		const bc_mesh_material* bc_mesh::get_node_mesh_material(const bc_mesh_node* p_node, bcUINT32 p_mesh_index) const
		{
			if(p_mesh_index >= p_node->m_mesh_count)
			{
				throw bc_out_of_range_exception("Invalid mesh index");
			}

			return &m_meshes[p_node->m_first_mesh_index + p_mesh_index].m_material;
		}

		const bc_render_state* bc_mesh::get_node_mesh_render_state(const bc_mesh_node* p_node, bcUINT32 p_mesh_index) const
		{
			if (p_mesh_index >= p_node->m_mesh_count)
			{
				throw bc_out_of_range_exception("Invalid mesh index");
			}

			return m_render_states[p_node->m_first_mesh_index + p_mesh_index].get();
		}

		bc_mesh_node* bc_mesh::_add_node(core::bc_string p_name,
			bc_mesh_node* p_parent,
			graphic::bc_matrix4f& p_transformation,
			core::bc_vector_frame<std::pair<bc_mesh_data, bc_render_state_ptr>> p_meshes)
		{
			if(!p_parent)
			{
				m_nodes.clear();
				m_transformations.clear();
				m_meshes.clear();
				m_render_states.clear();
			}

			auto l_data_index = m_transformations.size();
			auto l_node_hash = hash_t()(p_name.c_str());
			auto l_mesh_start_index = m_meshes.size();
			auto l_mesh_count = p_meshes.size();
			
			m_nodes.push_back(bc_mesh_node(p_name, p_parent, l_data_index, l_mesh_start_index, l_mesh_count));
			m_nodes_hash.insert(std::make_pair(l_node_hash, &*m_nodes.rbegin()));
			m_transformations.push_back(p_transformation);

			for (auto& l_mesh : p_meshes)
			{
				m_meshes.push_back(std::move(std::get< bc_mesh_data >(l_mesh)));
				m_render_states.push_back(std::get< bc_render_state_ptr >(l_mesh));
			}

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