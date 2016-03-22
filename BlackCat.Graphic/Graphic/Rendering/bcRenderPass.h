// [01/22/2016 MRB]

#pragma once

#include "CorePlatform/bcCorePlatformUtility.h"
#include "Graphic/bcPlatformRenderApi.h"

namespace black_cat
{
	namespace graphic
	{
		template<bc_platform_render_api>
		class bc_platform_device;
		using bc_device = bc_platform_device< g_current_platform_render_api >;

		template<bc_platform_render_api>
		class bc_platform_device_pipeline;
		using bc_device_pipeline = bc_platform_device_pipeline< g_current_platform_render_api >;

		template<bc_platform_render_api>
		class bc_platform_device_command_executer;
		using bc_device_command_executer = bc_platform_device_command_executer< g_current_platform_render_api >;

		// Represent a whole rendering pass that do all tasks that required to render a scene with
		// a specified configuration (Don't use movale memory for this type becuase render passes
		// usually register themselve in device via raw pointers) 
		class bc_irender_pass : public core_platform::bc_no_copy
		{
		public:
			bc_irender_pass() = default;

			virtual ~bc_irender_pass() = default;

			// This function will be called during app initialization
			virtual void initialize_resources(bc_device* p_device) = 0;

			// This function will be called during app update phase
			virtual void update(bcFLOAT32 p_time_elapsed) = 0;

			// This function will be called in start of frame
			// (Default implementation call set_render_parameters function)
			virtual void initialize_frame(bc_device_command_executer* p_executer, bc_device_pipeline* p_pipeline);

			// This function will be called in draw frme phase
			virtual void execute(bc_device_command_executer* p_executer, bc_device_pipeline* p_pipeline) = 0;

			// This function will be called when pass is going to be destroy
			// (before device destroying this function must be called) 
			virtual void destroy(bc_device* p_device) = 0;

			virtual core::bc_string get_name() = 0;

		protected:
			virtual void set_render_parameters(bc_device_pipeline* p_pipeline) = 0;

		private:
		};
		
		inline void bc_irender_pass::initialize_frame(bc_device_command_executer* p_executer, bc_device_pipeline* p_pipeline)
		{
			set_render_parameters(p_pipeline);
		}
	}
}