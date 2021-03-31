// [03/27/2021 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Game/Object/Mesh/bcMeshNode.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/Component/bcWeaponComponent.h"

namespace black_cat
{
	namespace game
	{
		struct bc_xbot_weapon
		{
			bc_actor m_actor;
			bc_weapon_class m_class;
			const bc_mesh_node* m_main_hand_node;
			const bc_mesh_node* m_second_hand_node;
			core::bc_vector3f m_main_hand_offset;
			core::bc_vector3f m_second_hand_offset;
			core::bc_vector3f m_local_up;
			core::bc_vector3f m_local_forward;
		};
	}
}