// [15/03/2022 MRB]

#include "SoundImp/SoundImpPCH.h"
#include "SoundImp/Resource/bcChannelGroup.h"
#include "SoundImp/Fundation/bcFmodError.h"
#include "SoundImp/bcExport.h"

namespace black_cat::sound
{
	template<>
	BC_SOUNDIMP_DLL
	bc_platform_channel_group<bc_sound_api::fmod>::bc_platform_channel_group() noexcept
		: bc_platform_sound_reference<bc_sound_api::fmod>()
	{
	}

	template<>
	BC_SOUNDIMP_DLL
	bc_platform_channel_group<bc_sound_api::fmod>::bc_platform_channel_group(platform_pack& p_pack) noexcept
		: bc_platform_sound_reference<bc_sound_api::fmod>(),
		  m_pack(p_pack)
	{
	}

	template<>
	BC_SOUNDIMP_DLL
	bc_platform_channel_group<bc_sound_api::fmod>::bc_platform_channel_group(const bc_platform_channel_group& p_other) noexcept
		: bc_platform_sound_reference<bc_sound_api::fmod>(p_other),
		  m_pack(p_other.m_pack)
	{
	}

	template<>
	BC_SOUNDIMP_DLL
	bc_platform_channel_group<bc_sound_api::fmod>::~bc_platform_channel_group() noexcept
	{
	}

	template<>
	BC_SOUNDIMP_DLL
	bc_platform_channel_group<bc_sound_api::fmod>& bc_platform_channel_group<bc_sound_api::fmod>::operator=(const bc_platform_channel_group& p_other) noexcept
	{
		bc_platform_sound_reference<bc_sound_api::fmod>::operator=(p_other);
		m_pack = p_other.m_pack;
		return *this;
	}

	template<>
	BC_SOUNDIMP_DLL
	bool bc_platform_channel_group<bc_sound_api::fmod>::is_playing() const noexcept
	{
		auto* l_fmod_channel = static_cast<FMOD::ChannelGroup*>(m_pack.m_pointer);

		bool l_is_playing;
		const auto l_fmod_result = bc_fmod_log(l_fmod_channel->isPlaying(&l_is_playing), FMOD_ERR_INVALID_HANDLE);

		if (l_fmod_result == FMOD_ERR_INVALID_HANDLE)
		{
			l_is_playing = false;
		}

		return l_is_playing;
	}

	template<>
	BC_SOUNDIMP_DLL
	void bc_platform_channel_group<bc_sound_api::fmod>::resume() noexcept
	{
		auto* l_fmod_channel = static_cast<FMOD::ChannelGroup*>(m_pack.m_pointer);

		bc_fmod_log(l_fmod_channel->setPaused(false));
	}

	template<>
	BC_SOUNDIMP_DLL
	void bc_platform_channel_group<bc_sound_api::fmod>::pause() noexcept
	{
		auto* l_fmod_channel = static_cast<FMOD::ChannelGroup*>(m_pack.m_pointer);

		bc_fmod_log(l_fmod_channel->setPaused(true));
	}

	template<>
	BC_SOUNDIMP_DLL
	void bc_platform_channel_group<bc_sound_api::fmod>::stop() const noexcept
	{
		auto* l_fmod_channel = static_cast<FMOD::ChannelGroup*>(m_pack.m_pointer);

		bc_fmod_log(l_fmod_channel->stop(), FMOD_ERR_CHANNEL_STOLEN);
	}

	template<>
	BC_SOUNDIMP_DLL
	bcFLOAT bc_platform_channel_group<bc_sound_api::fmod>::get_volume() const noexcept
	{
		auto* l_fmod_channel = static_cast<FMOD::ChannelGroup*>(m_pack.m_pointer);
		bcFLOAT l_volume;

		bc_fmod_log(l_fmod_channel->getVolume(&l_volume));

		return l_volume;
	}

	template<>
	BC_SOUNDIMP_DLL
	void bc_platform_channel_group<bc_sound_api::fmod>::set_volume(bcFLOAT p_volume) noexcept
	{
		auto* l_fmod_channel = static_cast<FMOD::ChannelGroup*>(m_pack.m_pointer);
		bc_fmod_log(l_fmod_channel->setVolume(p_volume));
	}

	template<>
	BC_SOUNDIMP_DLL
	bool bc_platform_channel_group<bc_sound_api::fmod>::get_mute() const noexcept
	{
		auto* l_fmod_channel = static_cast<FMOD::ChannelGroup*>(m_pack.m_pointer);
		bool l_mute;

		bc_fmod_log(l_fmod_channel->getMute(&l_mute));

		return l_mute;
	}

	template<>
	BC_SOUNDIMP_DLL
	void bc_platform_channel_group<bc_sound_api::fmod>::set_mute(bool p_value) noexcept
	{
		auto* l_fmod_channel = static_cast<FMOD::ChannelGroup*>(m_pack.m_pointer);
		bc_fmod_log(l_fmod_channel->setMute(p_value));
	}

	template<>
	BC_SOUNDIMP_DLL
	bcFLOAT bc_platform_channel_group<bc_sound_api::fmod>::get_pitch() const noexcept
	{
		auto* l_fmod_channel = static_cast<FMOD::ChannelGroup*>(m_pack.m_pointer);
		bcFLOAT l_pitch;

		bc_fmod_log(l_fmod_channel->getPitch(&l_pitch));

		return l_pitch;
	}

	template<>
	BC_SOUNDIMP_DLL
	void bc_platform_channel_group<bc_sound_api::fmod>::set_pitch(bcFLOAT p_pitch) const noexcept
	{
		auto* l_fmod_channel = static_cast<FMOD::ChannelGroup*>(m_pack.m_pointer);

		bc_fmod_log(l_fmod_channel->setPitch(p_pitch));
	}

	template<>
	BC_SOUNDIMP_DLL
	bc_sound_mode bc_platform_channel_group<bc_sound_api::fmod>::get_mode() const noexcept
	{
		auto* l_fmod_channel = static_cast<FMOD::ChannelGroup*>(m_pack.m_pointer);
		auto l_fmod_mod = FMOD_MODE();

		bc_fmod_log(l_fmod_channel->getMode(&l_fmod_mod));

		auto l_mode = bc_sound_mode::none;

		if (l_fmod_mod & FMOD_LOOP_OFF)
		{
			l_mode = core::bc_enum::set(l_mode, bc_sound_mode::loop_off, true);
		}
		if (l_fmod_mod & FMOD_LOOP_NORMAL)
		{
			l_mode = core::bc_enum::set(l_mode, bc_sound_mode::loop, true);
		}
		if (l_fmod_mod & FMOD_3D)
		{
			l_mode = core::bc_enum::set(l_mode, bc_sound_mode::d3, true);
		}

		return l_mode;
	}

	template<>
	BC_SOUNDIMP_DLL
	void bc_platform_channel_group<bc_sound_api::fmod>::set_mode(bc_sound_mode p_mode) noexcept
	{
		auto* l_fmod_channel = static_cast<FMOD::ChannelGroup*>(m_pack.m_pointer);

		if (core::bc_enum::has(p_mode, bc_sound_mode::loop_off))
		{
			bc_fmod_log(l_fmod_channel->setMode(FMOD_LOOP_OFF));
		}
		if (core::bc_enum::has(p_mode, bc_sound_mode::loop))
		{
			bc_fmod_log(l_fmod_channel->setMode(FMOD_LOOP_NORMAL));
		}
		if (core::bc_enum::has(p_mode, bc_sound_mode::d3))
		{
			bc_fmod_log(l_fmod_channel->setMode(FMOD_3D));
		}
	}

	template<>
	BC_SOUNDIMP_DLL
	core::bc_vector3f bc_platform_channel_group<bc_sound_api::fmod>::get_3d_position() const noexcept
	{
		auto* l_fmod_channel = static_cast<FMOD::ChannelGroup*>(m_pack.m_pointer);
		FMOD_VECTOR l_position;

		bc_fmod_log(l_fmod_channel->get3DAttributes(&l_position, nullptr));

		return { l_position.x, l_position.y, l_position.z };
	}

	template<>
	BC_SOUNDIMP_DLL
	void bc_platform_channel_group<bc_sound_api::fmod>::set_3d_position(const core::bc_vector3f& p_position) noexcept
	{
		auto* l_fmod_channel = static_cast<FMOD::ChannelGroup*>(m_pack.m_pointer);
		const FMOD_VECTOR l_position{ p_position.x, p_position.y, p_position.z };

		bc_fmod_log(l_fmod_channel->set3DAttributes(&l_position, nullptr));
	}

	template<>
	BC_SOUNDIMP_DLL
	std::tuple<bcFLOAT, bcFLOAT> bc_platform_channel_group<bc_sound_api::fmod>::get_3d_min_max_distance() const noexcept
	{
		auto* l_fmod_channel = static_cast<FMOD::ChannelGroup*>(m_pack.m_pointer);
		bcFLOAT l_min, l_max;

		bc_fmod_log(l_fmod_channel->get3DMinMaxDistance(&l_min, &l_max));

		return { l_min, l_max };
	}

	template<>
	BC_SOUNDIMP_DLL
	void bc_platform_channel_group<bc_sound_api::fmod>::set_3d_min_max_distance(bcFLOAT p_min, bcFLOAT p_max) noexcept
	{
		auto* l_fmod_channel = static_cast<FMOD::ChannelGroup*>(m_pack.m_pointer);

		bc_fmod_log(l_fmod_channel->set3DMinMaxDistance(p_min, p_max));
	}

	template<>
	BC_SOUNDIMP_DLL
	void* bc_platform_channel_group<bc_sound_api::fmod>::get_user_data() const noexcept
	{
		auto* l_fmod_channel = static_cast<FMOD::ChannelGroup*>(m_pack.m_pointer);
		void* l_data;

		bc_fmod_log(l_fmod_channel->getUserData(&l_data));

		return l_data;
	}

	template<>
	BC_SOUNDIMP_DLL
	void bc_platform_channel_group<bc_sound_api::fmod>::set_user_data(void* p_data) noexcept
	{
		auto* l_fmod_channel = static_cast<FMOD::ChannelGroup*>(m_pack.m_pointer);

		bc_fmod_log(l_fmod_channel->setUserData(p_data));
	}
}
