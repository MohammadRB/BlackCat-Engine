// [01/19/2016 MRB]

#pragma once

#include "Core/Math/bcVector4f.h"
#include "Graphic/GraphicPCH.h"
#include "Graphic/bcRenderApi.h"
#include "Graphic/bcDeviceReference.h"
#include "Graphic/bcDeviceRef.h"
#include "Graphic/Device/bcDevice.h"
#include "Graphic/Device/bcDevicePipelineState.h"
#include "Graphic/Device/bcDeviceComputeState.h"
#include "Graphic/Device/bcDeviceInfo.h"
#include "Graphic/Device/Command/bcDeviceCommandList.h"
#include "Graphic/Device/bcDeviceOcclusionQuery.h"
#include "Graphic/Shader/Parameter/bcConstantBufferParameter.h"
#include "Graphic/Shader/Parameter/bcSamplerParameter.h"
#include "Graphic/Shader/Parameter/bcResourceViewParameter.h"
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

namespace black_cat
{
	namespace graphic
	{
		enum class bc_pipeline_type
		{
			default, deferred
		};
		
		template<bc_render_api TRenderApi>
		struct bc_platform_device_pipeline_pack
		{
		};

		template<bc_render_api TRenderApi>
		class bc_platform_device_pipeline : public bc_platform_device_reference<TRenderApi>
		{
		public:
			using platform_pack = bc_platform_device_pipeline_pack<TRenderApi>;

		public:
			bc_platform_device_pipeline() noexcept;

			bc_platform_device_pipeline(platform_pack& p_pack) noexcept;

			bc_platform_device_pipeline(const bc_platform_device_pipeline&) noexcept;

			~bc_platform_device_pipeline() override;

			bc_platform_device_pipeline& operator=(const bc_platform_device_pipeline&) noexcept;

			/**
			 * \brief Bind and Apply required pipeline states.
			 * \param p_state
			 */
			void bind_pipeline_state(bc_device_pipeline_state p_state);

			void unbind_pipeline_state();

			/**
			 * \brief Bind and Apply required pipeline states.
			 * \param p_state
			 */
			void bind_compute_state(bc_device_compute_state p_state);

			void unbind_compute_state();

			void bind_ia_primitive_topology(bc_primitive p_primitive);

			void unbind_ia_primitive_topology();

			void bind_ia_index_buffer(bc_buffer p_buffer, bc_format p_format);

			void unbind_ia_index_buffer();

			void bind_ia_vertex_buffers(bcUINT p_start_slot, bcUINT p_buffer_count, const bc_buffer* p_buffers, bcUINT* p_strides, bcUINT* p_offsets);

			void unbind_ia_vertex_buffers(bcUINT p_start_slot, bcUINT p_buffer_count);

			void bind_ps_constant_buffer_parameter(const bc_constant_buffer_parameter& p_parameter);

			void unbind_ps_constant_buffer_parameter(const bc_constant_buffer_parameter& p_parameter);

			void bind_ps_sampler_parameter(const bc_sampler_parameter& p_parameter);

			void unbind_ps_sampler_parameter(const bc_sampler_parameter& p_parameter);

			void bind_ps_shader_view_parameter(const bc_resource_view_parameter& p_parameter);

			void unbind_ps_shader_view_parameter(const bc_resource_view_parameter& p_parameter);

			void bind_os_stream_outputs(bcUINT p_buffer_count, const bc_buffer* p_buffers, bcUINT* p_offsets);

			void unbind_os_stream_outputs();

			void bind_rs_viewports(bcUINT p_count, const bc_viewport* p_viewports);

			void unbind_rs_viewports();

			void bind_om_blend_factors(const core::bc_vector4f& p_factors);

			void bind_om_stencil_ref(bcUINT32 p_stencil_ref);

			void bind_om_render_targets(bcUINT p_target_count, const bc_render_target_view* p_targets, bc_depth_stencil_view p_depth);

			void unbind_om_render_targets();

			void pipeline_apply_states(bc_pipeline_stage p_stages, const bc_compute_stage::initial_counts_array* p_initial_counts = nullptr);

			void pipeline_set_default_states(bc_pipeline_stage p_stages);

			void draw(bcUINT p_start_vertex, bcUINT p_vertex_count);

			void draw_instanced(bcUINT p_vertex_count_per_instance, bcUINT p_instance_count, bcUINT p_start_vertex_location, bcUINT p_start_instance_location);

			void draw_instanced_indirect(bc_buffer p_args_buffer, bcUINT p_offset);
			
			void draw_indexed(bcUINT p_start_index, bcUINT p_index_count, bcINT p_vertex_offset);

			void draw_indexed_instanced(bcUINT p_index_count_per_instance, bcUINT p_instance_count, bcUINT p_start_index_location, bcINT p_base_vertex_location, bcUINT p_start_instance_location);
			
			void draw_indexed_instanced_indirect(bc_buffer p_args_buffer, bcUINT p_offset);

			void dispatch(bcUINT p_x, bcUINT p_y, bcUINT p_z);

			void dispatch_indirect(bc_buffer p_args, bcUINT p_offset);

			void clear_buffers(const core::bc_vector4f* p_color, bcUINT32 p_count, bcFLOAT p_depth = 1.0f, bcUINT p_stencil = 0);

			void update_subresource(bci_resource& p_resource, bcUINT p_dst_subresource, const void *p_src_data, bcUINT p_src_row_pitch, bcUINT p_src_depth_pitch);

			void copy_subresource(bci_resource& p_dest_resource, bcUINT p_dst_subresource, bci_resource& p_src_resource, bcUINT p_src_subresource);

			void copy_resource(bci_resource& p_dest_resource, bci_resource& p_src_resource);

			void copy_structure_count(bc_buffer p_dest_resource, bcUINT p_offset, bc_resource_view p_unordered_resource);

			void resolve_subresource(bci_resource& p_dest_resource,
				bcUINT p_dest_subresource,
				bci_resource& p_src_resource,
				bcUINT p_src_subresource,
				bc_format p_format);

			/**
			 * \brief Start capturing command lists and put pipeline to lock state
			 */
			void start_command_list();
			
			/**
			 * \brief Write commands to command list and reset pipeline state and lock
			 * \param p_command_list
			 */
			void finish_command_list(bc_device_command_list& p_command_list);

			void start_query(bc_device_occlusion_query& p_query);

			void end_query(bc_device_occlusion_query& p_query);

			/**
			 * \brief Get the result of an occlusion query if it is ready otherwise return false
			 * \param p_query 
			 * \return true if result is ready otherwise false
			 */
			std::pair<bool, bcUINT64> get_query_data(bc_device_occlusion_query& p_query);
			
			void start_monitoring_pipeline();

			bc_device_pipeline_statistic end_monitoring_pipeline();

			bool is_valid() const noexcept override;

			void set_debug_name(const bcCHAR* p_name) noexcept override;

			bc_pipeline_type get_type() const noexcept;
			
			bool operator==(const bc_platform_device_pipeline& p_other) const noexcept;

			bool operator!=(const bc_platform_device_pipeline& p_other) const noexcept;

			bool operator==(std::nullptr_t) const noexcept;

			bool operator!=(std::nullptr_t) const noexcept;

			platform_pack& get_platform_pack() noexcept override
			{
				return m_pack;
			}

			const platform_pack& get_platform_pack() const noexcept override
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;
		};

		using bc_device_pipeline = bc_platform_device_pipeline<g_current_render_api>;
		using bc_device_pipeline_ref = bc_device_ref<bc_device_pipeline>;
	}
}