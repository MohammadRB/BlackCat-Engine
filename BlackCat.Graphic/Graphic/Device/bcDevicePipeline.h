// [01/19/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcPlatformRenderApi.h"
#include "Graphic/bcDeviceObject.h"
#include "Graphic/bcResourcePtr.h"
#include "Graphic/Device/bcDevice.h"
#include "Graphic/Device/bcDevicePipelineState.h"
#include "Graphic/Device/bcDeviceInfo.h"
#include "Graphic/Device/Command/bcDeviceCommandList.h"
#include "Graphic/Shader/Parameter/bcConstantBufferParameter.h"
#include "Graphic/Shader/Parameter/bcSamplerParameter.h"
#include "Graphic/Shader/Parameter/bcShaderResourceParameter.h"
#include "Graphic/PipelineStage/bcInputAssemblerStage.h"
#include "Graphic/PipelineStage/bcStreamOutputStage.h"
#include "Graphic/PipelineStage/bcRasterizerStage.h"
#include "Graphic/PipelineStage/bcOutputMergerStage.h"
#include "Graphic/PipelineStage/bcVertexStage.h"
#include "Graphic/PipelineStage/bcHullStage.h"
#include "Graphic/PipelineStage/bcDomainStage.h"
#include "Graphic/PipelineStage/bcGeometryStage.h"
#include "Graphic/PipelineStage/bcPixelStage.h"
#include "Graphic/PipelineStage/bcComputeStage.h"
#include "Graphic/Resource/bcResource.h"
#include "Graphic/Math/bcVector4f.h"

namespace black_cat
{
	namespace graphic
	{
		template< bc_platform_render_api TRenderApi >
		struct bc_platform_device_pipeline_pack
		{
		};

		template< bc_platform_render_api TRenderApi >
		class bc_platform_device_pipeline : public bc_device_object
		{
		public:
			using platform_pack = bc_platform_device_pipeline_pack<TRenderApi>;

		public:
			bc_platform_device_pipeline(bc_device& p_device);

			bc_platform_device_pipeline(bc_platform_device_pipeline&&);

			~bc_platform_device_pipeline();

			bc_platform_device_pipeline& operator=(bc_platform_device_pipeline&&);

			void bind_pipeline_state(bc_device_pipeline_state* p_state);

			void bind_ia_primitive_topology(bc_primitive p_primitive);

			void bind_ia_index_buffer(bc_buffer* p_buffer, bc_format p_format);

			void bind_ia_vertex_buffers(bcUINT p_start_slot, bcUINT p_buffer_count, bc_buffer* p_buffers, bcUINT* p_strides, bcUINT* p_offsets);

			void bind_ps_constant_buffer_parameter(bc_shader_type p_shader, const bc_constant_buffer_parameter& p_parameter);

			void bind_ps_sampler_parameter(bc_shader_type p_shader, const bc_sampler_parameter& p_parameter);

			void bind_ps_shader_resource_parameter(bc_shader_type p_shader, const bc_shader_resource_parameter& p_parameter);

			void bind_os_stream_outputs(bcUINT p_buffer_count, bc_buffer* p_buffers, bcUINT* p_offsets);

			void bind_rs_viewports(bcUINT p_count, bc_viewport* p_viewports);

			void bind_om_blend_factors(bc_vector4f l_factors);

			void bind_om_stencil_ref(bcUINT32 l_stencil_ref);

			void bind_om_render_targets(bcUINT p_target_count, bc_render_target_view* p_targets, bc_depth_stencil_view* p_depth);

			void pipeline_apply_states(bc_pipeline_stage p_stages);

			void pipeline_set_default_states(bc_pipeline_stage p_stages);

			void draw(bcUINT p_start_vertex, bcUINT p_vertex_count);

			void draw_indexed(bcUINT p_start_index, bcUINT p_index_count, bcINT p_vertex_offset);

			void draw_indexed_instanced(bcUINT p_index_count_per_instance, bcUINT p_instance_count, bcUINT p_start_index_location, bcINT p_base_vertex_location, bcUINT p_start_instance_location);
			
			void draw_indexed_instanced_indirect(bc_buffer* p_args_buffer, bcUINT p_offset);

			void dispatch(bcUINT p_x, bcUINT p_y, bcUINT p_z);

			void dispatch_indirect(bc_buffer* p_args, bcUINT p_offset);

			void clear_buffers(bc_vector4f p_color, bcFLOAT32 p_depth = 1.0f, bcUINT p_stencil = 0);

			bc_mapped_resource map_resource(bc_iresource* p_resource, bcUINT p_subresource, bc_resource_map p_map_type);
			
			void unmap_resource(bc_iresource* p_resource, bcUINT p_subresource);

			void update_subresource(bc_iresource* p_resource, bcUINT p_dst_subresource, const void *p_src_data, bcUINT p_src_row_pitch, bcUINT p_src_depth_pitch);

			void copy_subresource(bc_iresource* p_dest_resource, bcUINT p_dst_subresource, bc_iresource* p_src_resource, bcUINT p_src_subresource);

			void copy_resource(bc_iresource* p_dest_resource, bc_iresource* p_src_resource);

			void copy_structure_count(bc_buffer* p_dest_resource, bcUINT p_offset, bc_shader_view* p_unordered_resource);

			void resolve_subresource(bc_iresource* p_dest_resource,
				bcUINT p_dest_subresource,
				bc_iresource* p_src_resource,
				bcUINT p_src_subresource,
				bc_format p_format);

			void finish_command_list(bc_device_command_list* p_command_list);

			void start_monitoring_pipeline();

			bc_device_pipeline_statistic end_monitoring_pipeline();

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			bc_device& m_device;

			bc_input_assembler_stage m_input_assembler_stage;
			bc_stream_output_stage m_stream_output_stage;
			bc_rasterizer_stage m_rasterizer_stage;
			bc_output_merger_stage m_output_merger_stage;
			bc_vertex_stage	m_vertex_shader_stage;
			bc_hull_stage m_hull_shader_stage;
			bc_domain_stage	m_domain_shader_stage;
			bc_geometry_stage m_geometry_shader_stage;
			bc_pixel_stage m_pixel_shader_stage;
			bc_compute_stage m_compute_shader_stage;

			bc_programmable_stage* m_shader_stages[6];

			platform_pack m_pack;
		};

		using bc_device_pipeline = bc_platform_device_pipeline< g_current_platform_render_api >;

		using bc_device_pipeline_ptr = bc_resource_ptr< bc_device_pipeline >;
	}
}
