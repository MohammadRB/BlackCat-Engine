// [02/03/2021 MRB]

#include "Game/GamePCH.h"

#include "Game/Object/Mesh/bcSkinnedMeshCollider.h"
#include "Game/bcException.h"

namespace black_cat
{
	namespace game
	{
		bc_skinned_mesh_collider::bc_skinned_mesh_collider()
			: m_faces_count(0)
		{	
		}

		bc_skinned_mesh_collider::bc_skinned_mesh_collider(bc_skinned_mesh_collider&& p_other) noexcept = default;

		bc_skinned_mesh_collider::~bc_skinned_mesh_collider() = default;

		bc_skinned_mesh_collider& bc_skinned_mesh_collider::operator=(bc_skinned_mesh_collider&&) noexcept = default;

		void bc_skinned_mesh_collider::add_mesh_collider(bc_skinned_mesh_collider_vertex* p_vertices,
			bcUINT16* p_indices,
			bcSIZE p_vertices_count,
			bcSIZE p_faces_count)
		{
			const auto l_vertices_count = m_vertices.size();
			const auto l_indices_count = m_16bit_indices.size();

			if(l_indices_count + p_faces_count * 3 > std::numeric_limits<bcUINT16>::max())
			{
				throw bc_invalid_operation_exception("Skinned collider indices count is higher than 16bit indices limit");
			}
			
			m_faces_count += p_faces_count;
			m_vertices.resize(m_vertices.size() + p_vertices_count);
			m_16bit_indices.resize(m_16bit_indices.size() + p_faces_count * 3);

			std::memcpy(m_vertices.data() + l_vertices_count, p_vertices, p_vertices_count * sizeof(bc_skinned_mesh_collider_vertex));
			std::memcpy(m_16bit_indices.data() + l_indices_count, p_indices, p_faces_count * 3 * sizeof(bcUINT16));

			for(auto l_ite = l_indices_count; l_ite < m_16bit_indices.size(); ++l_ite)
			{
				m_16bit_indices[l_ite] += l_vertices_count;
			}
		}

		void bc_skinned_mesh_collider::add_mesh_collider(bc_skinned_mesh_collider_vertex* p_vertices,
			bcUINT32* p_indices,
			bcSIZE p_vertices_count,
			bcSIZE p_faces_count)
		{
			if(!m_16bit_indices.empty())
			{
				throw bc_invalid_operation_exception("Skinned collider indices cannot contain 16bit and 32bit indices together");
			}
			
			const auto l_vertices_count = m_vertices.size();
			const auto l_indices_count = m_32bit_indices.size();

			m_faces_count += p_faces_count;
			m_vertices.resize(m_vertices.size() + p_vertices_count);
			m_32bit_indices.resize(m_32bit_indices.size() + p_faces_count * 3);

			std::memcpy(m_vertices.data() + l_vertices_count, p_vertices, p_vertices_count * sizeof(bc_skinned_mesh_collider_vertex));
			std::memcpy(m_32bit_indices.data() + l_indices_count, p_indices, p_faces_count * 3 * sizeof(bcUINT32));

			for (auto l_ite = l_indices_count; l_ite < m_32bit_indices.size(); ++l_ite)
			{
				m_32bit_indices[l_ite] += l_vertices_count;
			}
		}

		void bc_skinned_mesh_collider::shrink_to_fit()
		{
			m_vertices.shrink_to_fit();
			m_16bit_indices.shrink_to_fit();
			m_32bit_indices.shrink_to_fit();
		}
	}
}