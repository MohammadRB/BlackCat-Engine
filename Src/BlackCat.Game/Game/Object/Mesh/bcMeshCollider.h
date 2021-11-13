// [02/09/2017 MRB]

#pragma once

#include "Core/Container/bcIteratorAdapter.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Content/bcContent.h"
#include "Game/bcExport.h"
#include "Game/Object/Mesh/bcMeshPartCollider.h"
#include "Game/Object/Mesh/bcSkinnedMeshCollider.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_mesh_collider : public core::bc_const_iterator_adapter<core::bc_unordered_map< core::bc_string, bc_mesh_part_collider>>,
			public core::bci_content
		{
			BC_CONTENT(msh_cld)

		public:
			bc_mesh_collider();

			bc_mesh_collider(bc_mesh_collider&& p_other) noexcept;

			~bc_mesh_collider() override = default;

			bc_mesh_collider& operator=(bc_mesh_collider&& p_other) noexcept;

			bc_skinned_mesh_collider& get_skinned_collider() noexcept;
			
			const bc_skinned_mesh_collider& get_skinned_collider() const noexcept;
			
			const bc_mesh_part_collider* find_mesh_collider(const bcCHAR* p_mesh_name) const noexcept;

			const bc_mesh_part_collider* find_mesh_collider(const core::bc_string& p_mesh_name) const noexcept;

			void add_mesh_collider(const bcCHAR* p_name, bc_mesh_part_collider&& p_collider);

		private:
			container_type m_mesh_colliders;
			bc_skinned_mesh_collider m_skinned_collider;
		};

		using bc_mesh_collider_ptr = core::bc_content_ptr<bc_mesh_collider>;

		inline bc_skinned_mesh_collider& bc_mesh_collider::get_skinned_collider() noexcept
		{
			return m_skinned_collider;
		}

		inline const bc_skinned_mesh_collider& bc_mesh_collider::get_skinned_collider() const noexcept
		{
			return m_skinned_collider;
		}
	}
}