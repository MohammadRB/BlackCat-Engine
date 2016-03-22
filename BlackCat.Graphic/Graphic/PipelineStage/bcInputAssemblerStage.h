// [01/12/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcPlatformRenderApi.h"
#include "Graphic/bcGraphicDefinition.h"
#include "Graphic/bcRenderApiInfo.h"
#include "Graphic/PipelineStage/bcPipelineStateVariable.h"
#include "Graphic/PipelineStage/bcPipelineStateArrayVariable.h"
#include "Graphic/Resource/Buffer/bcBuffer.h"

namespace black_cat
{
	namespace graphic
	{
		template<bc_platform_render_api>
		class bc_platform_device_pipeline;
		using bc_device_pipeline = bc_platform_device_pipeline< g_current_platform_render_api >;

		// -- Input assembler state -----------------------------------------------------------------------------

		class BC_GRAPHIC_DLL_EXP bc_input_assembler_stage_state
		{
		public:
			bc_input_assembler_stage_state();

			bc_input_assembler_stage_state(const bc_input_assembler_stage_state&) = default;

			bc_input_assembler_stage_state(bc_input_assembler_stage_state&&) = default;

			~bc_input_assembler_stage_state();

			bc_input_assembler_stage_state& operator=(const bc_input_assembler_stage_state&) = default;

			bc_input_assembler_stage_state& operator=(bc_input_assembler_stage_state&&) = default;

			void set_to_initial_state();

			void reset_tracking();

			bc_pipeline_state_variable< bc_buffer* > m_index_buffer;
			bc_pipeline_state_variable< bc_format > m_index_buffer_format;
			bc_pipeline_state_array_variable< bc_buffer*, bc_render_api_info::number_of_ia_vertex_buffers() > m_vertex_buffers;
			bc_pipeline_state_array_variable< bcUINT, bc_render_api_info::number_of_ia_vertex_buffers() > m_vertex_buffers_strides;
			bc_pipeline_state_array_variable< bcUINT, bc_render_api_info::number_of_ia_vertex_buffers() > m_vertex_buffers_offsets;
			//bc_pipeline_state_variable< bc_input_layout* > m_input_layout;
			bc_pipeline_state_variable< bc_primitive > m_primitive_topology;

		protected:

		private:

		};

		// -- Input assembler stage -----------------------------------------------------------------------------

		template< bc_platform_render_api TRenderApi >
		struct bc_platform_input_assembler_stage_pack
		{
		};

		template< bc_platform_render_api TRenderApi >
		class bc_platform_input_assembler_stage : public core_platform::bc_no_copy
		{
		public:
			using platform_pack = bc_platform_input_assembler_stage_pack<TRenderApi>;

		public:
			bc_platform_input_assembler_stage();

			bc_platform_input_assembler_stage(bc_platform_input_assembler_stage&&);

			~bc_platform_input_assembler_stage();

			bc_platform_input_assembler_stage& operator=(bc_platform_input_assembler_stage&&);

			void apply_required_state(bc_device_pipeline* p_pipeline);

			void set_to_default_state(bc_device_pipeline* p_pipeline);
			
			bc_input_assembler_stage_state& get_required_state()
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

			bc_input_assembler_stage_state m_required_state;
		};

		using bc_input_assembler_stage = bc_platform_input_assembler_stage<g_current_platform_render_api>;
	}
}
