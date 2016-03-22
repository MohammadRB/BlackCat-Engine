// [02/23/2016 MRB]

#pragma once

#include "Core/Utility/bcParameterPack.h"
#include "Graphic/bcPlatformRenderApi.h"

namespace black_cat
{
	namespace graphic
	{
		template<bc_platform_render_api>
		class bc_platform_device_pipeline;
		using bc_device_pipeline = bc_platform_device_pipeline< g_current_platform_render_api >;

		// Command executer on device pipeline
		class bc_irender_command
		{
		public:
			bc_irender_command() = default;

			virtual ~bc_irender_command() = default;

			virtual void execute(bc_device_pipeline* p_pipeline, core::bc_parameter_pack&& p_parameters) = 0;

		protected:

		private:

		};
	}
}
