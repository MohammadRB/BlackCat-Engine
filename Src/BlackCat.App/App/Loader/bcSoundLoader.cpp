// [03/11/2022 MRB]

#include "App/AppPCH.h"

#include "Core/Utility/bcLogger.h"
#include "SoundImp/Resource/bcSound.h"
#include "Game/System/Sound/bcSoundSystem.h"
#include "Game/System/bcGameSystem.h"
#include "App/Loader/bcSoundLoader.h"
#include "App/bcConstant.h"
#include "App/bcException.h"

namespace black_cat
{
	bool bc_sound_loader::support_offline_processing() const
	{
		return false;
	}

	void bc_sound_loader::content_processing(core::bc_content_loading_context& p_context) const
	{
		auto& l_game_system = *core::bc_get_service<game::bc_game_system>();
		auto& l_device = l_game_system.get_sound_system().get_device();
		auto* l_load_option = p_context.m_parameters.get_value<core::bc_string>(constant::g_param_sound_load_mode);
		auto l_sound = sound::bc_sound_ref();

		if(l_load_option && *l_load_option == "compressed")
		{
			l_sound = l_device.create_compressed_sound(p_context.m_file_buffer.get(), p_context.m_file_buffer_size);
		}
		else if(l_load_option && *l_load_option == "stream")
		{
			l_sound = l_device.create_stream_sound(p_context.m_file_path);
		}
		else if(l_load_option && *l_load_option == "default")
		{
			l_sound = l_device.create_sound(p_context.m_file_buffer.get(), p_context.m_file_buffer_size);
		}
		else
		{
			throw bc_invalid_argument_exception("invalid sound load option");
		}

		p_context.set_result(sound::bc_sound_content(std::move(l_sound)));
	}
}
