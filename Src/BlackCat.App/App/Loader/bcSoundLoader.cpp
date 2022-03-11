// [03/11/2022 MRB]

#include "App/AppPCH.h"

#include "SoundImp/Resource/bcSound.h"
#include "Game/System/Sound/bcSoundSystem.h"
#include "Game/System/bcGameSystem.h"
#include "App/Loader/bcSoundLoader.h"

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

		auto l_sound = l_device.create_sound(p_context.m_file_buffer.get(), p_context.m_file_buffer_size);

		p_context.set_result(sound::bc_sound_content(std::move(l_sound)));
	}
}
