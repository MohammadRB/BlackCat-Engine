// [03/10/2022 MRB]

#pragma once

#include "Core/Container/bcStringStream.h"
#include "Core/Utility/bcLogger.h"
#include "Sound/bcException.h"
#include "3rdParty/FMOD/Include/fmod.hpp"
#include "3rdParty/FMOD/Include/fmod_errors.h"

namespace black_cat
{
	namespace sound
	{
		inline FMOD_RESULT  bc_fmod_throw(FMOD_RESULT p_result, FMOD_RESULT p_ignores = FMOD_OK)
		{
			if (p_result != FMOD_OK && !(p_result & p_ignores))
			{
				core::bc_string_stream_frame l_stream;
				l_stream << "FMOD error, code: " << p_result << " message: " << FMOD_ErrorString(p_result);

				throw bc_sound_exception(p_result, l_stream.str().c_str());
			}

			return p_result;
		}

		inline FMOD_RESULT  bc_fmod_log(FMOD_RESULT p_result, FMOD_RESULT p_ignores = FMOD_OK)
		{
			if (p_result != FMOD_OK && !(p_result & p_ignores))
			{
				core::bc_string_stream_frame l_stream;
				l_stream << "FMOD error, code: " << p_result << " message: " << FMOD_ErrorString(p_result);

				core::bc_log(core::bc_log_type::error) << l_stream.str() << core::bc_lend;
			}

			return p_result;
		}
	}
}
