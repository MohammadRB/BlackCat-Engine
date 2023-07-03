// [14/01/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcRenderApi.h"
#include "Graphic/PipelineStage/bcProgrammableStage.h"

namespace black_cat
{
	namespace graphic
	{
		template<bc_render_api TRenderApi>
		class bc_platform_geometry_stage : public bc_programmable_stage
		{
		public:
			bc_platform_geometry_stage() noexcept;

			bc_platform_geometry_stage(bc_platform_geometry_stage&&) noexcept;

			~bc_platform_geometry_stage() override;

			bc_platform_geometry_stage& operator=(bc_platform_geometry_stage&&) noexcept;

		protected:
			void apply_shader_program(bc_device_pipeline& p_pipeline) override;

			void apply_constant_buffers(bc_device_pipeline& p_pipeline) override;

			void apply_sampler_states(bc_device_pipeline& p_pipeline) override;

			void apply_shader_resource_views(bc_device_pipeline& p_pipeline) override;

			/*void apply_unordered_access_views(bc_device_pipeline& p_pipeline) override;*/
		};

		using bc_geometry_stage = bc_platform_geometry_stage<g_current_render_api>;
	}
}