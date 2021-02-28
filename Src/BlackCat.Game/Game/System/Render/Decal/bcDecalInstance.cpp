// [02/23/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Render/Decal/bcDecalInstance.h"
#include "Game/System/Render/Decal/bcDecalManager.h"

namespace black_cat
{
	namespace game
	{
		void bc_decal_instance_deleter::operator()(bc_decal_instance* p_ptr) const
		{
			m_manager->destroy_decal_instance(p_ptr);
		}
	}	
}