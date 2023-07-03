// [14/02/2021 MRB]

#pragma once

#include "Game/Object/Mesh/bcMeshCollider.h"

namespace black_cat
{
	namespace game
	{
		struct bc_px_shape_data
		{
			const bc_mesh_part_collider_entry* m_collider_entry;
			const bcCHAR* m_material_name;
			const bcCHAR* m_collision_particle;
			const bcCHAR* m_collision_decal;
		};
	}	
}