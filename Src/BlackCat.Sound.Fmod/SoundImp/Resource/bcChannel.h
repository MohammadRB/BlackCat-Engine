// [10/03/2022 MRB]

#pragma once

#include "Sound/Resource/bcChannel.h"
#include "3rdParty/FMOD/Include/fmod.hpp"

namespace black_cat::sound
{
	template<>
	struct bc_platform_channel_pack<bc_sound_api::fmod>
	{
		FMOD::Channel* m_channel{ nullptr };
	};
}
