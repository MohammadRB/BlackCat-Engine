// [09/02/2017 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Mesh/bcSubMesh.h"

namespace black_cat::game
{
	bc_sub_mesh::bc_sub_mesh() noexcept
		: m_mesh(nullptr),
		  m_root_node(nullptr)
	{
	}

	bc_sub_mesh::bc_sub_mesh(bc_mesh_ptr p_mesh) noexcept
	{
		BC_ASSERT(p_mesh != nullptr);
			
		m_mesh = std::move(p_mesh);
		m_root_node = m_mesh->get_root();
	}

	bc_sub_mesh::bc_sub_mesh(bc_mesh_ptr p_mesh, bc_mesh_node::node_index_t p_node_index)
	{
		BC_ASSERT(p_mesh != nullptr);

		m_mesh = std::move(p_mesh);
		m_root_node = m_mesh->find_node(p_node_index);

		if (m_root_node == nullptr)
		{
			throw bc_invalid_argument_exception("invalid node index argument");
		}
	}

	bc_sub_mesh::bc_sub_mesh(bc_mesh_ptr p_mesh, core::bc_string_view p_node_name)
		: bc_sub_mesh(std::move(p_mesh), p_mesh->find_node(p_node_name.data())->get_index())
	{
	}

	bc_sub_mesh::bc_sub_mesh(bc_sub_mesh&& p_other) noexcept
		: m_mesh(std::move(p_other.m_mesh)),
		  m_root_node(p_other.m_root_node)
	{
	}

	bc_sub_mesh::~bc_sub_mesh()
	{
	}

	bc_sub_mesh& bc_sub_mesh::operator=(bc_sub_mesh&& p_other) noexcept
	{
		m_mesh = std::move(p_other.m_mesh);
		m_root_node = p_other.m_root_node;

		return *this;
	}
}
