// [01/22/2016 MRB]

#pragma once

#include "CorePlatform/Utility/bcNoCopy.h"
#include "CorePlatformImp/Utility/bcClock.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Device/Command/bcDeviceCommandExecuter.h"
#include "Game/System/Render/bcRenderThread.h"

namespace black_cat
{
	namespace game
	{
		struct bc_render_system_update_param;
		class bc_render_system;

		// Represent a whole rendering pass that do all tasks that required to render a scene with
		// a specified configuration (Don't use movale memory for this type becuase render passes
		// usually register themselve in device via raw pointers) 
		class bc_irender_pass : public core_platform::bc_no_copy
		{
		public:
			bc_irender_pass() = default;

			virtual ~bc_irender_pass() = default;

			// This function will be called during app initialization
			virtual void initialize_resources(bc_render_system& p_render_system, graphic::bc_device* p_device) = 0;

			// This function will be called during app update phase
			virtual void update(const bc_render_system_update_param& p_update_param) = 0;

			// This function will be called in start of frame
			// (Default implementation call set_render_parameters function)
			virtual void initialize_frame(bc_render_system& p_render_system, bc_render_thread& p_thread) = 0;

			// This function will be called in draw frme phase
			virtual void execute(bc_render_system& p_render_system, bc_render_thread& p_thread) = 0;

			// This function will be called when device duo to some parameter changes and buffer resize need reset
			virtual void before_reset(bc_render_system& p_render_system, graphic::bc_device* p_device, graphic::bc_device_parameters& p_old_parameters, graphic::bc_device_parameters& p_new_parameters) = 0;

			// This function will be called when device duo to some parameter changes and buffer resize need reset
			virtual void after_reset(bc_render_system& p_render_system, graphic::bc_device* p_device, graphic::bc_device_parameters& p_old_parameters, graphic::bc_device_parameters& p_new_parameters) = 0;

			// This function will be called when pass is going to be destroy
			// (before device destroying this function must be called) 
			virtual void destroy(graphic::bc_device* p_device) = 0;

			virtual core::bc_string get_name() = 0;

		protected:

		private:
		};
	}
}
