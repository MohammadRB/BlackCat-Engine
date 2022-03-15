// [03/15/2022 MRB]

#pragma once

#include "CorePlatformImp/Utility/bcClock.h"
#include "SoundImp/Fundation/bcDevice.h"
#include "SoundImp/Resource/bcSound.h"
#include "SoundImp/Resource/bcChannel.h"
#include "SoundImp/Resource/bcChannelGroup.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_sound_manager
		{
		public:
			explicit bc_sound_manager(sound::bc_device& p_device) noexcept;

			bc_sound_manager(bc_sound_manager&&) noexcept = default;

			~bc_sound_manager() noexcept = default;

			bc_sound_manager& operator=(bc_sound_manager&&) noexcept = default;

			void resume() noexcept;

			void pause() noexcept;

			void stop() const noexcept;

			bcFLOAT get_volume() const noexcept;

			void set_volume(bcFLOAT p_volume) noexcept;

			sound::bc_channel play_sound(const sound::bc_sound& p_sound, bool p_paused) noexcept;

			/**
			 * \brief update sounds pitch based on elapsed time to slow down or speed up play speed
			 * \param p_clock 
			 */
			void update_pitch(const platform::bc_clock::update_param& p_clock) noexcept;

		private:
			sound::bc_device* m_device;
			sound::bc_channel_group_ref m_channel_group;
		};
	}
}
