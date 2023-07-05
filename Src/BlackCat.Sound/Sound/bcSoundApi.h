// [09/03/2022 MRB]

#pragma once

namespace black_cat::sound
{
	enum class bc_sound_api
	{
		unknown = 0,
		fmod = 1
	};

	constexpr bc_sound_api g_current_sound_api = bc_sound_api::fmod;
}
