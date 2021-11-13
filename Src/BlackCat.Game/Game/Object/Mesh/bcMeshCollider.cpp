// [02/09/2017 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Mesh/bcMeshCollider.h"

namespace black_cat
{
	namespace game
	{
		bc_mesh_collider::bc_mesh_collider()
			: bc_const_iterator_adapter(m_mesh_colliders)
		{
		}

		bc_mesh_collider::bc_mesh_collider(bc_mesh_collider&& p_other) noexcept
			: bc_const_iterator_adapter(m_mesh_colliders),
			bci_content(std::move(p_other)),
			m_mesh_colliders(std::move(p_other.m_mesh_colliders)),
			m_skinned_collider(std::move(p_other.m_skinned_collider))
		{
		}

		bc_mesh_collider& bc_mesh_collider::operator=(bc_mesh_collider&& p_other) noexcept
		{
			bci_content::operator=(std::move(p_other));
			m_mesh_colliders = std::move(p_other.m_mesh_colliders);
			m_skinned_collider = std::move(p_other.m_skinned_collider);
			return *this;
		}

		const bc_mesh_part_collider* bc_mesh_collider::find_mesh_collider(const bcCHAR* p_mesh_name) const noexcept
		{
			const container_type::key_type l_key(p_mesh_name);

			return find_mesh_collider(l_key);
		}

		const bc_mesh_part_collider* bc_mesh_collider::find_mesh_collider(const core::bc_string& p_mesh_name) const noexcept
		{
			const auto l_ite = m_mesh_colliders.find(p_mesh_name);
			const bc_mesh_part_collider* l_result = nullptr;

			if (l_ite != std::cend(m_mesh_colliders))
			{
				l_result = &l_ite->second;
			}

			return l_result;
		}

		void bc_mesh_collider::add_mesh_collider(const bcCHAR* p_name, bc_mesh_part_collider&& p_collider)
		{
			m_mesh_colliders.insert(container_type::value_type(p_name, std::move(p_collider)));
		}
	}
}