// [09/02/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/Utility/bcEnumOperand.h"
#include "Game/System/Render/bcRenderThread.h"
#include "Game/System/Render/bcRenderSystem.h"

namespace black_cat
{
	namespace game
	{
		graphic::bc_pipeline_stage _convert_shader_type_to_pipeline_stage(graphic::bc_shader_type p_shader_types)
		{
			auto l_pipeline_stages = core::bc_enum::none< graphic::bc_pipeline_stage >();

			if (core::bc_enum::has(p_shader_types, graphic::bc_shader_type::vertex))
			{
				l_pipeline_stages = core::bc_enum::mask_or
				({
					l_pipeline_stages,
					graphic::bc_pipeline_stage::vertex_stage
				});
			}
			if (core::bc_enum::has(p_shader_types, graphic::bc_shader_type::domain))
			{
				l_pipeline_stages = core::bc_enum::mask_or
				({
					l_pipeline_stages,
					graphic::bc_pipeline_stage::domain_stage
				});
			}
			if (core::bc_enum::has(p_shader_types, graphic::bc_shader_type::hull))
			{
				l_pipeline_stages = core::bc_enum::mask_or
				({
					l_pipeline_stages,
					graphic::bc_pipeline_stage::hull_stage
				});
			}
			if (core::bc_enum::has(p_shader_types, graphic::bc_shader_type::geometry))
			{
				l_pipeline_stages = core::bc_enum::mask_or
				({
					l_pipeline_stages,
					graphic::bc_pipeline_stage::geometry_stage
				});
			}
			if (core::bc_enum::has(p_shader_types, graphic::bc_shader_type::pixel))
			{
				l_pipeline_stages = core::bc_enum::mask_or
				({
					l_pipeline_stages,
					graphic::bc_pipeline_stage::pixel_stage
				});
			}

			return l_pipeline_stages;
		}

		bc_render_thread::bc_render_thread() noexcept
		{
		}

		bc_render_thread::bc_render_thread(bc_render_thread&& p_other) noexcept
			: m_pipeline(std::move(p_other.m_pipeline)),
			m_executor(std::move(p_other.m_executor)),
			m_command_list(p_other.m_command_list)
		{
		}

		bc_render_thread::~bc_render_thread() noexcept = default;

		bc_render_thread& bc_render_thread::operator=(bc_render_thread&& p_other) noexcept
		{
			m_pipeline = std::move(p_other.m_pipeline);
			m_executor = std::move(p_other.m_executor);
			m_command_list = p_other.m_command_list;

			return *this;
		}

		void bc_render_thread::bind_render_pass_state(const bc_render_pass_state& p_render_pass_state)
		{
			bcSIZE l_render_target_count = 0;

			bc_render_pass_state_render_target_view_array l_render_targets;
			graphic::bc_depth_stencil_view l_depth_stencil = p_render_pass_state.m_shader_depth;
			
			std::transform
			(
				std::cbegin(p_render_pass_state.m_shader_targets),
				std::cend(p_render_pass_state.m_shader_targets),
				std::begin(l_render_targets),
				[&l_render_target_count](const graphic::bc_render_target_view& p_target)
				{
					if (p_target != nullptr)
					{
						++l_render_target_count;
					}

					return p_target;
				}
			);

			graphic::bc_pipeline_stage l_pipeline_stages = core::bc_enum::mask_or
			({
				graphic::bc_pipeline_stage::rasterizer_stage,
				graphic::bc_pipeline_stage::output_merger_stage
			});

			m_pipeline->bind_pipeline_state(p_render_pass_state.m_pipeline_state);
			m_pipeline->bind_rs_viewports(1, &p_render_pass_state.m_viewport);
			m_pipeline->bind_om_render_targets(l_render_target_count, l_render_targets.data(), l_depth_stencil);

			graphic::bc_shader_type l_shader_types = core::bc_enum::none< graphic::bc_shader_type >();

			for (const auto& l_sampler_parameter : p_render_pass_state.m_shader_samplers)
			{
				if (l_sampler_parameter.is_valid())
				{
					l_shader_types = core::bc_enum::mask_or({l_shader_types, l_sampler_parameter.get_shader_types()});

					m_pipeline->bind_ps_sampler_parameter(l_sampler_parameter);
				}
			}
			for (const auto& l_view_parameter : p_render_pass_state.m_resource_views)
			{
				if (l_view_parameter.is_valid())
				{
					l_shader_types = core::bc_enum::mask_or({l_shader_types, l_view_parameter.get_shader_types()});

					m_pipeline->bind_ps_shader_view_parameter(l_view_parameter);
				}
			}
			for (const auto& l_view_parameter : p_render_pass_state.m_unordered_views)
			{
				if (l_view_parameter.is_valid())
				{
					l_shader_types = core::bc_enum::mask_or({ l_shader_types, l_view_parameter.get_shader_types() });
					// If there is any unordered view for pixel shader add output merger for apply changes
					if(l_view_parameter.get_shader_types() == graphic::bc_shader_type::pixel)
					{
						l_pipeline_stages = core::bc_enum::mask_or({ l_pipeline_stages ,graphic::bc_pipeline_stage::output_merger_stage });
					}
					
					m_pipeline->bind_ps_shader_view_parameter(l_view_parameter);
				}
			}
			for (const auto& l_buffer_parameter : p_render_pass_state.m_shader_cbuffers)
			{
				if (l_buffer_parameter.is_valid())
				{
					l_shader_types = core::bc_enum::mask_or({l_shader_types, l_buffer_parameter.get_shader_types()});

					m_pipeline->bind_ps_constant_buffer_parameter(l_buffer_parameter);
				}
			}

			l_pipeline_stages = core::bc_enum::mask_or({l_pipeline_stages, _convert_shader_type_to_pipeline_stage(l_shader_types)});

			m_pipeline->pipeline_apply_states(l_pipeline_stages);
		}

		void bc_render_thread::unbind_render_pass_state(const bc_render_pass_state& p_render_pass_state)
		{
			m_pipeline->unbind_pipeline_state();
			m_pipeline->unbind_rs_viewports();
			m_pipeline->unbind_om_render_targets();

			graphic::bc_pipeline_stage l_pipeline_stages = core::bc_enum::mask_or
			({
				graphic::bc_pipeline_stage::rasterizer_stage,
				graphic::bc_pipeline_stage::output_merger_stage
			});
			graphic::bc_shader_type l_shader_types = core::bc_enum::none< graphic::bc_shader_type >();

			for (const auto& l_sampler_parameter : p_render_pass_state.m_shader_samplers)
			{
				if (l_sampler_parameter.is_valid())
				{
					l_shader_types = core::bc_enum::mask_or({l_shader_types, l_sampler_parameter.get_shader_types()});

					m_pipeline->unbind_ps_sampler_parameter(l_sampler_parameter);
				}
			}
			for (const auto& l_view_parameter : p_render_pass_state.m_resource_views)
			{
				if (l_view_parameter.is_valid())
				{
					l_shader_types = core::bc_enum::mask_or({l_shader_types, l_view_parameter.get_shader_types()});

					m_pipeline->unbind_ps_shader_view_parameter(l_view_parameter);
				}
			}
			for (const auto& l_view_parameter : p_render_pass_state.m_unordered_views)
			{
				if (l_view_parameter.is_valid())
				{
					l_shader_types = core::bc_enum::mask_or({ l_shader_types, l_view_parameter.get_shader_types() });

					m_pipeline->unbind_ps_shader_view_parameter(l_view_parameter);
				}
			}
			for (const auto& l_buffer_parameter : p_render_pass_state.m_shader_cbuffers)
			{
				if (l_buffer_parameter.is_valid())
				{
					l_shader_types = core::bc_enum::mask_or({l_shader_types, l_buffer_parameter.get_shader_types()});

					m_pipeline->unbind_ps_constant_buffer_parameter(l_buffer_parameter);
				}
			}

			l_pipeline_stages = core::bc_enum::mask_or({l_pipeline_stages, _convert_shader_type_to_pipeline_stage(l_shader_types)});

			m_pipeline->pipeline_apply_states(l_pipeline_stages);
		}

		void bc_render_thread::bind_render_state(const bc_render_state& p_render_state, const bcUINT32* p_stencil_ref)
		{
			m_pipeline->bind_ia_primitive_topology(p_render_state.get_primitive());
			m_pipeline->bind_ia_vertex_buffers
			(
				0,
				1,
				(core::bc_array< graphic::bc_buffer, 1 >() = { p_render_state.get_vertex_buffer() }).data(),
				(core::bc_array< bcUINT, 1 >() = { p_render_state.get_vertex_buffer_stride() }).data(),
				(core::bc_array< bcUINT, 1 >() = { 0 }).data()
			);
			m_pipeline->bind_ia_index_buffer
			(
				p_render_state.get_index_buffer(),
				p_render_state.get_index_type() == i32bit ? graphic::bc_format::R32_UINT : graphic::bc_format::R16_UINT
			);

			graphic::bc_pipeline_stage l_pipeline_stages = graphic::bc_pipeline_stage::input_assembler_stage;
			graphic::bc_shader_type l_shader_types = core::bc_enum::none< graphic::bc_shader_type >();

			for (const auto& l_view_parameter : p_render_state.get_shader_views())
			{
				if (l_view_parameter.is_valid())
				{
					l_shader_types = core::bc_enum::mask_or({l_shader_types, l_view_parameter.get_shader_types()});
					m_pipeline->bind_ps_shader_view_parameter(l_view_parameter);
				}
			}
			for (const auto& l_buffer_parameter : p_render_state.get_shader_buffers())
			{
				if (l_buffer_parameter.is_valid())
				{
					l_shader_types = core::bc_enum::mask_or({l_shader_types, l_buffer_parameter.get_shader_types()});
					m_pipeline->bind_ps_constant_buffer_parameter(l_buffer_parameter);
				}
			}

			if(p_stencil_ref)
			{
				bind_om_stencil_ref(*p_stencil_ref);
				l_pipeline_stages = core::bc_enum::mask_or({ l_pipeline_stages, graphic::bc_pipeline_stage::output_merger_stage });
			}

			l_pipeline_stages = core::bc_enum::mask_or({l_pipeline_stages, _convert_shader_type_to_pipeline_stage(l_shader_types)});

			m_pipeline->pipeline_apply_states(l_pipeline_stages);
		}

		void bc_render_thread::unbind_render_state(const bc_render_state& p_render_state)
		{
			m_pipeline->unbind_ia_primitive_topology();
			m_pipeline->unbind_ia_vertex_buffers(0, 1);
			m_pipeline->unbind_ia_index_buffer();

			graphic::bc_pipeline_stage l_pipeline_stages = graphic::bc_pipeline_stage::input_assembler_stage;
			graphic::bc_shader_type l_shader_types = core::bc_enum::none< graphic::bc_shader_type >();

			for (const auto& l_view_parameter : p_render_state.get_shader_views())
			{
				if (l_view_parameter.is_valid())
				{
					l_shader_types = core::bc_enum::mask_or({l_shader_types, l_view_parameter.get_shader_types()});
					m_pipeline->unbind_ps_shader_view_parameter(l_view_parameter);
				}
			}
			for (const auto& l_buffer_parameter : p_render_state.get_shader_buffers())
			{
				if (l_buffer_parameter.is_valid())
				{
					l_shader_types = core::bc_enum::mask_or({l_shader_types, l_buffer_parameter.get_shader_types()});
					m_pipeline->unbind_ps_constant_buffer_parameter(l_buffer_parameter);
				}
			}

			l_pipeline_stages = core::bc_enum::mask_or({l_pipeline_stages, _convert_shader_type_to_pipeline_stage(l_shader_types)});

			m_pipeline->pipeline_apply_states(l_pipeline_stages);
		}

		void bc_render_thread::bind_compute_state(const bc_compute_state& p_compute_state, const bc_compute_state_unordered_view_initial_count_array* p_uav_initial_counts)
		{
			m_pipeline->bind_compute_state(p_compute_state.m_compute_state);

			for (const auto& l_sampler_parameter : p_compute_state.m_samplers)
			{
				if (l_sampler_parameter.is_valid())
				{
					m_pipeline->bind_ps_sampler_parameter(l_sampler_parameter);
				}
			}
			for (const auto& l_view_parameter : p_compute_state.m_resource_views)
			{
				if (l_view_parameter.is_valid())
				{
					m_pipeline->bind_ps_shader_view_parameter(l_view_parameter);
				}
			}
			for (const auto& l_view_parameter : p_compute_state.m_unordered_views)
			{
				if (l_view_parameter.is_valid())
				{
					m_pipeline->bind_ps_shader_view_parameter(l_view_parameter);
				}
			}
			for (const auto& l_buffer_parameter : p_compute_state.m_cbuffers)
			{
				if (l_buffer_parameter.is_valid())
				{
					m_pipeline->bind_ps_constant_buffer_parameter(l_buffer_parameter);
				}
			}

			m_pipeline->pipeline_apply_states(graphic::bc_pipeline_stage::compute_stage, p_uav_initial_counts);
		}

		void bc_render_thread::unbind_compute_state(const bc_compute_state& p_compute_state)
		{
			m_pipeline->unbind_compute_state();

			for (const auto& l_sampler_parameter : p_compute_state.m_samplers)
			{
				if (l_sampler_parameter.is_valid())
				{
					m_pipeline->unbind_ps_sampler_parameter(l_sampler_parameter);
				}
			}
			for (const auto& l_view_parameter : p_compute_state.m_resource_views)
			{
				if (l_view_parameter.is_valid())
				{
					m_pipeline->unbind_ps_shader_view_parameter(l_view_parameter);
				}
			}
			for (const auto& l_view_parameter : p_compute_state.m_unordered_views)
			{
				if (l_view_parameter.is_valid())
				{
					m_pipeline->unbind_ps_shader_view_parameter(l_view_parameter);
				}
			}
			for (const auto& l_buffer_parameter : p_compute_state.m_cbuffers)
			{
				if (l_buffer_parameter.is_valid())
				{
					m_pipeline->unbind_ps_constant_buffer_parameter(l_buffer_parameter);
				}
			}

			m_pipeline->pipeline_apply_states(graphic::bc_pipeline_stage::compute_stage);
		}

		/*void bc_render_thread::bind_ia_primitive_topology(graphic::bc_primitive p_primitive)
		{
			m_pipeline->bind_ia_primitive_topology(p_primitive);
		}

		void bc_render_thread::bind_ia_index_buffer(graphic::bc_buffer* p_buffer, graphic::bc_format p_format)
		{
			m_pipeline->bind_ia_index_buffer(p_buffer, p_format);
		}

		void bc_render_thread::unbind_ia_index_buffer()
		{
			m_pipeline->unbind_ia_index_buffer();
		}

		void bc_render_thread::bind_ia_vertex_buffers(bcUINT p_start_slot, bcUINT p_buffer_count, graphic::bc_buffer* p_buffers, bcUINT* p_strides, bcUINT* p_offsets)
		{
			m_pipeline->bind_ia_vertex_buffers(p_start_slot, p_buffer_count, p_buffers, p_strides, p_offsets);
		}

		void bc_render_thread::unbind_ia_vertex_buffers(bcUINT p_start_slot, bcUINT p_buffer_count)
		{
			m_pipeline->unbind_ia_vertex_buffers(p_start_slot, p_buffer_count);
		}*/

		void bc_render_thread::bind_ps_constant_buffer_parameter(const graphic::bc_constant_buffer_parameter& p_parameter)
		{
			m_pipeline->bind_ps_constant_buffer_parameter(p_parameter);
		}

		void bc_render_thread::unbind_ps_constant_buffer_parameter(const graphic::bc_constant_buffer_parameter& p_parameter)
		{
			m_pipeline->unbind_ps_constant_buffer_parameter(p_parameter);
		}

		void bc_render_thread::bind_ps_sampler_parameter(const graphic::bc_sampler_parameter& p_parameter)
		{
			m_pipeline->bind_ps_sampler_parameter(p_parameter);
		}

		void bc_render_thread::unbind_ps_sampler_parameter(const graphic::bc_sampler_parameter& p_parameter)
		{
			m_pipeline->unbind_ps_sampler_parameter(p_parameter);
		}

		void bc_render_thread::bind_ps_shader_view_parameter(const graphic::bc_resource_view_parameter& p_parameter)
		{
			m_pipeline->bind_ps_shader_view_parameter(p_parameter);
		}

		void bc_render_thread::unbind_ps_shader_view_parameter(const graphic::bc_resource_view_parameter& p_parameter)
		{
			m_pipeline->unbind_ps_shader_view_parameter(p_parameter);
		}

		/*void bc_render_thread::bind_os_stream_outputs(bcUINT p_buffer_count, graphic::bc_buffer* p_buffers, bcUINT* p_offsets)
		{
			m_pipeline->bind_os_stream_outputs(p_buffer_count, p_buffers, p_offsets);
		}

		void bc_render_thread::unbind_os_stream_outputs()
		{
			m_pipeline->unbind_os_stream_outputs();
		}

		void bc_render_thread::bind_rs_viewports(bcUINT p_count, graphic::bc_viewport* p_viewports)
		{
			m_pipeline->bind_rs_viewports(p_count, p_viewports);
		}

		void bc_render_thread::unbind_rs_viewports()
		{
			m_pipeline->unbind_rs_viewports();
		}*/

		void bc_render_thread::bind_om_blend_factors(const core::bc_vector4f& p_factors)
		{
			m_pipeline->bind_om_blend_factors(p_factors);
		}

		void bc_render_thread::bind_om_stencil_ref(bcUINT32 p_stencil_ref)
		{
			m_pipeline->bind_om_stencil_ref(p_stencil_ref);
		}

		/*void bc_render_thread::bind_om_render_targets(bcUINT p_target_count, graphic::bc_render_target_view* p_targets, graphic::bc_depth_stencil_view* p_depth)
		{
			m_pipeline->bind_om_render_targets(p_target_count, p_targets, p_depth);
		}

		void bc_render_thread::unbind_om_render_targets()
		{
			m_pipeline->unbind_om_render_targets();
		}*/

		void bc_render_thread::pipeline_apply_states(graphic::bc_pipeline_stage p_stages)
		{
			m_pipeline->pipeline_apply_states(p_stages);
		}

		void bc_render_thread::pipeline_set_default_states(graphic::bc_pipeline_stage p_stages)
		{
			m_pipeline->pipeline_set_default_states(p_stages);
		}

		void bc_render_thread::draw(bcUINT p_start_vertex, bcUINT p_vertex_count)
		{
			m_pipeline->draw(p_start_vertex, p_vertex_count);
		}

		void bc_render_thread::draw_instanced(bcUINT p_vertex_count_per_instance,
			bcUINT p_instance_count,
			bcUINT p_start_vertex_location,
			bcUINT p_start_instance_location)
		{
			m_pipeline->draw_instanced(p_vertex_count_per_instance, p_instance_count, p_start_vertex_location, p_start_instance_location);
		}

		void bc_render_thread::draw_instanced_indirect(graphic::bc_buffer p_args_buffer, bcUINT p_offset)
		{
			m_pipeline->draw_instanced_indirect(p_args_buffer, p_offset);
		}

		void bc_render_thread::draw_indexed(bcUINT p_start_index, bcUINT p_index_count, bcINT p_vertex_offset)
		{
			m_pipeline->draw_indexed(p_start_index, p_index_count, p_vertex_offset);
		}

		void bc_render_thread::draw_indexed_instanced(bcUINT p_index_count_per_instance, bcUINT p_instance_count, bcUINT p_start_index_location, bcINT p_base_vertex_location, bcUINT p_start_instance_location)
		{
			m_pipeline->draw_indexed_instanced(p_index_count_per_instance, p_instance_count, p_start_index_location, p_base_vertex_location, p_start_instance_location);
		}

		void bc_render_thread::draw_indexed_instanced_indirect(graphic::bc_buffer p_args_buffer, bcUINT p_offset)
		{
			m_pipeline->draw_indexed_instanced_indirect(p_args_buffer, p_offset);
		}

		void bc_render_thread::dispatch(bcUINT p_x, bcUINT p_y, bcUINT p_z)
		{
			m_pipeline->dispatch(p_x, p_y, p_z);
		}

		void bc_render_thread::dispatch_indirect(graphic::bc_buffer p_args, bcUINT p_offset)
		{
			m_pipeline->dispatch_indirect(p_args, p_offset);
		}

		void bc_render_thread::clear_buffers(const core::bc_vector4f* p_color, bcUINT32 p_count, bcFLOAT p_depth, bcUINT p_stencil)
		{
			m_pipeline->clear_buffers(p_color, p_count, p_depth, p_stencil);
		}

		/*graphic::bc_mapped_resource bc_render_thread::map_resource(graphic::bc_iresource& p_resource, bcUINT p_subresource, graphic::bc_resource_map p_map_type)
		{
			return m_pipeline->map_resource(p_resource, p_subresource, p_map_type);
		}

		void bc_render_thread::unmap_resource(graphic::bc_iresource& p_resource, bcUINT p_subresource)
		{
			m_pipeline->unmap_resource(p_resource, p_subresource);
		}*/

		void bc_render_thread::update_subresource(graphic::bci_resource& p_resource, bcUINT p_dst_subresource, const void* p_src_data, bcUINT p_src_row_pitch, bcUINT p_src_depth_pitch)
		{
			m_pipeline->update_subresource(p_resource, p_dst_subresource, p_src_data, p_src_row_pitch, p_src_depth_pitch);
		}

		void bc_render_thread::copy_subresource(graphic::bci_resource& p_dest_resource, bcUINT p_dst_subresource, graphic::bci_resource& p_src_resource, bcUINT p_src_subresource)
		{
			m_pipeline->copy_subresource(p_dest_resource, p_dst_subresource, p_src_resource, p_src_subresource);
		}

		void bc_render_thread::copy_resource(graphic::bci_resource& p_dest_resource, graphic::bci_resource& p_src_resource)
		{
			m_pipeline->copy_resource(p_dest_resource, p_src_resource);
		}

		void bc_render_thread::copy_structure_count(graphic::bc_buffer p_dest_resource, bcUINT p_offset, graphic::bc_resource_view p_unordered_resource)
		{
			m_pipeline->copy_structure_count(p_dest_resource, p_offset, p_unordered_resource);
		}

		void bc_render_thread::resolve_subresource(graphic::bci_resource& p_dest_resource, bcUINT p_dest_subresource, graphic::bci_resource& p_src_resource, bcUINT p_src_subresource, graphic::bc_format p_format)
		{
			m_pipeline->resolve_subresource(p_dest_resource, p_dest_subresource, p_src_resource, p_src_subresource, p_format);
		}

		void bc_render_thread::start(graphic::bc_device_command_list p_command_list) noexcept
		{
			BC_ASSERT(!m_command_list.is_valid());

			m_command_list = p_command_list;
			m_pipeline->start_command_list();
		}

		graphic::bc_device_command_list bc_render_thread::finish() noexcept
		{
			BC_ASSERT(m_command_list.is_valid());

			m_pipeline->finish_command_list(m_command_list);
			m_executor->execute_command_list(m_command_list);
			m_command_list.finished();

			graphic::bc_device_command_list l_command_list;
			std::swap(l_command_list, m_command_list);

			return l_command_list;
		}

		void bc_render_thread::start_query(graphic::bc_device_occlusion_query& p_query)
		{
			m_pipeline->start_query(p_query);
		}

		void bc_render_thread::end_query(graphic::bc_device_occlusion_query& p_query)
		{
			m_pipeline->end_query(p_query);
		}

		std::pair<bool, bcUINT64> bc_render_thread::get_query_data(graphic::bc_device_occlusion_query& p_query)
		{
			return m_pipeline->get_query_data(p_query);
		}

		void bc_render_thread::reset()
		{
			BC_ASSERT(!m_command_list.is_valid());
			
			m_pipeline.reset();
			m_executor.reset();
		}

		void bc_render_thread::reset(graphic::bc_device_pipeline_ref p_pipeline, graphic::bc_device_command_executor_ref p_command_executor)
		{
			BC_ASSERT(!m_command_list.is_valid());

			m_pipeline = std::move(p_pipeline);
			m_executor = std::move(p_command_executor);
		}
	}
}