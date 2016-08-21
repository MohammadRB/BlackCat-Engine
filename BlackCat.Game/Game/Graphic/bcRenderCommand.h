// [02/23/2016 MRB]

#pragma once

#include "Core/Utility/bcParameterPack.h"
#include "GraphicImp/Device/bcDevicePipeline.h"

namespace black_cat
{
	namespace game
	{
		// Command executer on device pipeline
		class bc_irender_command
		{
		public:
			bc_irender_command() = default;

			virtual ~bc_irender_command() = default;

			virtual void execute(graphic::bc_device_pipeline* p_pipeline, core::bc_parameter_pack&& p_parameters) = 0;

		protected:

		private:

		};
	}
}
