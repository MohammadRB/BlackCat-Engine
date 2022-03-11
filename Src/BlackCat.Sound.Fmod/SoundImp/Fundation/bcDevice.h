// [03/09/2022 MRB]

#pragma once

#include "SoundImp/bcSoundRef.h"

#include "Sound/Fundation/bcDevice.h"
#include "SoundImp/bcExport.h"
#include "3rdParty/FMOD/Include/fmod.hpp"

namespace black_cat
{
	namespace sound
	{
		template<>
		struct bc_platform_device_pack<bc_sound_api::fmod>
		{
			FMOD::System* m_system{ nullptr };
		};
	}
}
