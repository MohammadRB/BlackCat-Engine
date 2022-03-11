// [03/10/2022 MRB]

#include "SoundImp/SoundImpPCH.h"

#include "Core/Container/bcString.h"
#include "SoundImp/Fundation/bcDevice.h"
#include "SoundImp/Fundation/bcFmodError.h"
#include "SoundImp/Resource/bcSound.h"
#include "SoundImp/Resource/bcSoundChannel.h"
#include "SoundImp/bcExport.h"

namespace black_cat
{
	namespace sound
	{
		template<>
		BC_SOUNDIMP_DLL
		bc_platform_device<bc_sound_api::fmod>::bc_platform_device() noexcept
		{
		}

		template<>
		BC_SOUNDIMP_DLL
		bc_platform_device<bc_sound_api::fmod>::bc_platform_device(bc_platform_device&& p_other) noexcept
			: m_pack(p_other.m_pack)
		{
			p_other.m_pack.m_system = nullptr;
		}

		template<>
		BC_SOUNDIMP_DLL
		bc_platform_device<bc_sound_api::fmod>::~bc_platform_device() noexcept
		{
			if (is_initialized())
			{
				destroy();
			}
		}

		template<>
		BC_SOUNDIMP_DLL
		bc_platform_device<bc_sound_api::fmod>& bc_platform_device<bc_sound_api::fmod>::operator=(bc_platform_device&& p_other) noexcept
		{
			m_pack.m_system = p_other.m_pack.m_system;
			p_other.m_pack.m_system = nullptr;
			return *this;
		}

		template<>
		BC_SOUNDIMP_DLL
		bc_sound_ref bc_platform_device<bc_sound_api::fmod>::create_sound(const void* p_data, bcSIZE p_data_length)
		{
			FMOD::Sound* l_fmod_sound;
			FMOD_CREATESOUNDEXINFO l_fmod_info{};
			l_fmod_info.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
			l_fmod_info.length = p_data_length;

			bc_fmod_call(m_pack.m_system->createSound(static_cast<const bcCHAR*>(p_data), FMOD_OPENMEMORY, &l_fmod_info, &l_fmod_sound));

			bc_sound::platform_pack l_pack{ l_fmod_sound };
			const bc_sound l_sound(l_pack);
			return bc_sound_ref(l_sound);
		}

		template<>
		BC_SOUNDIMP_DLL
		bc_sound_ref bc_platform_device<bc_sound_api::fmod>::create_sound(core::bc_estring_view p_path)
		{
			FMOD::Sound* l_fmod_sound;
			const auto l_path = core::bc_to_exclusive_string(p_path.data());

			bc_fmod_call(m_pack.m_system->createSound(l_path.c_str(), FMOD_DEFAULT, nullptr, &l_fmod_sound));

			bc_sound::platform_pack l_pack{ l_fmod_sound };
			const bc_sound l_sound(l_pack);
			return bc_sound_ref(l_sound);
		}

		template<>
		BC_SOUNDIMP_DLL
		bc_sound_ref bc_platform_device<bc_sound_api::fmod>::create_compressed_sound(const void* p_data, bcSIZE p_data_length)
		{
			FMOD::Sound* l_fmod_sound;
			FMOD_CREATESOUNDEXINFO l_fmod_info{};
			l_fmod_info.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
			l_fmod_info.length = p_data_length;

			bc_fmod_call(m_pack.m_system->createSound(static_cast<const bcCHAR*>(p_data), FMOD_OPENMEMORY | FMOD_CREATECOMPRESSEDSAMPLE, &l_fmod_info, &l_fmod_sound));

			bc_sound::platform_pack l_pack{ l_fmod_sound };
			const bc_sound l_sound(l_pack);
			return bc_sound_ref(l_sound);
		}

		template<>
		BC_SOUNDIMP_DLL
		bc_sound_ref bc_platform_device<bc_sound_api::fmod>::create_compressed_sound(core::bc_estring_view p_path)
		{
			FMOD::Sound* l_fmod_sound;
			const auto l_path = core::bc_to_exclusive_string(p_path.data());

			bc_fmod_call(m_pack.m_system->createSound(l_path.c_str(), FMOD_CREATECOMPRESSEDSAMPLE, nullptr, &l_fmod_sound));

			bc_sound::platform_pack l_pack{ l_fmod_sound };
			const bc_sound l_sound(l_pack);
			return bc_sound_ref(l_sound);
		}

		template<>
		BC_SOUNDIMP_DLL
		bc_sound_ref bc_platform_device<bc_sound_api::fmod>::create_stream_sound(core::bc_estring_view p_path)
		{
			FMOD::Sound* l_fmod_sound;
			const auto l_path = core::bc_to_exclusive_string(p_path.data());

			bc_fmod_call(m_pack.m_system->createSound(l_path.c_str(), FMOD_CREATESTREAM, nullptr, &l_fmod_sound));

			bc_sound::platform_pack l_pack{ l_fmod_sound };
			const bc_sound l_sound(l_pack);
			return bc_sound_ref(l_sound);
		}

		template<>
		BC_SOUNDIMP_DLL
		bc_sound_channel bc_platform_device<bc_sound_api::fmod>::play_sound(bc_sound p_sound, bool p_paused)
		{
			auto* l_fmod_sound = static_cast<FMOD::Sound*>(p_sound.get_platform_pack().m_pointer);
			auto* l_fmod_channel = static_cast<FMOD::Channel*>(nullptr);

			bc_fmod_call(m_pack.m_system->playSound(l_fmod_sound, nullptr, p_paused, &l_fmod_channel));

			bc_sound_channel::platform_pack l_channel_pack{ l_fmod_channel };
			return bc_sound_channel(l_channel_pack);
		}

		template<>
		BC_SOUNDIMP_DLL
		void bc_platform_device<bc_sound_api::fmod>::update(const update_context& p_context)
		{
			const FMOD_VECTOR l_listener_position{ p_context.m_listener_position.x, p_context.m_listener_position.y, p_context.m_listener_position.z };
			const FMOD_VECTOR l_listener_velocity{ 0,0,0 };
			const FMOD_VECTOR l_listener_forward{ p_context.m_listener_forward.x, p_context.m_listener_forward.y, p_context.m_listener_forward.z };
			const FMOD_VECTOR l_listener_up{ p_context.m_listener_up.x, p_context.m_listener_up.y, p_context.m_listener_up.z };

			bc_fmod_call(m_pack.m_system->set3DListenerAttributes(0, &l_listener_position, &l_listener_velocity, &l_listener_forward, &l_listener_up));
			bc_fmod_call(m_pack.m_system->update());
		}

		template<>
		BC_SOUNDIMP_DLL
		void bc_platform_device<bc_sound_api::fmod>::_initialize(bc_device_init_params p_params)
		{
			FMOD_MODE l_mode = FMOD_INIT_NORMAL;

			if(!p_params.m_is_left_hand)
			{
				l_mode |= FMOD_INIT_3D_RIGHTHANDED;
			}

			bc_fmod_call(FMOD::System_Create(&m_pack.m_system));
			bc_fmod_call(m_pack.m_system->init(static_cast<bcINT>(p_params.m_max_channels), l_mode, nullptr));
			bc_fmod_call(m_pack.m_system->set3DSettings(1.f, 1.f, 1.f));
		}

		template<>
		BC_SOUNDIMP_DLL
		void bc_platform_device<bc_sound_api::fmod>::_destroy()
		{
			bc_fmod_call(m_pack.m_system->close());
			bc_fmod_call(m_pack.m_system->release());
		}
	}
}
