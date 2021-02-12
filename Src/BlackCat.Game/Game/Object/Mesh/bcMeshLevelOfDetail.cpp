// [02/06/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Mesh/bcMeshLevelOfDetail.h"

namespace black_cat
{
	namespace game
	{
		bcFLOAT bc_mesh_level_of_detail::s_lod_factor_multiplier = 4.0f;
		bcUINT32 bc_mesh_level_of_detail::s_lod_culling_index = 6U;
	}	
}