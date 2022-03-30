// [02/09/2017 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Mesh/bcMeshCollider.h"

namespace black_cat
{
	namespace game
	{
		bc_mesh_collider::bc_mesh_collider()
			: bci_content()
		{
		}

		bc_mesh_collider::bc_mesh_collider(bc_mesh_collider&& p_other) noexcept = default;

		bc_mesh_collider& bc_mesh_collider::operator=(bc_mesh_collider&& p_other) noexcept = default;

		bc_mesh_part_collider_entry* bc_mesh_collider::find_collider_by_name(core::bc_string_view p_name) const noexcept
		{
			const auto l_ite = std::find_if
			(
				std::begin(m_colliders),
				std::end(m_colliders),
				[=](const bc_mesh_part_collider_entry& p_entry)
				{
					return p_entry.m_name == p_name;
				}
			);

			if(l_ite != std::end(m_colliders))
			{
				return &*l_ite;
			}

			return nullptr;
		}

		core::bc_const_span<bc_mesh_part_collider_entry> bc_mesh_collider::find_mesh_node_colliders(core::bc_string_view p_node_name) const noexcept
		{
			const auto l_ite = std::find_if
			(
				std::begin(m_mesh_colliders),
				std::end(m_mesh_colliders),
				[=](const decltype(m_mesh_colliders)::value_type& p_entry)
				{
					auto& l_collider_name = std::get<core::bc_string>(p_entry);
					return core::bc_string_view(l_collider_name.c_str(), l_collider_name.size()) == p_node_name;
				}
			);

			if(l_ite == std::end(m_mesh_colliders))
			{
				return {};
			}

			const auto l_colliders_start = std::get<1>(*l_ite);
			const auto l_colliders_length = std::get<2>(*l_ite);
			return { &m_colliders[l_colliders_start], l_colliders_length };
		}

		const physics::bc_transform* bc_mesh_collider::find_joint(core::bc_string_view p_collider1, core::bc_string_view p_collider2) const noexcept
		{
			const auto l_collider1_hash = bc_mesh_part_collider_joint_entry::hash_t()(p_collider1.data());
			const auto l_collider2_hash = bc_mesh_part_collider_joint_entry::hash_t()(p_collider2.data());

			const auto l_ite = std::find_if
			(
				std::begin(m_joints), 
				std::end(m_joints), 
				[=](const bc_mesh_part_collider_joint_entry& p_joint)
				{
					return	(p_joint.m_collider1 == l_collider1_hash || p_joint.m_collider1 == l_collider2_hash) &&
							(p_joint.m_collider2 == l_collider1_hash || p_joint.m_collider2 == l_collider2_hash);
				}
			);

			if(l_ite == std::end(m_joints))
			{
				return nullptr;
			}

			return &l_ite->m_transform;
		}
	}
}