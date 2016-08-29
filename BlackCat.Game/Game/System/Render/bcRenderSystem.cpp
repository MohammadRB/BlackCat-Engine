// [05/10/2016 MRB]

#include "Game/GamePCH.h"

#include "CorePlatform/CorePlatformPCH.h"
#include "Core/File/bcContentStreamManager.h"
#include "Core/Event/bcEventManager.h"
#include "Core/Utility/bcEnumOperand.h"
#include "Core/Utility/bcLogger.h"
#include "Platform/bcPlatformEvents.h"
#include "Graphic/bcEvent.h"
#include "Graphic/Math/bcMatrix4f.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "GraphicImp/Resource/bcResourceConfig.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcRenderState.h"

namespace black_cat
{
	namespace game
	{
		struct _bc_render_system_global_state_cbuffer
		{
			BC_CBUFFER_ALIGN graphic::bc_matrix4f m_view;
			BC_CBUFFER_ALIGN graphic::bc_matrix4f m_projection;
			BC_CBUFFER_ALIGN graphic::bc_matrix4f m_viewprojection;
			BC_CBUFFER_ALIGN bcDOUBLE m_total_elapsed;
			bcFLOAT m_elapsed;
		};

		graphic::bc_pipeline_stage _convert_shader_type_to_pipeline_stage(graphic::bc_shader_type p_shader_types)
		{
			graphic::bc_pipeline_stage l_pipeline_stages = core::bc_enum::none< graphic::bc_pipeline_stage  >();

			if (core::bc_enum::has(p_shader_types, graphic::bc_shader_type::vertex))
			{
				l_pipeline_stages = core::bc_enum:: or
				({
					l_pipeline_stages,
					graphic::bc_pipeline_stage::vertex_stage
				});
			}
			if (core::bc_enum::has(p_shader_types, graphic::bc_shader_type::domain))
			{
				l_pipeline_stages = core::bc_enum:: or
				({
					l_pipeline_stages,
					graphic::bc_pipeline_stage::domain_stage
				});
			}
			if (core::bc_enum::has(p_shader_types, graphic::bc_shader_type::hull))
			{
				l_pipeline_stages = core::bc_enum:: or
				({
					l_pipeline_stages,
					graphic::bc_pipeline_stage::hull_stage
				});
			}
			if (core::bc_enum::has(p_shader_types, graphic::bc_shader_type::geometry))
			{
				l_pipeline_stages = core::bc_enum:: or
				({
					l_pipeline_stages,
					graphic::bc_pipeline_stage::geometry_stage
				});
			}
			if (core::bc_enum::has(p_shader_types, graphic::bc_shader_type::pixel))
			{
				l_pipeline_stages = core::bc_enum:: or
				({
					l_pipeline_stages,
					graphic::bc_pipeline_stage::pixel_stage
				});
			}

			return l_pipeline_stages;
		}

		bc_render_system::~bc_render_system()
		{
			if(m_initialized)
			{
				destroy();
			}
		}

		graphic::bc_device_pipeline_state_ptr bc_render_system::create_device_pipeline_state(const bcCHAR* p_vertex_shader_name,
			const bcCHAR* p_hull_shader_name,
			const bcCHAR* p_domain_shader_name,
			const bcCHAR* p_geometry_shader_name,
			const bcCHAR* p_pixel_shader_name,
			bc_vertex_type p_vertex_layout, 
			bc_blend_type p_blend,
			bc_depth_stencil_type p_depth_stencil, 
			bc_rasterizer_type p_rasterizer, 
			bcUINT p_sample_mask, 
			std::initializer_list< graphic::bc_format > p_render_target_formats, 
			graphic::bc_format p_depth_stencil_format, 
			bc_multi_sample_type p_ms_config)
		{
			bcAssert(p_render_target_formats.size() < graphic::bc_render_api_info::number_of_om_render_target_slots());

			auto* l_content_stream = core::bc_service_manager::get().get_service< core::bc_content_stream_manager >();

			graphic::bc_vertex_shader_content_ptr l_vertex_shader;
			graphic::bc_hull_shader_content_ptr l_hull_shader;
			graphic::bc_domain_shader_content_ptr l_domain_shader;
			graphic::bc_geometry_shader_content_ptr l_geometry_shader;
			graphic::bc_pixel_shader_content_ptr l_pixel_shader;
			graphic::bc_input_layout_config l_input_layout_config;
			graphic::bc_blend_state_config l_blend_state_config;
			graphic::bc_depth_stencil_state_config l_depth_stencil_state_config;
			graphic::bc_rasterizer_state_config l_rasterizer_state_config;

			if(p_vertex_shader_name)
			{
				l_vertex_shader = l_content_stream->find_content_throw< graphic::bc_vertex_shader_content >(p_vertex_shader_name);
			}
			if (p_hull_shader_name)
			{
				l_hull_shader = l_content_stream->find_content_throw< graphic::bc_hull_shader_content >(p_hull_shader_name);
			}
			if (p_domain_shader_name)
			{
				l_domain_shader = l_content_stream->find_content_throw< graphic::bc_domain_shader_content >(p_domain_shader_name);
			}
			if (p_geometry_shader_name)
			{
				l_geometry_shader = l_content_stream->find_content_throw< graphic::bc_geometry_shader_content >(p_geometry_shader_name);
			}
			if (p_pixel_shader_name)
			{
				l_pixel_shader = l_content_stream->find_content_throw< graphic::bc_pixel_shader_content >(p_pixel_shader_name);
			}

			l_input_layout_config = bc_graphic_state_configs::bc_input_layout_config(p_vertex_layout);
			l_blend_state_config = bc_graphic_state_configs::bc_blend_config(p_blend);
			l_depth_stencil_state_config = bc_graphic_state_configs::bc_depth_stencil_config(p_depth_stencil);
			l_rasterizer_state_config = bc_graphic_state_configs::bc_rasterizer_config(p_rasterizer);

			graphic::bc_device_pipeline_state_config l_pipeline_config;

			l_pipeline_config.m_vertex_shader = l_vertex_shader ? l_vertex_shader.get()->get_resource() : nullptr;
			l_pipeline_config.m_hull_shader = l_hull_shader ? l_hull_shader.get()->get_resource() : nullptr;
			l_pipeline_config.m_domain_shader = l_domain_shader ? l_domain_shader.get()->get_resource() : nullptr;
			l_pipeline_config.m_geometry_shader = l_geometry_shader ? l_geometry_shader.get()->get_resource() : nullptr;
			l_pipeline_config.m_pixel_shader = l_pixel_shader ? l_pixel_shader.get()->get_resource() : nullptr;
			l_pipeline_config.m_blend_state_config = l_blend_state_config;
			l_pipeline_config.m_sample_mask = p_sample_mask;
			l_pipeline_config.m_depth_stencil_state_config = l_depth_stencil_state_config;
			l_pipeline_config.m_rasterizer_state_config = l_rasterizer_state_config;
			l_pipeline_config.m_input_layout_config = l_input_layout_config;
			l_pipeline_config.m_num_render_target = p_render_target_formats.size();
			std::copy(std::begin(p_render_target_formats), std::end(p_render_target_formats), std::begin(l_pipeline_config.m_render_target_format));
			l_pipeline_config.m_depth_stencil_format = p_depth_stencil_format;
			l_pipeline_config.m_sample_config = bc_graphic_state_configs::bc_texture_multi_sample(p_ms_config);
			
			return m_device.create_pipeline_state(l_pipeline_config);
		}

		bc_render_pass_state_ptr bc_render_system::create_render_pass_state(graphic::bc_device_pipeline_state_ptr& p_pipeline_state,
			graphic::bc_viewport p_viewport,
			bc_render_pass_state_render_target_view_array&& p_shader_targets,
			graphic::bc_depth_stencil_view_ptr p_shader_depth,
			bc_render_pass_state_sampler_array&& p_shader_samplers,
			bc_render_pass_state_shader_view_array&& p_shader_views,
			bc_render_pass_state_constant_buffer_array&& p_shader_buffers)
		{
			bc_render_pass_state l_render_pass_state
			(
				p_pipeline_state,
				p_viewport,
				std::move(p_shader_targets),
				p_shader_depth,
				std::move(p_shader_samplers),
				std::move(p_shader_views),
				std::move(p_shader_buffers)
			);

			bcSIZE l_parameter_register = g_render_pass_state_sampler_min_index;

			for (auto& l_sampler_parameter : l_render_pass_state.m_shader_samplers)
			{
				l_sampler_parameter.set_register_index(l_parameter_register++);
			}

			l_parameter_register = g_render_pass_state_shader_view_min_index;

			for (auto& l_shader_parameter : l_render_pass_state.m_shader_views)
			{
				l_shader_parameter.set_register_index(l_parameter_register++);
			}

			l_parameter_register = g_render_pass_state_constant_buffer_min_index;

			for (auto& l_cbuffer_parameter : l_render_pass_state.m_shader_cbuffers)
			{
				l_cbuffer_parameter.set_register_index(l_parameter_register++);
			}

			core_platform::bc_lock_guard< core_platform::bc_mutex > l_guard(m_render_states_mutex);
			{
				auto l_first_empty = std::find_if(std::begin(m_render_pass_states), std::end(m_render_pass_states), [this](const core::bc_nullable< bc_render_pass_state >& p_item)
				{
					return !p_item.is_set();
				});

				if (l_first_empty != std::end(m_render_pass_states))
				{
					(*l_first_empty).reset(std::move(l_render_pass_state));
				}
				else
				{
					m_render_pass_states.push_back(core::bc_nullable< bc_render_pass_state >(std::move(l_render_pass_state)));

					l_first_empty = std::begin(m_render_pass_states);
					std::advance(l_first_empty, std::size(m_render_pass_states) - 1);
				}

				auto l_first_empty_index = std::distance(std::begin(m_render_pass_states), l_first_empty);
				auto l_ptr = bc_render_pass_state_ptr(_bc_render_state_handle(l_first_empty_index), _bc_render_pass_state_handle_deleter(this));

				return l_ptr;
			}
		}

		bc_render_state_ptr bc_render_system::create_render_state(graphic::bc_primitive p_primitive,
			graphic::bc_buffer_ptr& p_vertex_buffer, 
			bcUINT32 p_verext_buffer_offset, 
			graphic::bc_buffer_ptr& p_index_buffer, 
			bc_index_type p_index_type,
			bcUINT32 p_index_count, 
			bcUINT32 p_index_buffer_offset, 
			bc_render_state_shader_view_array&& p_shader_views,
			bc_render_state_constant_buffer_array&& p_shader_buffers)
		{
			bc_render_state l_render_state
				(
					p_primitive,
					p_vertex_buffer,
					p_verext_buffer_offset,
					p_index_buffer,
					p_index_type,
					p_index_count,
					p_index_buffer_offset,
					std::move(p_shader_views),
					std::move(p_shader_buffers)
				);

			bcSIZE l_parameter_register = g_render_state_shader_view_min_index;

			for (auto& l_shader_parameter : l_render_state.m_shader_views)
			{
				l_shader_parameter.set_register_index(l_parameter_register++);
			}

			l_parameter_register = g_render_state_constant_buffer_min_index;

			for (auto& l_cbuffer_parameter : l_render_state.m_shader_cbuffers)
			{
				l_cbuffer_parameter.set_register_index(l_parameter_register++);
			}

			core_platform::bc_lock_guard< core_platform::bc_mutex > l_guard(m_render_states_mutex);
			{
				auto l_first_empty = std::find_if(std::begin(m_render_states), std::end(m_render_states), [this](const render_state_entry& p_item)
				{
					return !p_item.first.is_set();
				});

				if (l_first_empty != std::end(m_render_states))
				{
					(*l_first_empty).first.reset(std::move(l_render_state));
				}
				else
				{
					m_render_states.push_back(render_state_entry
					(
						core::bc_nullable< bc_render_state >(std::move(l_render_state)),
						core::bc_vector_frame< bc_render_instance >()
					));

					l_first_empty = std::begin(m_render_states);
					std::advance(l_first_empty, std::size(m_render_states) - 1);
				}

				auto l_first_empty_index = std::distance(std::begin(m_render_states), l_first_empty);
				auto l_ptr = bc_render_state_ptr(_bc_render_state_handle(l_first_empty_index), _bc_render_state_handle_deleter(this));

				return l_ptr;
			}
		}

		void bc_render_system::destroy_render_pass_state(bc_render_pass_state* p_render_pass_state)
		{
			core_platform::bc_lock_guard< core_platform::bc_mutex > l_guard(m_render_states_mutex);
			{
				auto l_item = std::find_if(std::begin(m_render_pass_states), std::end(m_render_pass_states), [this, p_render_pass_state](const core::bc_nullable< bc_render_pass_state >& p_item)
				{

					return &p_item.get() == p_render_pass_state;

				});

				if (l_item == std::end(m_render_pass_states))
				{
					bcAssert(false, "Render pass state not found");
					return;
				}

				// We set object to null because we can use null objetcs again 
				(*l_item).reset(nullptr);
			}
		}

		void bc_render_system::destroy_render_state(bc_render_state* p_render_state)
		{
			core_platform::bc_lock_guard< core_platform::bc_mutex > l_guard(m_render_states_mutex);
			{
				auto l_item = std::find_if(std::begin(m_render_states), std::end(m_render_states), [this, p_render_state](const render_state_entry& p_item)
				{

					return &p_item.first.get() == p_render_state;

				});

				if (l_item == std::end(m_render_states))
				{
					bcAssert(false, "Render state not found");
					return;
				}

				// We set object to null because we can use null objetcs again 
				(*l_item).first.reset(nullptr);
			}
		}

		void bc_render_system::add_render_pass(bcUINT p_location, core::bc_unique_ptr<bc_irender_pass>&& p_pass)
		{
			p_pass->initialize_resources(*this, &m_device);
			m_render_pass_manager.add_pass(p_location, std::move(p_pass));
		}

		bool bc_render_system::remove_render_pass(bcUINT p_location)
		{
			bc_irender_pass* l_pass = m_render_pass_manager.get_pass(p_location);

			if (!l_pass)
				return false;

			l_pass->destroy(&m_device);

			return m_render_pass_manager.remove_pass(p_location);
		}

		bool bc_render_system::remove_render_pass(core::bc_string p_name)
		{
			auto* l_pass = m_render_pass_manager.get_pass(std::move(p_name));

			if (!l_pass)
				return false;

			l_pass->destroy(&m_device);

			return m_render_pass_manager.remove_pass(std::move(p_name));
		}

		void bc_render_system::bind_render_pass_state(graphic::bc_device_pipeline* p_pipeline, bc_render_pass_state_ptr& p_render_pass_state)
		{
			bcSIZE l_render_target_count = 0;
			core::bc_array<graphic::bc_render_target_view*, g_render_pass_state_render_target_view_count> l_render_targets;
			graphic::bc_depth_stencil_view* l_depth_stencil = p_render_pass_state->m_shader_depth.get();
			std::transform
				(
					std::cbegin(p_render_pass_state->m_shader_targets),
					std::cend(p_render_pass_state->m_shader_targets),
					std::begin(l_render_targets),
					[&l_render_target_count](const graphic::bc_render_target_view_ptr& p_target)
					{
						auto l_pointer = static_cast<graphic::bc_render_target_view*>(p_target.get());
						
						if(l_pointer)
						{
							++l_render_target_count;
						}

						return l_pointer;
					}
				);

			graphic::bc_pipeline_stage l_pipeline_stages = core::bc_enum:: or
				({
					graphic::bc_pipeline_stage::rasterizer_stage,
					graphic::bc_pipeline_stage::output_merger_stage
				});

			p_pipeline->bind_pipeline_state(p_render_pass_state->m_pipeline_state.get());
			p_pipeline->bind_rs_viewports(1, &p_render_pass_state->m_viewport);
			p_pipeline->bind_om_render_targets(l_render_target_count, *l_render_targets.data(), l_depth_stencil);

			graphic::bc_shader_type l_shader_types = core::bc_enum::none< graphic::bc_shader_type >();

			for(auto& l_sampler_parameter : p_render_pass_state->m_shader_samplers)
			{
				if(l_sampler_parameter.is_valid())
				{
					l_shader_types = core::bc_enum:: or({ l_shader_types, l_sampler_parameter.get_shader_types() });

					p_pipeline->bind_ps_sampler_parameter(l_sampler_parameter);
				}
			}
			for (auto& l_view_parameter : p_render_pass_state->m_shader_views)
			{
				if(l_view_parameter.is_valid())
				{
					l_shader_types = core::bc_enum:: or({ l_shader_types, l_view_parameter.get_shader_types() });

					p_pipeline->bind_ps_shader_view_parameter(l_view_parameter);
				}
			}
			for (auto& l_buffer_parameter : p_render_pass_state->m_shader_cbuffers)
			{
				if(l_buffer_parameter.is_valid())
				{
					l_shader_types = core::bc_enum:: or({ l_shader_types, l_buffer_parameter.get_shader_types() });

					p_pipeline->bind_ps_constant_buffer_parameter(l_buffer_parameter);
				}
			}

			l_pipeline_stages = core::bc_enum:: or({ l_pipeline_stages, _convert_shader_type_to_pipeline_stage(l_shader_types) });

			p_pipeline->pipeline_apply_states(l_pipeline_stages);
		}

		void bc_render_system::unbind_render_pass_state(graphic::bc_device_pipeline* p_pipeline, bc_render_pass_state_ptr& p_render_pass_state)
		{
			p_pipeline->unbind_pipeline_state();
			p_pipeline->unbind_rs_viewports();
			p_pipeline->unbind_om_render_targets();

			graphic::bc_pipeline_stage l_pipeline_stages = core::bc_enum::or
			({
				graphic::bc_pipeline_stage::rasterizer_stage,
				graphic::bc_pipeline_stage::output_merger_stage
			});
			graphic::bc_shader_type l_shader_types = core::bc_enum::none< graphic::bc_shader_type >();

			for (auto& l_sampler_parameter : p_render_pass_state->m_shader_samplers)
			{
				if (l_sampler_parameter.is_valid())
				{
					l_shader_types = core::bc_enum::or({ l_shader_types, l_sampler_parameter.get_shader_types() });

					p_pipeline->unbind_ps_sampler_parameter(l_sampler_parameter);
				}
			}
			for (auto& l_view_parameter : p_render_pass_state->m_shader_views)
			{
				if (l_view_parameter.is_valid())
				{
					l_shader_types = core::bc_enum::or({ l_shader_types, l_view_parameter.get_shader_types() });

					p_pipeline->unbind_ps_shader_view_parameter(l_view_parameter);
				}
			}
			for (auto& l_buffer_parameter : p_render_pass_state->m_shader_cbuffers)
			{
				if (l_buffer_parameter.is_valid())
				{
					l_shader_types = core::bc_enum::or({ l_shader_types, l_buffer_parameter.get_shader_types() });

					p_pipeline->unbind_ps_constant_buffer_parameter(l_buffer_parameter);
				}
			}

			l_pipeline_stages = core::bc_enum:: or({ l_pipeline_stages, _convert_shader_type_to_pipeline_stage(l_shader_types) });

			p_pipeline->pipeline_apply_states(l_pipeline_stages);
		}

		void bc_render_system::bind_render_state(graphic::bc_device_pipeline* p_pipeline, bc_render_state_ptr& p_render_state)
		{
			p_pipeline->bind_ia_primitive_topology(p_render_state->get_primitive());
			p_pipeline->bind_ia_vertex_buffers
				(
					0,
					1,
					p_render_state->get_vertex_buffer().get(),
					(core::bc_array< bcUINT, 1 >() = {p_render_state->get_vertex_buffer()->get_structure_byte_stride()}).data(),
					(core::bc_array< bcUINT, 1 >() = {0}).data()
				);
			p_pipeline->bind_ia_index_buffer
				(
					p_render_state->get_index_buffer().get(),
					p_render_state->get_index_type() == i32bit ? graphic::bc_format::R32_UINT : graphic::bc_format::R16_UINT
				);

			graphic::bc_pipeline_stage l_pipeline_stages = graphic::bc_pipeline_stage::input_assembler_stage;
			graphic::bc_shader_type l_shader_types = core::bc_enum::none< graphic::bc_shader_type >();

			for (auto& l_view_parameter : p_render_state->get_shader_views())
			{
				if(l_view_parameter.is_valid())
				{
					l_shader_types = core::bc_enum::or({ l_shader_types, l_view_parameter.get_shader_types() });
					p_pipeline->bind_ps_shader_view_parameter(l_view_parameter);
				}
			}
			for (auto& l_buffer_parameter : p_render_state->get_shader_buffers())
			{
				if(l_buffer_parameter.is_valid())
				{
					l_shader_types = core::bc_enum::or({ l_shader_types, l_buffer_parameter.get_shader_types() });
					p_pipeline->bind_ps_constant_buffer_parameter(l_buffer_parameter);
				}
			}

			l_pipeline_stages = core::bc_enum::or({ l_pipeline_stages, _convert_shader_type_to_pipeline_stage(l_shader_types) });

			p_pipeline->pipeline_apply_states(l_pipeline_stages);
		}

		void bc_render_system::unbind_render_state(graphic::bc_device_pipeline* p_pipeline, bc_render_state_ptr& p_render_state)
		{
			p_pipeline->unbind_ia_vertex_buffers(0, 1);
			p_pipeline->unbind_ia_index_buffer();

			graphic::bc_pipeline_stage l_pipeline_stages = graphic::bc_pipeline_stage::input_assembler_stage;
			graphic::bc_shader_type l_shader_types = core::bc_enum::none< graphic::bc_shader_type >();

			for (auto& l_view_parameter : p_render_state->get_shader_views())
			{
				if (l_view_parameter.is_valid())
				{
					l_shader_types = core::bc_enum::or({ l_shader_types, l_view_parameter.get_shader_types() });
					p_pipeline->unbind_ps_shader_view_parameter(l_view_parameter);
				}
			}
			for (auto& l_buffer_parameter : p_render_state->get_shader_buffers())
			{
				if (l_buffer_parameter.is_valid())
				{
					l_shader_types = core::bc_enum::or({ l_shader_types, l_buffer_parameter.get_shader_types() });
					p_pipeline->unbind_ps_constant_buffer_parameter(l_buffer_parameter);
				}
			}

			l_pipeline_stages = core::bc_enum::or({ l_pipeline_stages, _convert_shader_type_to_pipeline_stage(l_shader_types) });

			p_pipeline->pipeline_apply_states(l_pipeline_stages);
		}

		void bc_render_system::add_render_instance(const bc_render_state_ptr& p_state, const bc_render_instance& p_instance)
		{
			// Bug this function isn't threadsafe
			auto l_state_index = p_state.get_handle().m_index;

			bcAssert(m_render_states.at(l_state_index).first.is_set());

			m_render_states.at(l_state_index).second.push_back(p_instance);
		}

		void bc_render_system::render_all_instances()
		{
			auto l_index = 0;
			for (render_state_entry& l_render_state : m_render_states)
			{
				if (l_render_state.first.is_set())
				{
					auto l_state_ptr = bc_render_state_ptr(_bc_render_state_handle(l_index), _bc_render_state_handle_deleter(this));
					bind_render_state(m_device_pipeline.get(), l_state_ptr);

					for (bc_render_instance& l_instance : l_render_state.second)
					{
						auto l_transposed_world = l_instance.get_world().transpose();

						/*l_transposed_world = graphic::bc_matrix4f::identity();
						l_transposed_world.transpose();*/

						m_device_pipeline->update_subresource(m_perobject_cbuffer_parameter.get_buffer().get(), 0, &l_transposed_world, 0, 0);

						m_device_pipeline->bind_ps_constant_buffer_parameter(m_perobject_cbuffer_parameter);
						m_device_pipeline->pipeline_apply_states(graphic::bc_pipeline_stage::vertex_stage);

						m_device_pipeline->draw_indexed
						(
							l_render_state.first->get_index_buffer_offset(),
							l_render_state.first->get_index_count(),
							l_render_state.first->get_vertex_buffer_offset()
						);
					}

					unbind_render_state(m_device_pipeline.get(), l_state_ptr);
				}

				++l_index;
			}
		}

		void bc_render_system::clear_render_instances()
		{
			// Clear all instances and free their memory
			for (render_state_entry& l_render_state : m_render_states)
			{
				l_render_state.second.clear();
				l_render_state.second.shrink_to_fit();
			}
		}

		void bc_render_system::update(update_param p_update_params)
		{
			_bc_render_system_global_state_cbuffer l_global_state;
			l_global_state.m_view = p_update_params.m_view_matrix.transpose();
			l_global_state.m_projection = p_update_params.m_projection_matrix.transpose();
			l_global_state.m_viewprojection = (p_update_params.m_view_matrix * p_update_params.m_projection_matrix).transpose();
			l_global_state.m_elapsed = p_update_params.m_elapsed;
			l_global_state.m_total_elapsed = p_update_params.m_total_elapsed;

			/*auto l_view = graphic::bc_matrix4f::look_at_lh_matrix(graphic::bc_vector3f(0, 0, -100), graphic::bc_vector3f(0, 0, 0), graphic::bc_vector3f(0, 1, 0));
			auto l_proj = graphic::bc_matrix4f::perspective_fov_lh_matrix(1.570796327f, 800.0f / 500.0f, 1, 1000);

			l_global_state.m_view = l_view.transpose();
			l_global_state.m_projection = l_proj.transpose();
			l_global_state.m_viewprojection = (p_update_params.m_view_matrix * p_update_params.m_projecttion_matrix).transpose();*/

			m_device_pipeline->update_subresource(m_global_cbuffer_parameter.get_buffer().get(), 0, &l_global_state, 0, 0);

			m_render_pass_manager.pass_update(p_update_params);
		}

		void bc_render_system::initialize_frame()
		{
			m_device_pipeline->bind_ps_constant_buffer_parameter(m_global_cbuffer_parameter);
			m_device_pipeline->pipeline_apply_states
			(
				core::bc_enum:: or ({ graphic::bc_pipeline_stage::vertex_stage, graphic::bc_pipeline_stage::pixel_stage })
			);

			m_render_pass_manager.pass_initialize_frame(*this, m_device_pipeline.get(), m_device_executer.get());
		}

		void bc_render_system::start_frame()
		{
			m_render_pass_manager.pass_execute(*this, m_device_pipeline.get(), m_device_executer.get());
		}

		void bc_render_system::finish_frame()
		{
			clear_render_instances();

			m_device.present();
		}

		void bc_render_system::_initialize(bc_render_system_parameter p_parameter)
		{
			m_device.initialize
				(
					p_parameter.m_device_backbuffer_width,
					p_parameter.m_device_backbuffer_height,
					p_parameter.m_device_backbuffer_format,
					p_parameter.m_render_window
				);
			auto l_alloc_type = m_device.set_allocator_alloc_type(core::bc_alloc_type::program);

			m_device_executer = m_device.create_command_executer();
			m_device_pipeline = m_device.create_pipeline();

			m_device.set_allocator_alloc_type(l_alloc_type);

			auto* l_event_manager = core::bc_service_manager::get().get_service< core::bc_event_manager >();

			m_window_resize_handle = l_event_manager->register_event_listener< platform::bc_app_event_window_resize >
				(
					core::bc_event_manager::delegate_type(this, &bc_render_system::_event_handler)
				);
			m_device_listener_handle = l_event_manager->register_event_listener< graphic::bc_app_event_device_reset >
				(
					core::bc_event_manager::delegate_type(this, &bc_render_system::_event_handler)
				);

			auto l_global_cbuffer_config = graphic::bc_graphic_resource_configure()
				.as_resource()
				.as_buffer
				(
					1,
					sizeof(_bc_render_system_global_state_cbuffer),
					graphic::bc_resource_usage::gpu_rw,
					graphic::bc_resource_view_type::none
				)
				.as_constant_buffer();
			auto l_perobject_cbuffer_config = graphic::bc_graphic_resource_configure()
				.as_resource()
				.as_buffer
				(
					1,
					sizeof(graphic::bc_matrix4f),
					graphic::bc_resource_usage::gpu_rw,
					graphic::bc_resource_view_type::none
				)
				.as_constant_buffer();
			auto l_global_cbuffer = m_device.create_buffer(l_global_cbuffer_config, nullptr);
			auto l_perobject_cbuffer = m_device.create_buffer(l_perobject_cbuffer_config, nullptr);
			
			m_global_cbuffer_parameter = graphic::bc_constant_buffer_parameter
			(
				g_render_pass_state_constant_buffer_min_index - 1,
				core::bc_enum::or({ graphic::bc_shader_type::vertex, graphic::bc_shader_type::pixel }),
				l_global_cbuffer
			);
			m_perobject_cbuffer_parameter = graphic::bc_constant_buffer_parameter
			(
				g_render_state_constant_buffer_min_index - 1,
				core::bc_enum:: or ({ graphic::bc_shader_type::vertex, graphic::bc_shader_type::pixel }),
				l_perobject_cbuffer
			);
		}

		void bc_render_system::_destroy()
		{
			auto* l_event_manager = core::bc_service_manager::get().get_service< core::bc_event_manager >();

			m_global_cbuffer_parameter = graphic::bc_constant_buffer_parameter();
			m_perobject_cbuffer_parameter = graphic::bc_constant_buffer_parameter();
			 
			l_event_manager->unregister_event_listener(m_device_listener_handle);
			l_event_manager->unregister_event_listener(m_window_resize_handle);

			m_render_pass_manager.pass_destroy(&m_device);

#ifdef BC_DEBUG // All states must be release upon render system destruction
			auto l_render_pass_states_count = 0;
			auto l_render_states_count = 0;

			for (auto& l_item : m_render_pass_states)
			{
				if (l_item != nullptr)
					++l_render_pass_states_count;
			}
			for (auto& l_item : m_render_states)
			{
				if (l_item.first != nullptr)
					++l_render_states_count;
			}

			bcAssert(l_render_pass_states_count + l_render_states_count == 0);
#endif

			m_device_executer.reset(nullptr);
			m_device_pipeline.reset(nullptr);
			m_device.destroy();
		}

		bool bc_render_system::_event_handler(core::bc_ievent& p_event)
		{
			bool l_is_window_resize = core::bc_ievent::event_is<platform::bc_app_event_window_resize>(p_event);
			bool l_is_device_reset = core::bc_ievent::event_is<graphic::bc_app_event_device_reset>(p_event);

			if(l_is_window_resize)
			{
				platform::bc_app_event_window_resize& l_resize_event = static_cast< platform::bc_app_event_window_resize& >(p_event);

				m_device.resize_back_buffer(l_resize_event.width(), l_resize_event.height());

				return true;
			}
			
			if(l_is_device_reset)
			{
				auto& l_device_reset_event = static_cast<graphic::bc_app_event_device_reset&>(p_event);

				if (l_device_reset_event.m_before_reset)
				{
					m_render_pass_manager.before_reset(*this, l_device_reset_event.m_device, l_device_reset_event.m_old_parameters, l_device_reset_event.m_new_parameters);
				}
				else
				{
					m_render_pass_manager.after_reset(*this, l_device_reset_event.m_device, l_device_reset_event.m_old_parameters, l_device_reset_event.m_new_parameters);
				}

				return true;
			}
			
			return false;
		}
	}
}