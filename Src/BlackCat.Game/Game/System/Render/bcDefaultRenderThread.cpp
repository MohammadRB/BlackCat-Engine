// [12/04/2020 MRB]

#include "Game/GamePCH.h"

#include "GraphicImp/Device/Command/bcDeviceCommandList.h"
#include "GraphicImp/Device/Command/bcDeviceCommandExecutor.h"
#include "Game/System/Render/bcDefaultRenderThread.h"

namespace black_cat
{
	namespace game
	{
		void bc_default_render_thread::start() noexcept
		{
			get_pipeline().start_command_list();
		}

		void bc_default_render_thread::finish() noexcept
		{
			graphic::bc_device_command_list l_command_list;
			get_pipeline().finish_command_list(l_command_list);
		}

		void bc_default_render_thread::reset()
		{
			bc_render_thread::reset();
		}

		void bc_default_render_thread::reset(graphic::bc_device_pipeline_ptr p_pipeline)
		{
			bc_render_thread::reset(std::move(p_pipeline), graphic::bc_device_command_executor_ptr());
		}
	}
}