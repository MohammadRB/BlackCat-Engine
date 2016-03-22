// [01/22/2016 MRB]

#pragma once

#include "CorePlatform/bcCorePlatformUtility.h"
#include "Graphic/bcPlatformRenderApi.h"

namespace black_cat
{
	namespace graphic
	{
		template<bc_platform_render_api>
		class bc_platform_device_pipeline;
		using bc_device_pipeline = bc_platform_device_pipeline< g_current_platform_render_api >;

		// Represent a rendering task that do all work required to render all related stuff together
		class bc_irender_task : public core_platform::bc_no_copy
		{
		public:
			bc_irender_task() = default;

			virtual ~bc_irender_task() = default;

			virtual void update(bcFLOAT32 p_time_elapsed) = 0;

			virtual void execute(bc_device_pipeline* p_pipeline) = 0;

		protected:

		private:

		};
	}
}
