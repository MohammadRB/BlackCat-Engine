// [01/14/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcExport.h"
#include "Graphic/bcRenderApi.h"
#include "Graphic/bcRenderApiInfo.h"
#include "Graphic/PipelineStage/bcPipelineStateVariable.h"
#include "Graphic/PipelineStage/bcPipelineStateArrayVariable.h"
#include "Graphic/Resource/Buffer/bcBuffer.h"

namespace black_cat
{
	namespace graphic
	{
		template<bc_render_api>
		class bc_platform_device_pipeline;
		using bc_device_pipeline = bc_platform_device_pipeline< g_current_render_api >;

		// -- Stream outptu state -----------------------------------------------------------------------------

		class BC_GRAPHIC_DLL bc_stream_output_stage_state
		{
		public:
			bc_stream_output_stage_state();

			bc_stream_output_stage_state(const bc_stream_output_stage_state&) = default;

			bc_stream_output_stage_state(bc_stream_output_stage_state&&) = default;

			~bc_stream_output_stage_state();

			bc_stream_output_stage_state& operator=(const bc_stream_output_stage_state&) = default;

			bc_stream_output_stage_state& operator=(bc_stream_output_stage_state&&) = default;

			void set_to_initial_state();

			void reset_tracking();

			bc_pipeline_state_array_variable< bc_buffer*, bc_render_api_info::number_of_so_streams() >  m_stream_buffers;
			bc_pipeline_state_array_variable< bcUINT, bc_render_api_info::number_of_so_streams()> m_stream_offsets;

		protected:

		private:

		};

		// -- Stream output stage -----------------------------------------------------------------------------

		template< bc_render_api TRenderApi >
		struct bc_platform_stream_output_stage_pack
		{
		};

		template< bc_render_api TRenderApi >
		class bc_platform_stream_output_stage : public core_platform::bc_no_copy
		{
		public:
			using platform_pack = bc_platform_stream_output_stage_pack<TRenderApi>;

		public:
			bc_platform_stream_output_stage();

			bc_platform_stream_output_stage(bc_platform_stream_output_stage&&);

			~bc_platform_stream_output_stage();

			bc_platform_stream_output_stage& operator=(bc_platform_stream_output_stage&&);

			void apply_required_state(bc_device_pipeline* p_pipeline);

			void set_to_default_state(bc_device_pipeline* p_pipeline);

			bc_stream_output_stage_state& get_required_state()
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
			bc_stream_output_stage_state m_required_state;
		};

		using bc_stream_output_stage = bc_platform_stream_output_stage<g_current_render_api>;
	}
}