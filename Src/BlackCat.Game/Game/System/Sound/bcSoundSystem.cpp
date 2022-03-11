// [03/11/2022 MRB]

#include "Game/GamePCH.h"

#include "Core/Concurrency/bcConcurrency.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "Game/System/Sound/bcSoundSystem.h"

namespace black_cat
{
	namespace game
	{
		void bc_sound_system::update(const update_context& p_context)
		{
			m_device.update
			(
				sound::bc_device::update_context
				{
					p_context.m_clock,
					p_context.m_camera.get_position(),
					p_context.m_camera.get_forward(),
					p_context.m_camera.get_up()
				}
			);
		}

		core::bc_task<void> bc_sound_system::update_async(const update_context& p_context)
		{
			auto l_task = core::bc_concurrency::start_task
			(
				core::bc_delegate<void()>
				(
					[=, &p_context]()
					{
						update(p_context);
					}
				)
			);

			return l_task;
		}

		void bc_sound_system::_initialize(bc_sound_system_params p_params)
		{
			m_device.initialize(sound::bc_device_init_params{ p_params.m_max_channels, graphic::bc_render_api_info::use_left_handed() });
		}

		void bc_sound_system::_destroy()
		{
			m_device.destroy();
		}
	}
}
