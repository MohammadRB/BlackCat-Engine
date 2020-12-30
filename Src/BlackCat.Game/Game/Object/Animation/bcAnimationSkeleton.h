// [12/29/2020 MRB]

#pragma once

#include "Core/Container/bcVector.h"

#include "3rdParty/Ozz/Include/ozz/animation/runtime/skeleton.h"
#include "3rdParty/Ozz/Include/ozz/animation/runtime/animation.h"

namespace black_Cat
{
	namespace game
	{
		class bc_animation_skeleton
		{
		public:
			ozz::animation::Skeleton m_skeleton;
		};
	}
}