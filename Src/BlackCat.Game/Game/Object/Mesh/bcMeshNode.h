// [01/05/2016 MRB]

#pragma once

#include "CorePlatform/Utility/bcNoCopy.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcVector.h"
#include "Game/bcExport.h"

namespace black_cat::game
{
	class bc_mesh;
		
	class BC_GAME_DLL bc_mesh_node : public platform::bc_no_copy
	{
	public:
		friend class bc_mesh;
		using node_index_t = bcSIZE;
		const static node_index_t s_invalid_index = static_cast<node_index_t>(-1);

	public:
		bc_mesh_node() = default;
			
		bc_mesh_node(bc_mesh_node&&) = default;

		~bc_mesh_node() = default;

		bc_mesh_node& operator=(bc_mesh_node&&) = default;

		core::bc_string_view get_name() const;
			
		node_index_t get_index() const;

		node_index_t get_mesh_count() const;

		bcSIZE get_children_count() const;

		bcSIZE get_all_children_count() const;

	private:
		core::bc_string m_name;
		bc_mesh_node* m_parent;
		core::bc_vector< bc_mesh_node* > m_children;
		node_index_t m_index;
		node_index_t m_first_mesh_index;
		bcSIZE m_mesh_count;
	};

	inline core::bc_string_view bc_mesh_node::get_name() const
	{
		return core::bc_string_view(m_name.c_str(), m_name.size());
	}
		
	inline bc_mesh_node::node_index_t bc_mesh_node::get_index() const
	{
		return m_index;
	}

	inline bc_mesh_node::node_index_t bc_mesh_node::get_mesh_count() const
	{
		return m_mesh_count;
	}

	inline bcSIZE bc_mesh_node::get_children_count() const
	{
		return m_children.size();
	}

	inline bcSIZE bc_mesh_node::get_all_children_count() const
	{
		auto l_children_count = m_children.size();

		for (auto* l_child : m_children)
		{
			l_children_count += l_child->get_all_children_count();
		}

		return l_children_count;
	}
}
