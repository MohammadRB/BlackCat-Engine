// [01/13/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcPlatformRenderApi.h"
#include "Graphic/Math/bcVector4f.h"
#include "Graphic/bcRenderApiInfo.h"
#include "Graphic/PipelineStage/bcPipelineStateVariable.h"
#include "Graphic/PipelineStage/bcPipelineStateArrayVariable.h"
#include "Graphic/Resource/View/bcShaderView.h"
#include "Graphic/Resource/View/bcRenderTargetView.h"
#include "Graphic/Resource/View/bcDepthStencilView.h"

namespace black_cat
{
	namespace graphic
	{
		template<bc_platform_render_api>
		class bc_platform_device_pipeline;
		using bc_device_pipeline = bc_platform_device_pipeline< g_current_platform_render_api >;

		// -- Output merger state -----------------------------------------------------------------------------

		class BC_GRAPHIC_DLL_EXP bc_output_merger_stage_state
		{
		public:
			bc_output_merger_stage_state();

			bc_output_merger_stage_state(const bc_output_merger_stage_state&) = default;

			bc_output_merger_stage_state(bc_output_merger_stage_state&&) = default;

			~bc_output_merger_stage_state();

			bc_output_merger_stage_state& operator=(const bc_output_merger_stage_state&) = default;

			bc_output_merger_stage_state& operator=(bc_output_merger_stage_state&&) = default;

			void set_to_initial_state();

			void reset_tracking();

			bcUINT associated_render_target_count() const;

			//bc_pipeline_state_variable< bc_blend_state* > m_blend_state;
			//bc_pipeline_state_variable< bc_depth_stencil_state* > m_depth_stencil_state;
			bc_pipeline_state_variable< bc_vector4f > m_blend_factors;
			bc_pipeline_state_variable< bcUINT > m_stencil_ref;
			bc_pipeline_state_array_variable< bc_render_target_view*, bc_render_api_info::number_of_om_render_target_slots() > m_render_target_views;
			bc_pipeline_state_variable< bc_depth_stencil_view* > m_depth_target_view;
			bc_pipeline_state_array_variable< bc_shader_view*, bc_render_api_info::number_of_ps_cs_uav_registers() > m_unordered_access_views;
			//bc_pipeline_state_array_variable< bcUINT > m_uav_initial_counts;

		protected:

		private:

		};

		// -- Output merger stage -----------------------------------------------------------------------------

		template< bc_platform_render_api TRenderApi >
		struct bc_platform_output_merger_stage_pack
		{
		};

		template< bc_platform_render_api TRenderApi >
		class bc_platform_output_merger_stage : public core_platform::bc_no_copy
		{
		public:
			using platform_pack = bc_platform_output_merger_stage_pack<TRenderApi>;

		public:
			bc_platform_output_merger_stage();

			bc_platform_output_merger_stage(bc_platform_output_merger_stage&&);

			~bc_platform_output_merger_stage();

			bc_platform_output_merger_stage& operator=(bc_platform_output_merger_stage&&);

			void apply_required_state(bc_device_pipeline* p_pipeline);

			void set_to_default_state(bc_device_pipeline* p_pipeline);

			bc_output_merger_stage_state& get_required_state()
			{
				return m_required_state;
			}

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			platform_pack m_pack;

			bc_output_merger_stage_state m_required_state;
		};

		using bc_output_merger_stage = bc_platform_output_merger_stage<g_current_platform_render_api>;
	}
}
