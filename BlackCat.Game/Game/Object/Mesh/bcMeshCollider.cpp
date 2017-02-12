// [02/09/2017 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Mesh/bcMeshCollider.h"

namespace black_cat
{
	namespace game
	{
		void bc_mesh_collider::add_mesh_colliders(const bcCHAR* p_mesh_name, bc_mesh_part_collider&& p_colliders)
		{
			m_mesh_colliders.insert(colliders_container_t::value_type(p_mesh_name, std::move(p_colliders)));
		}

		const bc_mesh_part_collider* bc_mesh_collider::find_mesh_colliders(const bcCHAR* p_mesh_name)
		{
			colliders_container_t::key_type l_key(p_mesh_name);

			return find_mesh_colliders(l_key);
		}

		const bc_mesh_part_collider* bc_mesh_collider::find_mesh_colliders(const core::bc_string& p_mesh_name)
		{
			auto l_ite = m_mesh_colliders.find(p_mesh_name);
			bc_mesh_part_collider* l_result = nullptr;

			if (l_ite != std::cend(m_mesh_colliders))
			{
				l_result = &l_ite->second;
			}

			return l_result;
		}

		bc_mesh_collider::colliders_container_t::const_iterator bc_mesh_collider::cbegin() const
		{
			return m_mesh_colliders.cbegin();
		}

		bc_mesh_collider::colliders_container_t::const_iterator bc_mesh_collider::cend() const
		{
			return m_mesh_colliders.cend();
		}

		bc_mesh_collider::colliders_container_t::size_type bc_mesh_collider::size() const
		{
			return m_mesh_colliders.size();
		}
	}
}