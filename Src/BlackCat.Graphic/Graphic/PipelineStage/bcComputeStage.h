// [01/14/2016 MRB]

#pragma once

#include "Core/Container/bcArray.h"
#include "Graphic/GraphicPCH.h"
#include "Graphic/bcRenderApi.h"
#include "Graphic/PipelineStage/bcProgrammableStage.h"

namespace black_cat
{
	namespace graphic
	{
		template< bc_render_api TRenderApi >
		class bc_platform_compute_stage : public bc_programmable_stage
		{
		public:
			using bc_programmable_stage::apply_required_state;
			using initial_counts_array = core::bc_array<bcINT32, bc_render_api_info::number_of_ps_cs_uav_resource()>;
			
		public:
			bc_platform_compute_stage();

			bc_platform_compute_stage(bc_platform_compute_stage&&) noexcept;

			~bc_platform_compute_stage();

			bc_platform_compute_stage& operator=(bc_platform_compute_stage&&) noexcept;

			void apply_required_state(bc_device_pipeline* p_pipeline, const initial_counts_array& p_initial_counts);
			
		protected:
			void apply_shader_program(bc_device_pipeline* p_pipeline) override;

			void apply_constant_buffers(bc_device_pipeline* p_pipeline) override;

			void apply_sampler_states(bc_device_pipeline* p_pipeline) override;

			void apply_shader_resource_views(bc_device_pipeline* p_pipeline) override;

			/*void apply_unordered_access_views(bc_device_pipeline& p_pipeline) override;*/
		private:
			initial_counts_array m_initial_counts;
		};

		using bc_compute_stage = bc_platform_compute_stage< g_current_render_api >;
	}
}