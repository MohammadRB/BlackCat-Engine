// [10/03/2022 MRB]

#pragma once

#include "SoundImp/bcSoundRef.h"
#include "SoundImp/bcSoundReference.h"

#include "Sound/Resource/bcSound.h"

namespace black_cat
{
	namespace sound
	{
		template<>
		struct bc_platform_sound_pack<bc_sound_api::fmod> : bc_platform_sound_reference_pack<bc_sound_api::fmod>
		{
		};
	}
}