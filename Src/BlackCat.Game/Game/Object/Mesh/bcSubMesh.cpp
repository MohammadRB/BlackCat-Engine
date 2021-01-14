// [02/09/2017 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Physics/bcPhysicsShapeUtility.h"
#include "Game/Object/Mesh/bcSubMesh.h"

namespace black_cat
{
	namespace game
	{
		bc_sub_mesh::bc_sub_mesh() noexcept
			: m_mesh(nullptr),
			m_root_node(nullptr)
		{
		}

		bc_sub_mesh::bc_sub_mesh(bc_mesh_ptr p_mesh) noexcept
		{
			BC_ASSERT(p_mesh != nullptr);
			
			m_mesh = p_mesh;
			m_root_node = p_mesh->get_root();
		}

		bc_sub_mesh::bc_sub_mesh(bc_mesh_ptr p_mesh, const bcCHAR* p_node)
		{
			BC_ASSERT(p_mesh != nullptr);
			
			m_mesh = p_mesh;
			m_root_node = p_mesh->find_node(p_node);

			if (m_root_node == nullptr)
			{
				throw bc_invalid_argument_exception("null argument");
			}
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
}