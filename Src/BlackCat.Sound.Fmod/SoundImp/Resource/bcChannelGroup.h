// [03/15/2022 MRB]

#pragma once

#include "SoundImp/bcSoundReference.h"
#include "SoundImp/bcSoundRef.h"

#include "Sound/Resource/bcChannelGroup.h"
#include "3rdParty/FMOD/Include/fmod.hpp"

namespace black_cat
{
	namespace sound
	{
		template<>
		struct bc_platform_channel_group_pack<bc_sound_api::fmod> : bc_platform_sound_reference_pack<bc_sound_api::fmod>
		{
		};
	}
}
