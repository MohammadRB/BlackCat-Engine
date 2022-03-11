// [03/10/2022 MRB]

#include "SoundImp/SoundImpPCH.h"
#include "SoundImp/Fundation/bcFmodError.h"
#include "SoundImp/Resource/bcSound.h"
#include "SoundImp/bcExport.h"
#include "3rdParty/FMOD/Include/fmod.hpp"

namespace black_cat
{
	namespace sound
	{
		template<>
		BC_SOUNDIMP_DLL
		bc_platform_sound<bc_sound_api::fmod>::bc_platform_sound() noexcept
		{
		}

		template<>
		BC_SOUNDIMP_DLL
		bc_platform_sound<bc_sound_api::fmod>::bc_platform_sound(platform_pack& p_pack) noexcept
		{
			m_pack.m_pointer = p_pack.m_pointer;
		}

		template<>
		BC_SOUNDIMP_DLL
		bc_platform_sound<bc_sound_api::fmod>::bc_platform_sound(const bc_platform_sound& p_other) noexcept
			: bc_platform_sound_reference<bc_sound_api::fmod>(p_other),
			m_pack(p_other.m_pack)
		{
		}

		template<>
		BC_SOUNDIMP_DLL
		bc_platform_sound<bc_sound_api::fmod>::~bc_platform_sound() noexcept
		{
		}

		template<>
		BC_SOUNDIMP_DLL
		bc_platform_sound<bc_sound_api::fmod>& bc_platform_sound<bc_sound_api::fmod>::operator=(const bc_platform_sound& p_other) noexcept
		{
			bc_platform_sound_reference<bc_sound_api::fmod>::operator=(p_other);
			m_pack = p_other.m_pack;

			return *this;
		}

		template<>
		BC_SOUNDIMP_DLL
		bc_sound_mode bc_platform_sound<bc_sound_api::fmod>::get_mode() const noexcept
		{
			auto* l_fmod_sound = static_cast<FMOD::Sound*>(m_pack.m_pointer);
			auto l_fmod_mod = FMOD_MODE();

			bc_fmod_call(l_fmod_sound->getMode(&l_fmod_mod));

			auto l_mode = bc_sound_mode::none;

			if(l_fmod_mod & FMOD_LOOP_OFF)
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
		void bc_platform_sound<bc_sound_api::fmod>::set_mode(bc_sound_mode p_mode) noexcept
		{
			auto* l_fmod_sound = static_cast<FMOD::Sound*>(m_pack.m_pointer);

			if(core::bc_enum::has(p_mode, bc_sound_mode::loop_off))
			{
				l_fmod_sound->setMode(FMOD_LOOP_OFF);
			}
			if (core::bc_enum::has(p_mode, bc_sound_mode::loop))
			{
				l_fmod_sound->setMode(FMOD_LOOP_NORMAL);
			}
			if (core::bc_enum::has(p_mode, bc_sound_mode::d3))
			{
				l_fmod_sound->setMode(FMOD_3D);
			}
		}

		template<>
		BC_SOUNDIMP_DLL
		void* bc_platform_sound<bc_sound_api::fmod>::get_user_data() const noexcept
		{
			auto* l_fmod_sound = static_cast<FMOD::Sound*>(m_pack.m_pointer);
			void* l_user_data;

			l_fmod_sound->getUserData(&l_user_data);

			return l_user_data;
		}

		template<>
		BC_SOUNDIMP_DLL
		void bc_platform_sound<bc_sound_api::fmod>::set_user_data(void* p_data) noexcept
		{
			auto* l_fmod_sound = static_cast<FMOD::Sound*>(m_pack.m_pointer);
			l_fmod_sound->setUserData(p_data);
		}
	}
}
