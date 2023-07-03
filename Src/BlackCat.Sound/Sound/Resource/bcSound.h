// [10/03/2022 MRB]

#pragma once

#include "Core/bcConstant.h"
#include "Sound/Resource/bcResourceContent.h"
#include "Sound/bcSoundRef.h"
#include "Sound/bcSoundReference.h"
#include "Sound/bcDefinition.h"
#include "Sound/bcSoundApi.h"

namespace black_cat
{
	namespace sound
	{
		template<bc_sound_api TApi>
		struct bc_platform_sound_pack : bc_platform_sound_reference_pack<TApi>
		{
		};

		template<bc_sound_api TApi>
		class bc_platform_sound : public bc_platform_sound_reference<TApi>
		{
		public:
			using platform_pack = bc_platform_sound_pack<TApi>;

		public:
			bc_platform_sound() noexcept;

			bc_platform_sound(platform_pack& p_pack) noexcept;

			bc_platform_sound(const bc_platform_sound&) noexcept;

			~bc_platform_sound() noexcept override;

			bc_platform_sound& operator=(const bc_platform_sound&) noexcept;

			/**
			 * \brief Return length of the sound in ms
			 * \return 
			 */
			bcUINT32 get_length() const noexcept;

			bc_sound_mode get_mode() const noexcept;

			void set_mode(bc_sound_mode p_mode) noexcept;

			void* get_user_data() const noexcept;

			void set_user_data(void* p_data) noexcept;

			platform_pack& get_platform_pack() noexcept override
			{
				return m_pack;
			}

			const platform_pack& get_platform_pack() const noexcept override
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;
		};

		constexpr bcCHAR g_sound_content_name[] = BC_CONTENT_NAME(sound);
		using bc_sound = bc_platform_sound<g_current_sound_api>;
		using bc_sound_ref = bc_sounds_ref<bc_sound>;
		using bc_sound_content = bc_device_resource_content<bc_sound, g_sound_content_name>;
		using bc_sound_content_ptr = core::bc_content_ptr<bc_sound_content>;
	}
}
