// [02/09/2017 MRB]

#pragma once

#include "Core/Container/bcString.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/File/bcContent.h"
#include "Game/bcExport.h"
#include "Game/Object/Mesh/bcMeshPartCollider.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_mesh_collider : public core::bc_icontent
		{
			BC_CONTENT(mesh_physics)

		private:
			using colliders_container_t = core::bc_unordered_map<core::bc_string, bc_mesh_part_collider>;

		public:
			using const_iterator = colliders_container_t::const_iterator;
			using size_t = colliders_container_t::size_type;

		public:
			bc_mesh_collider() = default;

			bc_mesh_collider(bc_mesh_collider&&) = default;

			~bc_mesh_collider() = default;

			bc_mesh_collider& operator=(bc_mesh_collider&&) = default;

			void add_mesh_colliders(const bcCHAR* p_mesh_name, bc_mesh_part_collider&& p_colliders);

			const bc_mesh_part_collider* find_mesh_colliders(const bcCHAR* p_mesh_name);

			const bc_mesh_part_collider* find_mesh_colliders(const core::bc_string& p_mesh_name);

			const_iterator cbegin() const;

			const_iterator cend() const;

			size_t size() const;

		protected:

		private:
			colliders_container_t m_mesh_colliders;
		};

		using bc_mesh_collider_ptr = core::bc_content_ptr< bc_mesh_collider >;
	}
}