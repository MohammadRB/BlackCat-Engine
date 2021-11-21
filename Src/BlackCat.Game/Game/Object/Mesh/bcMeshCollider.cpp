// [02/09/2017 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Mesh/bcMeshCollider.h"

namespace black_cat
{
	namespace game
	{
		bc_mesh_collider::bc_mesh_collider()
			: bci_content(),
			bc_const_iterator_adapter(m_colliders)
		{
		}

		bc_mesh_collider::bc_mesh_collider(bc_mesh_collider&& p_other) noexcept
			: bci_content(std::move(p_other)),
			bc_const_iterator_adapter(m_colliders),
			m_mesh_colliders(std::move(p_other.m_mesh_colliders)),
			m_colliders(std::move(p_other.m_colliders)),
			m_convex_shapes(std::move(p_other.m_convex_shapes)),
			m_triangle_shapes(std::move(p_other.m_triangle_shapes)),
			m_skinned_collider(std::move(p_other.m_skinned_collider))
		{
		}

		bc_mesh_collider& bc_mesh_collider::operator=(bc_mesh_collider&& p_other) noexcept
		{
			bci_content::operator=(std::move(p_other));
			m_mesh_colliders = std::move(p_other.m_mesh_colliders);
			m_colliders = std::move(p_other.m_colliders);
			m_convex_shapes = std::move(p_other.m_convex_shapes);
			m_triangle_shapes = std::move(p_other.m_triangle_shapes);
			m_skinned_collider = std::move(p_other.m_skinned_collider);
			return *this;
		}

		core::bc_const_span<bc_mesh_part_collider_entry> bc_mesh_collider::find_mesh_collider(std::string_view p_mesh_name) const noexcept
		{
			const auto l_ite = std::find_if
			(
				std::begin(m_mesh_colliders),
				std::end(m_mesh_colliders),
				[=](const decltype(m_mesh_colliders)::value_type& p_entry)
				{
					auto& l_collider_name = std::get<core::bc_string>(p_entry);
					return std::string_view(l_collider_name.c_str(), l_collider_name.size()) == p_mesh_name;
				}
			);

			if(l_ite == std::end(m_mesh_colliders))
			{
				return {};
			}

			const auto l_colliders_start = std::get<1>(*l_ite);
			const auto l_colliders_length = std::get<2>(*l_ite);
			return core::bc_const_span<bc_mesh_part_collider_entry>(&m_colliders[l_colliders_start], l_colliders_length);
		}
	}
}