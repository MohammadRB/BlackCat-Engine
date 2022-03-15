// [03/15/2022 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Sound/bcSoundManager.h"

namespace black_cat
{
	namespace game
	{
		bc_sound_manager::bc_sound_manager(sound::bc_device& p_device) noexcept
			: m_device(&p_device)
		{
			m_channel_group = p_device.create_channel_group("sound_manager");
		}

		void bc_sound_manager::resume() noexcept
		{
			m_channel_group->resume();
		}

		void bc_sound_manager::pause() noexcept
		{
			m_channel_group->pause();
		}

		void bc_sound_manager::stop() const noexcept
		{
			m_channel_group->stop();
		}

		bcFLOAT bc_sound_manager::get_volume() const noexcept
		{
			return m_channel_group->get_volume();
		}

		void bc_sound_manager::set_volume(bcFLOAT p_volume) noexcept
		{
			m_channel_group->set_volume(p_volume);
		}

		sound::bc_channel bc_sound_manager::play_sound(const sound::bc_sound& p_sound, bool p_paused) noexcept
		{
			return m_device->play_sound(p_sound, *m_channel_group, p_paused);
		}

		void bc_sound_manager::update_pitch(const platform::bc_clock::update_param& p_clock) noexcept
		{
		}
	}
}
