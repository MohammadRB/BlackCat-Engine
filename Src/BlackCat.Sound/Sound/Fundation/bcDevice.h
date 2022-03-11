// [03/09/2022 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcString.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Utility/bcInitializable.h"
#include "Sound/bcSoundRef.h"
#include "Sound/bcSoundApi.h"

namespace black_cat
{
	namespace sound
	{
		template<bc_sound_api TApi>
		class bc_platform_sound;
		using bc_sound = bc_platform_sound<g_current_sound_api>;
		using bc_sound_ref = bc_sounds_ref<bc_sound>;

		template<bc_sound_api TApi>
		class bc_platform_sound_channel;
		using bc_sound_channel = bc_platform_sound_channel<g_current_sound_api>;

		template<bc_sound_api TApi>
		struct bc_platform_device_pack
		{
		};

		struct bc_device_init_params
		{
			bcUINT32 m_max_channels;
			bool m_is_left_hand;
		};

		struct bc_device_update_context
		{
			platform::bc_clock::update_param m_clock;
			core::bc_vector3f m_listener_position;
			core::bc_vector3f m_listener_forward;
			core::bc_vector3f m_listener_up;
		};

		template<bc_sound_api TApi>
		class bc_platform_device : public core::bc_initializable<bc_device_init_params>
		{
		public:
			using platform_pack = bc_platform_device_pack<TApi>;
			using update_context = bc_device_update_context;

		public:
			bc_platform_device() noexcept;

			bc_platform_device(bc_platform_device&&) noexcept;

			~bc_platform_device() noexcept;

			bc_platform_device& operator=(bc_platform_device&&) noexcept;

			bc_sound_ref create_sound(const void* p_data, bcSIZE p_data_length);

			bc_sound_ref create_sound(core::bc_estring_view p_path);

			bc_sound_ref create_compressed_sound(const void* p_data, bcSIZE p_data_length);

			bc_sound_ref create_compressed_sound(core::bc_estring_view p_path);

			bc_sound_ref create_stream_sound(core::bc_estring_view p_path);

			bc_sound_channel play_sound(bc_sound p_sound, bool p_paused);

			void update(const update_context& p_context);

			platform_pack& get_platform_pack() noexcept
			{
				return m_pack;
			}

			const platform_pack& get_platform_pack() const noexcept
			{
				return m_pack;
			}

		private:
			void _initialize(bc_device_init_params) override;

			void _destroy() override;

			platform_pack m_pack;
		};

		using bc_device = bc_platform_device<g_current_sound_api>;
	}
}
