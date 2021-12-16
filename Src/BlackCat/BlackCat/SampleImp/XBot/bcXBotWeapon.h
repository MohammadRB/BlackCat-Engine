// [03/27/2021 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/Component/bcWeaponComponent.h"

namespace black_cat
{
	struct bc_xbot_weapon
	{
		game::bc_actor m_actor;
		game::bc_weapon_component* m_component;
		game::bc_weapon_class m_class;
		core::bc_vector3f m_local_up;
		core::bc_vector3f m_local_forward;
		core::bc_vector3f m_main_hand_offset;
		core::bc_vector3f m_second_hand_offset;
		core::bc_vector3f m_fire_offset;
		bcFLOAT m_rate_of_fire_seconds;
		bcFLOAT m_mass;
	};

	struct bc_xbot_grenade
	{
		const bcCHAR* m_entity_name;
		game::bc_actor m_actor;
		core::bc_vector3f m_local_up = core::bc_vector3f::up();
		core::bc_vector3f m_local_forward = core::bc_vector3f::forward();
	};
}