// [03/10/2022 MRB]

#include "SoundImp/SoundImpPCH.h"

#include "Core/Container/bcString.h"
#include "SoundImp/Fundation/bcDevice.h"
#include "SoundImp/Fundation/bcFmodError.h"
#include "SoundImp/Resource/bcSound.h"
#include "SoundImp/Resource/bcChannel.h"
#include "SoundImp/Resource/bcChannelGroup.h"
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
		bc_channel_group bc_platform_device<bc_sound_api::fmod>::get_default_channel_group() noexcept
		{
			FMOD::ChannelGroup* l_fmod_group;
			bc_fmod_throw(m_pack.m_system->getMasterChannelGroup(&l_fmod_group));

			bc_channel_group::platform_pack l_pack{ l_fmod_group };
			return bc_channel_group(l_pack);
		}

		template<>
		BC_SOUNDIMP_DLL
		bc_sound_ref bc_platform_device<bc_sound_api::fmod>::create_sound(const void* p_data, bcSIZE p_data_length, bc_sound_mode p_mode)
		{
			FMOD_MODE l_mode = FMOD_DEFAULT | m_pack.m_default_rolloff | FMOD_OPENMEMORY;
			FMOD_CREATESOUNDEXINFO l_fmod_info{};
			l_fmod_info.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
			l_fmod_info.length = p_data_length;

			if(core::bc_enum::has(p_mode, bc_sound_mode::d3))
			{
				l_mode |= FMOD_3D;
			}

			FMOD::Sound* l_fmod_sound;
			bc_fmod_throw(m_pack.m_system->createSound(static_cast<const bcCHAR*>(p_data), l_mode, &l_fmod_info, &l_fmod_sound));

			bc_sound::platform_pack l_pack{ l_fmod_sound };
			bc_sound l_sound(l_pack);
			l_sound.set_mode(p_mode);

			return bc_sound_ref(l_sound);
		}

		template<>
		BC_SOUNDIMP_DLL
		bc_sound_ref bc_platform_device<bc_sound_api::fmod>::create_sound(core::bc_estring_view p_path, bc_sound_mode p_mode)
		{
			const auto l_path = core::bc_to_exclusive_string(p_path.data());
			FMOD_MODE l_mode = FMOD_DEFAULT | m_pack.m_default_rolloff;

			if (core::bc_enum::has(p_mode, bc_sound_mode::d3))
			{
				l_mode |= FMOD_3D;
			}

			FMOD::Sound* l_fmod_sound;
			bc_fmod_throw(m_pack.m_system->createSound(l_path.c_str(), l_mode, nullptr, &l_fmod_sound));

			bc_sound::platform_pack l_pack{ l_fmod_sound };
			bc_sound l_sound(l_pack);
			l_sound.set_mode(p_mode);

			return bc_sound_ref(l_sound);
		}

		template<>
		BC_SOUNDIMP_DLL
		bc_sound_ref bc_platform_device<bc_sound_api::fmod>::create_compressed_sound(const void* p_data, bcSIZE p_data_length, bc_sound_mode p_mode)
		{
			FMOD_MODE l_mode = FMOD_DEFAULT | m_pack.m_default_rolloff | FMOD_OPENMEMORY | FMOD_CREATECOMPRESSEDSAMPLE;
			FMOD_CREATESOUNDEXINFO l_fmod_info{};
			l_fmod_info.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
			l_fmod_info.length = p_data_length;

			if (core::bc_enum::has(p_mode, bc_sound_mode::d3))
			{
				l_mode |= FMOD_3D;
			}

			FMOD::Sound* l_fmod_sound;
			bc_fmod_throw(m_pack.m_system->createSound(static_cast<const bcCHAR*>(p_data), l_mode, &l_fmod_info, &l_fmod_sound));

			bc_sound::platform_pack l_pack{ l_fmod_sound };
			bc_sound l_sound(l_pack);
			//l_sound.set_mode(p_mode);

			return bc_sound_ref(l_sound);
		}

		template<>
		BC_SOUNDIMP_DLL
		bc_sound_ref bc_platform_device<bc_sound_api::fmod>::create_compressed_sound(core::bc_estring_view p_path, bc_sound_mode p_mode)
		{
			const auto l_path = core::bc_to_exclusive_string(p_path.data());
			FMOD_MODE l_mode = FMOD_DEFAULT | m_pack.m_default_rolloff | FMOD_CREATECOMPRESSEDSAMPLE;

			if (core::bc_enum::has(p_mode, bc_sound_mode::d3))
			{
				l_mode |= FMOD_3D;
			}

			FMOD::Sound* l_fmod_sound;
			bc_fmod_throw(m_pack.m_system->createSound(l_path.c_str(), l_mode, nullptr, &l_fmod_sound));

			bc_sound::platform_pack l_pack{ l_fmod_sound };
			bc_sound l_sound(l_pack);
			l_sound.set_mode(p_mode);

			return bc_sound_ref(l_sound);
		}

		template<>
		BC_SOUNDIMP_DLL
		bc_sound_ref bc_platform_device<bc_sound_api::fmod>::create_stream_sound(core::bc_estring_view p_path, bc_sound_mode p_mode)
		{
			const auto l_path = core::bc_to_exclusive_string(p_path.data());
			FMOD_MODE l_mode = FMOD_DEFAULT | m_pack.m_default_rolloff | FMOD_CREATESTREAM;

			if (core::bc_enum::has(p_mode, bc_sound_mode::d3))
			{
				l_mode |= FMOD_3D;
			}

			FMOD::Sound* l_fmod_sound;
			bc_fmod_throw(m_pack.m_system->createSound(l_path.c_str(), l_mode, nullptr, &l_fmod_sound));

			bc_sound::platform_pack l_pack{ l_fmod_sound };
			bc_sound l_sound(l_pack);
			l_sound.set_mode(p_mode);

			return bc_sound_ref(l_sound);
		}

		template<>
		BC_SOUNDIMP_DLL
		bc_channel_group_ref bc_platform_device<bc_sound_api::fmod>::create_channel_group(core::bc_string_view p_name)
		{
			FMOD::ChannelGroup* l_fmod_group;
			bc_fmod_throw(m_pack.m_system->createChannelGroup(p_name.data(), &l_fmod_group));

			bc_channel_group::platform_pack l_pack{ l_fmod_group };
			const bc_channel_group l_group(l_pack);
			return bc_channel_group_ref(l_group);
		}

		template<>
		BC_SOUNDIMP_DLL
		bc_channel bc_platform_device<bc_sound_api::fmod>::play_sound(const bc_sound& p_sound, const bc_channel_group& p_group, bool p_paused) noexcept
		{
			auto* l_fmod_sound = static_cast<FMOD::Sound*>(p_sound.get_platform_pack().m_pointer);
			auto* l_fmod_channel_group = static_cast<FMOD::ChannelGroup*>(p_group.get_platform_pack().m_pointer);
			auto* l_fmod_channel = static_cast<FMOD::Channel*>(nullptr);

			bc_fmod_log(m_pack.m_system->playSound(l_fmod_sound, l_fmod_channel_group, p_paused, &l_fmod_channel));

			bc_channel::platform_pack l_channel_pack{ l_fmod_channel };
			return bc_channel(l_channel_pack);
		}

		template<>
		BC_SOUNDIMP_DLL
		void bc_platform_device<bc_sound_api::fmod>::update(const update_context& p_context) noexcept
		{
			const FMOD_VECTOR l_listener_position{ p_context.m_listener_position.x, p_context.m_listener_position.y, p_context.m_listener_position.z };
			const FMOD_VECTOR l_listener_forward{ p_context.m_listener_forward.x, p_context.m_listener_forward.y, p_context.m_listener_forward.z };
			const FMOD_VECTOR l_listener_up{ p_context.m_listener_up.x, p_context.m_listener_up.y, p_context.m_listener_up.z };
			const FMOD_VECTOR l_listener_velocity{ p_context.m_listener_velocity.x, p_context.m_listener_velocity.y, p_context.m_listener_velocity.z };

			bc_fmod_log(m_pack.m_system->set3DListenerAttributes(0, &l_listener_position, &l_listener_velocity, &l_listener_forward, &l_listener_up));
			bc_fmod_log(m_pack.m_system->update());
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

			m_pack.m_default_rolloff = FMOD_3D_LINEARSQUAREROLLOFF;
			bc_fmod_throw(FMOD::System_Create(&m_pack.m_system));
			bc_fmod_throw(m_pack.m_system->init(static_cast<bcINT>(p_params.m_max_channels), l_mode, nullptr));
			bc_fmod_throw(m_pack.m_system->set3DSettings(1.f, 1.f, 1.f));
		}

		template<>
		BC_SOUNDIMP_DLL
		void bc_platform_device<bc_sound_api::fmod>::_destroy()
		{
			bc_fmod_throw(m_pack.m_system->close());
			bc_fmod_throw(m_pack.m_system->release());
		}
	}
}
