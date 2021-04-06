// [02/14/2021 MRB]

#pragma once

#include "Game/Object/Mesh/bcMeshPartCollider.h"

namespace black_cat
{
	namespace game
	{
		struct bc_px_shape_data
		{
			const bcCHAR* m_material_name;
			bc_mesh_part_collider_entry* m_collider_entry;
			const bcCHAR* m_collision_particle;
			const bcCHAR* m_collision_decal;
		};
	}	
}