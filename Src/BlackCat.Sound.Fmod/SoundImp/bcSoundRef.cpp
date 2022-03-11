// [03/09/2022 MRB]

#include "SoundImp/SoundImpPCH.h"
#include "SoundImp/bcSoundRef.h"
#include "SoundImp/Resource/bcSound.h"
#include "3rdParty/FMOD/Include/fmod.hpp"

namespace black_cat
{
	namespace sound
	{
		void _release_fmod_reference(bc_platform_sound<bc_sound_api::fmod>& p_reference)
		{
			auto* l_fmod_sound = static_cast<FMOD::Sound*>(p_reference.get_platform_pack().m_pointer);
			l_fmod_sound->release();
		}
	}
}
