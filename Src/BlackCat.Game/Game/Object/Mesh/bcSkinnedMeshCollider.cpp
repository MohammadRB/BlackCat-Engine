// [03/02/2021 MRB]

#include "Game/GamePCH.h"

#include "Game/Object/Mesh/bcSkinnedMeshCollider.h"
#include "Game/bcException.h"

namespace black_cat::game
{
	bc_skinned_mesh_collider::bc_skinned_mesh_collider()
		: m_faces_count(0)
	{	
	}

	bc_skinned_mesh_collider::bc_skinned_mesh_collider(bc_skinned_mesh_collider&& p_other) noexcept = default;

	bc_skinned_mesh_collider::~bc_skinned_mesh_collider() = default;

	bc_skinned_mesh_collider& bc_skinned_mesh_collider::operator=(bc_skinned_mesh_collider&&) noexcept = default;
}
