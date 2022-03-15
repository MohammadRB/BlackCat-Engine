// [03/09/2022 MRB]

#include "SoundImp/SoundImpPCH.h"
#include "SoundImp/bcSoundRef.h"
#include "SoundImp/Resource/bcSound.h"
#include "SoundImp/Resource/bcChannelGroup.h"
#include "3rdParty/FMOD/Include/fmod.hpp"

namespace black_cat
{
	namespace sound
	{
		void _release_fmod_reference(bc_platform_sound<bc_sound_api::fmod>& p_reference)
		{
			auto* l_sound = static_cast<FMOD::Sound*>(p_reference.get_platform_pack().m_pointer);
			l_sound->release();
		}

		void _release_fmod_reference(bc_platform_channel_group<bc_sound_api::fmod>& p_reference)
		{
			auto* l_channel_group = static_cast<FMOD::ChannelGroup*>(p_reference.get_platform_pack().m_pointer);
			l_channel_group->release();
		}
	}
}
