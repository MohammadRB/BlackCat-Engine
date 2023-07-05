// [02/09/2016 MRB]

#pragma once

#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Device/Query/bcDeviceClockQuery.h"
#include "GraphicImp/Device/Query/bcDeviceTimeStampQuery.h"
#include "GraphicImp/Device/Query/bcDeviceOcclusionQuery.h"
#include "GraphicImp/Device/Command/bcDeviceCommandList.h"
#include "GraphicImp/Device/Command/bcDeviceCommandExecutor.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "GraphicImp/Resource/View/bcDepthStencilView.h"
#include "Game/bcExport.h"
#include "Game/System/Render/State/bcRenderPassState.h"
#include "Game/System/Render/State/bcRenderState.h"
#include "Game/System/Render/State/bcComputeState.h"

namespace black_cat::game
{
	class bc_render_system;

	class BC_GAME_DLL bc_render_thread
	{
	public:
		bc_render_thread() noexcept;

		bc_render_thread(bc_render_thread&&) noexcept;

		~bc_render_thread() noexcept;

		bc_render_thread& operator=(bc_render_thread&&) noexcept;

		graphic::bc_device_pipeline get_pipeline() noexcept
		{
			return m_pipeline.get();
		}

		graphic::bc_device_command_executor get_executor() noexcept
		{
			return m_executor.get();
		}

		/**
			 * \brief Bind render pass states to given device pipeline and apply changes
			 * Apply RasterizerStage for ViewPort
			 * Apply OutputMergerStage for RenderTargets and DepthStencil
			 * Apply ProgrammableStates for parameters
			 * \param p_render_pass_state 
			 */
		void bind_render_pass_state(const bc_render_pass_state& p_render_pass_state);

		void unbind_render_pass_state(const bc_render_pass_state& p_render_pass_state);

		/**
			 * \brief Bind render states to given device pipeline and apply changes
			 * Apply InputAssemblerStage for primitive topology, vertex buffer and index buffer
			 * Apply ProgrammableStates for parameters
			 * \param p_render_state
			 * \param p_stencil_ref 
			 */
		void bind_render_state(const bc_render_state& p_render_state, const bcUINT32* p_stencil_ref = nullptr);

		void unbind_render_state(const bc_render_state& p_render_state);

		/**
			 * \brief Bind compute states to given device pipeline and apply changes
			 * Apply ComputeStage for compute shader and parameters
			 * \param p_compute_state
			 * \param p_uav_initial_counts 
			 */
		void bind_compute_state(const bc_compute_state& p_compute_state, const bc_compute_state_unordered_view_initial_count_array* p_uav_initial_counts = nullptr);

		void unbind_compute_state(const bc_compute_state& p_compute_state);

		/*void bind_ia_primitive_topology(graphic::bc_primitive p_primitive);

			void bind_ia_index_buffer(graphic::bc_buffer* p_buffer, graphic::bc_format p_format);

			void unbind_ia_index_buffer();

			void bind_ia_vertex_buffers(bcUINT p_start_slot, bcUINT p_buffer_count, graphic::bc_buffer* p_buffers, bcUINT* p_strides, bcUINT* p_offsets);

			void unbind_ia_vertex_buffers(bcUINT p_start_slot, bcUINT p_buffer_count);*/

		void bind_ps_constant_buffer_parameter(const graphic::bc_constant_buffer_parameter& p_parameter);

		void unbind_ps_constant_buffer_parameter(const graphic::bc_constant_buffer_parameter& p_parameter);

		void bind_ps_sampler_parameter(const graphic::bc_sampler_parameter& p_parameter);

		void unbind_ps_sampler_parameter(const graphic::bc_sampler_parameter& p_parameter);

		void bind_ps_shader_view_parameter(const graphic::bc_resource_view_parameter& p_parameter);

		void unbind_ps_shader_view_parameter(const graphic::bc_resource_view_parameter& p_parameter);

		/*void bind_os_stream_outputs(bcUINT p_buffer_count, graphic::bc_buffer* p_buffers, bcUINT* p_offsets);

			void unbind_os_stream_outputs();

			void bind_rs_viewports(bcUINT p_count, graphic::bc_viewport* p_viewports);

			void unbind_rs_viewports();*/

		void bind_om_blend_factors(const core::bc_vector4f& p_factors);

		void bind_om_stencil_ref(bcUINT32 p_stencil_ref);

		/*void bind_om_render_targets(bcUINT p_target_count, graphic::bc_render_target_view* p_targets, graphic::bc_depth_stencil_view* p_depth);

			void unbind_om_render_targets();*/

		void pipeline_apply_states(graphic::bc_pipeline_stage p_stages);

		void pipeline_set_default_states(graphic::bc_pipeline_stage p_stages);

		void draw(bcUINT p_start_vertex, bcUINT p_vertex_count);

		void draw_instanced(bcUINT p_vertex_count_per_instance, bcUINT p_instance_count, bcUINT p_start_vertex_location, bcUINT p_start_instance_location);

		void draw_instanced_indirect(graphic::bc_buffer p_args_buffer, bcUINT p_offset);
			
		void draw_indexed(bcUINT p_start_index, bcUINT p_index_count, bcINT p_vertex_offset);

		void draw_indexed_instanced(bcUINT p_index_count_per_instance, bcUINT p_instance_count, bcUINT p_start_index_location, bcINT p_base_vertex_location, bcUINT p_start_instance_location);

		void draw_indexed_instanced_indirect(graphic::bc_buffer p_args_buffer, bcUINT p_offset);

		void dispatch(bcUINT p_x, bcUINT p_y, bcUINT p_z);

		void dispatch_indirect(graphic::bc_buffer p_args, bcUINT p_offset);

		void clear_buffers(const core::bc_vector4f* p_color, bcUINT32 p_count, bcFLOAT p_depth = 1.0f, bcUINT p_stencil = 0);

		void clear_render_target_view(graphic::bc_render_target_view p_view, const bcFLOAT* p_color);

		void clear_depth_stencil_view(graphic::bc_depth_stencil_view p_view, bcFLOAT p_depth = 1.0f, bcUINT8 p_stencil = 0);

		/*graphic::bc_mapped_resource map_resource(graphic::bc_iresource& p_resource, bcUINT p_subresource, graphic::bc_resource_map p_map_type);

			void unmap_resource(graphic::bc_iresource& p_resource, bcUINT p_subresource);*/

		void update_subresource(graphic::bci_resource& p_resource, bcUINT p_dst_subresource, const void *p_src_data, bcUINT p_src_row_pitch, bcUINT p_src_depth_pitch);

		void copy_subresource(graphic::bci_resource& p_dest_resource, bcUINT p_dst_subresource, graphic::bci_resource& p_src_resource, bcUINT p_src_subresource);

		void copy_resource(graphic::bci_resource& p_dest_resource, graphic::bci_resource& p_src_resource);

		void copy_structure_count(graphic::bc_buffer p_dest_resource, bcUINT p_offset, graphic::bc_resource_view p_unordered_resource);

		void resolve_subresource(graphic::bci_resource& p_dest_resource,
		                         bcUINT p_dest_subresource,
		                         graphic::bci_resource& p_src_resource,
		                         bcUINT p_src_subresource,
		                         graphic::bc_format p_format);

		void generate_mips(graphic::bc_resource_view& p_resource);

		/**
			 * \brief Start thread to capture device pipeline commands and save them into the passed command list
			 * \param p_command_list
			 */
		void start(graphic::bc_device_command_list p_command_list) noexcept;

		/**
			 * \brief save device pipeline commands in command list and execute command list
			 * \return command list passed into when running new thread
			 */
		graphic::bc_device_command_list finish() noexcept;

		void start_query(graphic::bc_device_clock_query& p_query);

		void end_query(graphic::bc_device_clock_query& p_query);
			
		void end_query(graphic::bc_device_timestamp_query& p_query);

		void start_query(graphic::bc_device_occlusion_query& p_query);

		void end_query(graphic::bc_device_occlusion_query& p_query);

		std::pair<bool, bcUINT64> get_query_data(graphic::bc_device_clock_query& p_query);

		std::pair<bool, bcUINT64> get_query_data(graphic::bc_device_timestamp_query& p_query);

		std::pair<bool, bcUINT64> get_query_data(graphic::bc_device_occlusion_query& p_query);
			
		void reset();

		void reset(graphic::bc_device_pipeline_ref p_pipeline, graphic::bc_device_command_executor_ref p_command_executor);

	private:
		graphic::bc_device_pipeline_ref m_pipeline;
		graphic::bc_device_command_executor_ref m_executor;
		graphic::bc_device_command_list m_command_list;
	};
}
