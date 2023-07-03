// [12/01/2016 MRB]

#pragma once

#include "Graphic/bcExport.h"
#include "Graphic/bcRenderApi.h"
#include "Graphic/bcGraphicDefinition.h"
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
		using bc_device_pipeline = bc_platform_device_pipeline<g_current_render_api>;

		class bc_input_assembler_stage_state
		{
		public:
			bc_input_assembler_stage_state() noexcept;

			bc_input_assembler_stage_state(const bc_input_assembler_stage_state&) noexcept = default;

			bc_input_assembler_stage_state(bc_input_assembler_stage_state&&) noexcept = default;

			~bc_input_assembler_stage_state();

			bc_input_assembler_stage_state& operator=(const bc_input_assembler_stage_state&) noexcept = default;

			bc_input_assembler_stage_state& operator=(bc_input_assembler_stage_state&&) noexcept = default;

			void set_to_initial_state() noexcept;

			void reset_tracking() noexcept;

			bc_pipeline_state_variable<bc_buffer> m_index_buffer;
			bc_pipeline_state_variable<bc_format> m_index_buffer_format;
			bc_pipeline_state_array_variable<bc_buffer, bc_render_api_info::number_of_ia_vertex_buffers()> m_vertex_buffers;
			bc_pipeline_state_array_variable<bcUINT, bc_render_api_info::number_of_ia_vertex_buffers()> m_vertex_buffers_strides;
			bc_pipeline_state_array_variable<bcUINT, bc_render_api_info::number_of_ia_vertex_buffers()> m_vertex_buffers_offsets;
			//bc_pipeline_state_variable<bc_input_layout*> m_input_layout;
			bc_pipeline_state_variable<bc_primitive> m_primitive_topology;
		};

		inline bc_input_assembler_stage_state::bc_input_assembler_stage_state() noexcept
			: m_index_buffer(bc_buffer()),
			m_index_buffer_format(bc_format::R32_UINT),
			m_vertex_buffers(bc_buffer()),
			m_vertex_buffers_strides(0),
			m_vertex_buffers_offsets(0),
			//m_input_layout(nullptr),
			m_primitive_topology(bc_primitive::undefined)

		{
		}

		inline bc_input_assembler_stage_state::~bc_input_assembler_stage_state() = default;

		inline void bc_input_assembler_stage_state::set_to_initial_state() noexcept
		{
			m_index_buffer.set_to_initial_state();
			m_index_buffer_format.set_to_initial_state();
			m_vertex_buffers.set_to_initial_state();
			m_vertex_buffers_strides.set_to_initial_state();
			m_vertex_buffers_offsets.set_to_initial_state();
			//m_input_layout.set_to_initial_state();
			m_primitive_topology.set_to_initial_state();
		}

		inline void bc_input_assembler_stage_state::reset_tracking() noexcept
		{
			m_index_buffer.reset_tracking();
			m_index_buffer_format.reset_tracking();
			m_vertex_buffers.reset_tracking();
			m_vertex_buffers_strides.reset_tracking();
			m_vertex_buffers_offsets.reset_tracking();
			//m_input_layout.reset_tracking();
			m_primitive_topology.reset_tracking();
		}

		template<bc_render_api TRenderApi>
		struct bc_platform_input_assembler_stage_pack
		{
		};

		template<bc_render_api TRenderApi>
		class bc_platform_input_assembler_stage : public platform::bc_no_copy
		{
		public:
			using platform_pack = bc_platform_input_assembler_stage_pack<TRenderApi>;

		public:
			bc_platform_input_assembler_stage(platform_pack p_pack) noexcept;

			bc_platform_input_assembler_stage(bc_platform_input_assembler_stage&&) noexcept;

			~bc_platform_input_assembler_stage();

			bc_platform_input_assembler_stage& operator=(bc_platform_input_assembler_stage&&) noexcept;

			void apply_required_state(bc_device_pipeline& p_pipeline);

			void set_to_default_state(bc_device_pipeline& p_pipeline);
			
			bc_input_assembler_stage_state& get_required_state()
			{
				return m_required_state;
			}

			platform_pack& get_platform_pack() noexcept
			{
				return m_pack;
			}

			const platform_pack& get_platform_pack() const noexcept
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;

			bc_input_assembler_stage_state m_required_state;
		};

		using bc_input_assembler_stage = bc_platform_input_assembler_stage<g_current_render_api>;
	}
}