// [09/03/2022 MRB]

#pragma once

#include "Sound/bcSoundApi.h"

namespace black_cat::sound
{
	template<bc_sound_api TApi>
	struct bc_platform_sound_reference_pack
	{
	};

	template<bc_sound_api TApi>
	class bc_platform_sound_reference
	{
	public:
		using platform_pack = bc_platform_sound_reference_pack<TApi>;

	public:
		bool operator==(const bc_platform_sound_reference& p_other) const noexcept;

		bool operator!=(const bc_platform_sound_reference& p_other) const noexcept;

		bool operator==(std::nullptr_t) const noexcept;

		bool operator!=(std::nullptr_t) const noexcept;

		virtual bool is_valid() const noexcept;

		virtual platform_pack& get_platform_pack() noexcept = 0;

		virtual const platform_pack& get_platform_pack() const noexcept = 0;

	protected:
		bc_platform_sound_reference() noexcept;

		bc_platform_sound_reference(const bc_platform_sound_reference&) noexcept;

		virtual ~bc_platform_sound_reference() noexcept;

		bc_platform_sound_reference& operator=(const bc_platform_sound_reference&) noexcept;
	};

	using bc_sound_reference = bc_platform_sound_reference<g_current_sound_api>;
}
