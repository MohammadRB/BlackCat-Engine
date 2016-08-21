// [01/14/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcExport.h"
#include "Graphic/bcPlatformRenderApi.h"
#include "Graphic/bcRenderApiInfo.h"
#include "Graphic/PipelineStage/bcPipelineStateVariable.h"
#include "Graphic/PipelineStage/bcPipelineStateArrayVariable.h"
#include "Graphic/Resource/Buffer/bcBuffer.h"
#include "Graphic/Resource/State/bcSamplerState.h"
#include "Graphic/Resource/View/bcShaderView.h"

namespace black_cat
{
	namespace graphic
	{
		template<bc_platform_render_api>
		class bc_platform_device_pipeline;
		using bc_device_pipeline = bc_platform_device_pipeline< g_current_platform_render_api >;

		// -- Programmable state --------------------------------------------------------------------------------

		class BC_GRAPHIC_DLL bc_programmable_stage_state
		{
		public:
			bc_programmable_stage_state();

			bc_programmable_stage_state(const bc_programmable_stage_state&) = default;

			bc_programmable_stage_state(bc_programmable_stage_state&&) = default;

			~bc_programmable_stage_state();

			bc_programmable_stage_state& operator=(const bc_programmable_stage_state&) = default;

			bc_programmable_stage_state& operator=(bc_programmable_stage_state&&) = default;

			void set_to_initial_state();

			void reset_tracking();

			//bc_pipeline_state_variable< bc_ishader* > m_shader_program;
			bc_pipeline_state_array_variable< bc_buffer*, bc_render_api_info::number_of_shader_constant_buffer()>  m_constant_buffers;
			bc_pipeline_state_array_variable< bc_sampler_state*, bc_render_api_info::number_of_shader_sampler() > m_sampler_states;
			bc_pipeline_state_array_variable< bc_shader_view*, bc_render_api_info::number_of_shader_resource()> m_shader_resource_views;
			bc_pipeline_state_array_variable< bc_shader_view*, bc_render_api_info::number_of_ps_cs_uav_registers() > m_unordered_access_views;
			/*bc_pipeline_state_array_variable< bcUINT > m_uav_initial_counts;*/

		protected:

		private:

		};

		// -- Programmable stage --------------------------------------------------------------------------------

		template< bc_platform_render_api TRenderApi >
		struct bc_platform_programmable_stage_pack
		{
		};

		template< bc_platform_render_api TRenderApi >
		class bc_platform_programmable_stage : public core_platform::bc_no_copy
		{
		public:
			using platform_pack = bc_platform_programmable_stage_pack<TRenderApi>;

		public:
			bc_platform_programmable_stage();

			bc_platform_programmable_stage(bc_platform_programmable_stage&&);

			~bc_platform_programmable_stage();

			bc_platform_programmable_stage& operator=(bc_platform_programmable_stage&&);

			void apply_required_state(bc_device_pipeline* p_pipeline);

			void set_to_default_state(bc_device_pipeline* p_pipeline);

			bc_programmable_stage_state& get_required_state()
			{
				return m_required_state;
			}

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:
			virtual void apply_shader_program(bc_device_pipeline* p_pipeline) = 0;

			virtual void apply_constant_buffers(bc_device_pipeline* p_pipeline) = 0;

			virtual void apply_sampler_states(bc_device_pipeline* p_pipeline) = 0;

			virtual void apply_shader_resource_views(bc_device_pipeline* p_pipeline) = 0;

			/*virtual void apply_unordered_access_views(bc_device_pipeline& p_pipeline) = 0;*/

			platform_pack m_pack;
			bc_programmable_stage_state m_required_state;

		private:
		};

		using bc_programmable_stage = bc_platform_programmable_stage<g_current_platform_render_api>;
	}
}