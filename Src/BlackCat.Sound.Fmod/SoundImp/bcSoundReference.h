// [09/03/2022 MRB]

#pragma once

#include "Sound/bcSoundReference.h"

namespace black_cat
{
	namespace sound
	{
		template<>
		struct bc_platform_sound_reference_pack<bc_sound_api::fmod>
		{
			void* m_pointer{ nullptr };
		};

		template<>
		inline bool bc_platform_sound_reference<bc_sound_api::fmod>::operator==(const bc_platform_sound_reference& p_other) const noexcept
		{
			return get_platform_pack().m_pointer == p_other.get_platform_pack().m_pointer;
		}

		template<>
		inline bool bc_platform_sound_reference<bc_sound_api::fmod>::operator!=(const bc_platform_sound_reference& p_other) const noexcept
		{
			return get_platform_pack().m_pointer != p_other.get_platform_pack().m_pointer;
		}

		template<>
		inline bool bc_platform_sound_reference<bc_sound_api::fmod>::operator==(std::nullptr_t) const noexcept
		{
			return get_platform_pack().m_pointer == nullptr;
		}

		template<>
		inline bool bc_platform_sound_reference<bc_sound_api::fmod>::operator!=(std::nullptr_t) const noexcept
		{
			return get_platform_pack().m_pointer != nullptr;
		}

		template<>
		inline bool bc_platform_sound_reference<bc_sound_api::fmod>::is_valid() const noexcept
		{
			return *this != nullptr;
		}

		template<>
		inline bc_platform_sound_reference<bc_sound_api::fmod>::bc_platform_sound_reference() noexcept = default;

		template<>
		inline bc_platform_sound_reference<bc_sound_api::fmod>::bc_platform_sound_reference(const bc_platform_sound_reference&) noexcept = default;

		template<>
		inline bc_platform_sound_reference<bc_sound_api::fmod>::~bc_platform_sound_reference() noexcept = default;

		template<>
		inline bc_platform_sound_reference<bc_sound_api::fmod>& bc_platform_sound_reference<bc_sound_api::fmod>::operator=(const bc_platform_sound_reference&) noexcept = default;
	}
}
