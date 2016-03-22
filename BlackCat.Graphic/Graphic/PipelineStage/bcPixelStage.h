// [01/14/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcPlatformRenderApi.h"
#include "Graphic/PipelineStage/bcProgrammableStage.h"

namespace black_cat
{
	namespace graphic
	{
		template< bc_platform_render_api TRenderApi >
		class bc_platform_pixel_stage : public bc_programmable_stage
		{
		public:
			bc_platform_pixel_stage();

			bc_platform_pixel_stage(bc_platform_pixel_stage&&);

			~bc_platform_pixel_stage();

			bc_platform_pixel_stage& operator=(bc_platform_pixel_stage&&);

		protected:
			void apply_shader_program(bc_device_pipeline* p_pipeline) override;

			void apply_constant_buffers(bc_device_pipeline* p_pipeline) override;

			void apply_sampler_states(bc_device_pipeline* p_pipeline) override;

			void apply_shader_resource_views(bc_device_pipeline* p_pipeline) override;

			/*void apply_unordered_access_views(bc_device_pipeline& p_pipeline) override;*/

		private:

		};

		using bc_pixel_stage = bc_platform_pixel_stage< g_current_platform_render_api >;
	}
}