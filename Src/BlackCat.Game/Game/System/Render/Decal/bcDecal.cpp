// [21/02/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Render/Decal/bcDecal.h"
#include "Game/System/Render/Decal/bcDecalManager.h"

namespace black_cat::game
{
	void bc_decal_deleter::operator()(bc_decal* p_ptr) const
	{
		m_manager->destroy_decal(p_ptr);
	}
}
