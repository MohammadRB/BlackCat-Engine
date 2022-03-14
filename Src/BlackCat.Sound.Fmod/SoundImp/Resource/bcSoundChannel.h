// [03/10/2022 MRB]

#pragma once

#include "Sound/Resource/bcSoundChannel.h"
#include "3rdParty/FMOD/Include/fmod.hpp"

namespace black_cat
{
	namespace sound
	{
		template<>
		struct bc_platform_sound_channel_pack<bc_sound_api::fmod>
		{
			FMOD::Channel* m_channel{ nullptr };
		};
	}
}
