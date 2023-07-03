// [10/03/2022 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Sound/bcDefinition.h"
#include "Sound/bcSoundApi.h"

namespace black_cat
{
	namespace sound
	{
		template<bc_sound_api TApi>
		struct bc_platform_channel_pack
		{
		};

		template<bc_sound_api TApi>
		class bc_platform_channel
		{
		public:
			using platform_pack = bc_platform_channel_pack<TApi>;

		public:
			bc_platform_channel() noexcept;

			bc_platform_channel(platform_pack& p_pack) noexcept;

			bc_platform_channel(const bc_platform_channel&) noexcept;

			~bc_platform_channel() noexcept;

			bc_platform_channel& operator=(const bc_platform_channel&) noexcept;

			bool is_playing() const noexcept;

			void resume() noexcept;

			void pause() noexcept;

			void stop() const noexcept;

			bcFLOAT get_volume() const noexcept;

			void set_volume(bcFLOAT p_volume) noexcept;

			bool get_mute() const noexcept;

			void set_mute(bool p_value) noexcept;

			bcFLOAT get_pitch() const noexcept;

			void set_pitch(bcFLOAT p_pitch) const noexcept;

			bc_sound_mode get_mode() const noexcept;

			void set_mode(bc_sound_mode p_mode) noexcept;

			core::bc_vector3f get_3d_position() const noexcept;

			void set_3d_position(const core::bc_vector3f& p_position) noexcept;

			std::tuple<bcFLOAT, bcFLOAT> get_3d_min_max_distance() const noexcept;

			void set_3d_min_max_distance(bcFLOAT p_min, bcFLOAT p_max) noexcept;

			bool is_valid() const noexcept;

			void* get_user_data() const noexcept;

			void set_user_data(void* p_data) noexcept;

			platform_pack& get_platform_pack() noexcept
			{
				return m_pack;
			}

			const platform_pack& get_platform_pack() const noexcept
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;
		};

		using bc_channel = bc_platform_channel<g_current_sound_api>;
	}
}
