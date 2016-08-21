// [01/22/2016 MRB]

#pragma once

#include "CorePlatform/Utility/bcNoCopy.h"
#include "GraphicImp/Device/bcDevicePipeline.h"

namespace black_cat
{
	namespace game
	{
		// Represent a rendering task that do all work required to render all related stuff together
		class bc_irender_task : public core_platform::bc_no_copy
		{
		public:
			bc_irender_task() = default;

			virtual ~bc_irender_task() = default;

			virtual void update(core_platform::bc_clock::update_param p_clock_update_param) = 0;

			virtual void execute(graphic::bc_device_pipeline* p_pipeline) = 0;

		protected:

		private:

		};
	}
}
