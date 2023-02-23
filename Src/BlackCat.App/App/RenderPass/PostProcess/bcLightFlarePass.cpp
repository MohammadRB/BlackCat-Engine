// [04/29/2021 MRB]

#include "App/AppPCH.h"

#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcCoordinate.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Utility/bcEnumOperand.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "GraphicImp/Shader/Parameter/bcSamplerParameter.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcDefaultRenderThread.h"
#include "App/RenderPass/PostProcess/bcLightFlarePass.h"
#include "App/bcConstant.h"

namespace black_cat
{
	struct _bc_light_flare_draw_instance
	{
		core::bc_vector3f m_position;
		core::bc_vector3f m_color;
		bcFLOAT m_size;
		bcFLOAT m_intensity;
		bcUINT32 m_texture_index;
		bcFLOAT m_u0;
		bcFLOAT m_v0;
		bcFLOAT m_u1;
		bcFLOAT m_v1;
	};

	bc_light_flare_pass::bc_light_flare_pass(game::bc_render_pass_variable_t p_render_target_texture, game::bc_render_pass_variable_t p_render_target_view)
		: m_flare_size_shrink_distance(s_flare_size_shrink_distance * bc_get_global_config().get_scene_global_scale()),
		m_flare_surface_grow_distance(s_flare_surface_grow_distance * bc_get_global_config().get_scene_global_scale()),
		m_render_target_texture(p_render_target_texture),
		m_render_target_view(p_render_target_view)
	{
	}

	void bc_light_flare_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
		auto& l_device = p_render_system.get_device();
		auto& l_device_swap_buffer = p_render_system.get_device_swap_buffer();

		core::bc_array<core::bc_vector3f, 8> l_cube_vertices
		{
			core::bc_vector3f(-.5, -.5, -.5),
			core::bc_vector3f(-.5, -.5, .5),
			core::bc_vector3f(.5, -.5, .5),
			core::bc_vector3f(.5, -.5, -.5),
			core::bc_vector3f(-.5, .5, -.5),
			core::bc_vector3f(-.5, .5, .5),
			core::bc_vector3f(.5, .5, .5),
			core::bc_vector3f(.5, .5, -.5),
		};
		core::bc_array<bcINT16, 36> l_cube_indices
		{
			0,2,1, // Bottom
			0,3,2,
			4,5,6, // Top
			4,6,7,
			0,4,7, // Front
			0,7,3,
			1,4,0, // Left
			1,5,4,
			1,6,5, // Back
			1,2,6,
			2,7,6, // Right
			2,3,7,
		};

		auto l_cube_vb_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer(8, sizeof(core::bc_vector3f), graphic::bc_resource_usage::gpu_r)
			.as_vertex_buffer();
		auto l_cube_ib_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer(36, sizeof(bcINT16), graphic::bc_resource_usage::gpu_r)
			.as_index_buffer();
		auto l_flare_instances_buffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer(s_per_draw_flare_count, sizeof(_bc_light_flare_draw_instance), graphic::bc_resource_usage::gpu_rw, graphic::bc_resource_view_type::shader)
			.with_structured_buffer(sizeof(_bc_light_flare_draw_instance))
			.as_buffer();
		auto l_flare_instances_buffer_view_config = graphic::bc_graphic_resource_builder()
			.as_resource_view()
			.as_buffer_view(graphic::bc_format::unknown)
			.as_shader_view(0, s_per_draw_flare_count)
			.as_structured_buffer();
		auto l_cube_vb_data = graphic::bc_subresource_data(&l_cube_vertices[0], 0, 0);
		auto l_cube_ib_data = graphic::bc_subresource_data(&l_cube_indices[0], 0, 0);

		const auto l_linear_sampler_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_sampler_state
			(
				graphic::bc_filter::min_mag_mip_linear,
				graphic::bc_texture_address_mode::wrap,
				graphic::bc_texture_address_mode::wrap,
				graphic::bc_texture_address_mode::wrap
			)
			.as_sampler_state();
		
		m_linear_sampler = l_device.create_sampler_state(l_linear_sampler_config);
		m_cube_vb = l_device.create_buffer(l_cube_vb_config, &l_cube_vb_data);
		m_cube_ib = l_device.create_buffer(l_cube_ib_config, &l_cube_ib_data);
		m_flare_instances_buffer = l_device.create_buffer(l_flare_instances_buffer_config, nullptr);
		m_flare_instances_view = l_device.create_resource_view(m_flare_instances_buffer.get(), l_flare_instances_buffer_view_config);

		after_reset
		(
			game::bc_render_pass_reset_context
			(
				p_render_system,
				l_device,
				l_device_swap_buffer,
				graphic::bc_device_parameters
				(
					0,
					0,
					graphic::bc_format::unknown,
					graphic::bc_texture_ms_config(1, 0)
				),
				graphic::bc_device_parameters
				(
					l_device_swap_buffer.get_back_buffer_width(),
					l_device_swap_buffer.get_back_buffer_height(),
					l_device_swap_buffer.get_back_buffer_format(),
					l_device_swap_buffer.get_back_buffer_texture().get_sample_count()
				)
			)
		);
	}

	void bc_light_flare_pass::update(const game::bc_render_pass_update_context& p_context)
	{
	}

	void bc_light_flare_pass::initialize_frame(const game::bc_render_pass_render_context& p_context)
	{
		if(m_scene_light_query.is_executed())
		{
			m_scene_light_query_result = m_scene_light_query.get().get_lights();
			
			const auto l_remove_ite = std::remove_if
			(
				std::cbegin(m_scene_light_query_result),
				std::cend(m_scene_light_query_result),
				[](const game::bc_light_instance& p_light)
				{
					return p_light.get_flare() == nullptr;
				}
			);
			if(l_remove_ite != std::cend(m_scene_light_query_result))
			{
				m_scene_light_query_result.erase(l_remove_ite, std::cend(m_scene_light_query_result));
			}
		}

		m_scene_light_query = p_context.m_query_manager.queue_query
		(
			game::bc_main_camera_scene_light_query(core::bc_enum::mask_or({game::bc_light_type::point, game::bc_light_type::spot}))
		);
	}

	void bc_light_flare_pass::execute(const game::bc_render_pass_render_context& p_context)
	{
		p_context.m_render_thread.start();

		_update_light_queries(p_context);
		_draw_flares(p_context);
		
		p_context.m_render_thread.finish();

		m_ready_to_draw_instances.clear();
		m_ready_to_draw_instances.shrink_to_fit();
	}

	void bc_light_flare_pass::before_reset(const game::bc_render_pass_reset_context& p_context)
	{
	}

	void bc_light_flare_pass::after_reset(const game::bc_render_pass_reset_context& p_context)
	{
		const auto l_depth_stencil = get_shared_resource_throw<graphic::bc_texture2d>(constant::g_rpass_depth_stencil_texture);
		const auto l_depth_stencil_view = get_shared_resource_throw<graphic::bc_depth_stencil_view>(constant::g_rpass_depth_stencil_render_view);
		const auto l_render_target_texture = get_shared_resource_throw<graphic::bc_texture2d>(m_render_target_texture);
		const auto l_render_target_view = get_shared_resource_throw<graphic::bc_render_target_view>(m_render_target_view);
		const auto l_viewport = graphic::bc_viewport::default_config(l_render_target_texture.get_width(), l_render_target_texture.get_height());
		
		m_query_device_pipeline_state = p_context.m_render_system.create_device_pipeline_state
		(
			"light_flare_query_vs",
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			game::bc_vertex_type::pos,
			game::bc_blend_type::opaque,
			core::bc_enum::mask_or({ game::bc_depth_stencil_type::depth_less_no_write, game::bc_depth_stencil_type::stencil_off }),
			game::bc_rasterizer_type::fill_solid_cull_back,
			0xffffffff,
			{
				l_render_target_texture.get_format()
			},
			l_depth_stencil.get_format(),
			game::bc_multi_sample_type::c1_q1
		);
		m_query_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			m_query_device_pipeline_state.get(),
			l_viewport,
			{
				graphic::bc_render_target_view_parameter(l_render_target_view)
			},
			l_depth_stencil_view,
			{},
			{},
			{},
			{
				p_context.m_render_system.get_global_cbuffer()
			}
		);
		m_query_render_state = p_context.m_render_system.create_render_state
		(
			graphic::bc_primitive::trianglelist,
			m_cube_vb.get(),
			sizeof(core::bc_vector3f),
			0,
			m_cube_ib.get(),
			game::i16bit,
			m_cube_ib->get_byte_width() / sizeof(bcUINT16),
			0,
			{},
			{
				p_context.m_render_system.get_per_object_cbuffer()
			}
		);

		game::bc_render_state_resource_view_array l_render_state_resource_views;
		for(bcSIZE l_ite = 0; l_ite < s_per_draw_texture_count; ++l_ite)
		{
			l_render_state_resource_views[l_ite] = graphic::bc_resource_view_parameter(l_ite, graphic::bc_shader_type::pixel, m_flare_texture_parameters[l_ite]);
		}

		m_flare_device_pipeline_state = p_context.m_render_system.create_device_pipeline_state
		(
			"light_flare_vs",
			nullptr,
			nullptr,
			"light_flare_gs",
			"light_flare_ps",
			game::bc_vertex_type::none,
			game::bc_blend_type::add_preserve_alpha,
			core::bc_enum::mask_or({ game::bc_depth_stencil_type::depth_off, game::bc_depth_stencil_type::stencil_off }),
			game::bc_rasterizer_type::fill_solid_cull_none,
			0xffffffff,
			{
				l_render_target_texture.get_format()
			},
			l_depth_stencil.get_format(),
			game::bc_multi_sample_type::c1_q1
		);
		m_flare_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			m_flare_device_pipeline_state.get(),
			l_viewport,
			{
				graphic::bc_render_target_view_parameter(l_render_target_view)
			},
			l_depth_stencil_view,
			{
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, m_linear_sampler.get()),
			},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::geometry, m_flare_instances_view.get())
			},
			{},
			{
				p_context.m_render_system.get_global_cbuffer()
			}
		);
		m_flare_render_state = p_context.m_render_system.create_render_state
		(
			graphic::bc_primitive::pointlist,
			graphic::bc_buffer(),
			0,
			0,
			graphic::bc_buffer(),
			game::i16bit,
			0,
			0,
			std::move(l_render_state_resource_views),
			{}
		);
	}

	void bc_light_flare_pass::destroy(game::bc_render_system& p_render_system)
	{
		m_scene_light_query_result.clear();
		m_occlusion_queries.clear();
		m_ready_to_draw_instances.clear();

		m_linear_sampler.reset();
		m_cube_vb.reset();
		m_cube_ib.reset();
		m_flare_instances_buffer.reset();
		m_flare_instances_view.reset();
		m_query_device_pipeline_state.reset();
		m_query_render_pass_state.reset();
		m_query_render_state.reset();
		m_flare_device_pipeline_state.reset();
		m_flare_render_pass_state.reset();
		m_flare_render_state.reset();
	}

	void bc_light_flare_pass::_update_light_queries(const game::bc_render_pass_render_context& p_context)
	{
		p_context.m_render_thread.bind_render_pass_state(*m_query_render_pass_state);
		p_context.m_render_thread.bind_render_state(*m_query_render_state);

		for(_bc_light_flare_query_instance& l_query : m_occlusion_queries)
		{
			auto l_light_ite = std::find_if
			(
				std::cbegin(m_scene_light_query_result),
				std::cend(m_scene_light_query_result),
				[&](const game::bc_light_instance& p_light)
				{
					return l_query.m_light_id == p_light.get_id();
				}
			);

			if (l_light_ite == std::cend(m_scene_light_query_result))
			{
				l_query.m_free = true;
				l_query.m_light_id = 0;
			}
		}

		for (game::bc_light_instance& l_light : m_scene_light_query_result)
		{
			auto l_light_query_ite = std::find_if
			(
				std::cbegin(m_occlusion_queries),
				std::cend(m_occlusion_queries),
				[&](const _bc_light_flare_query_instance& p_query)
				{
					return p_query.m_light_id == l_light.get_id();
				}
			);

			if (l_light_query_ite == std::end(m_occlusion_queries))
			{
				auto* l_query = _find_free_query(p_context.m_render_system.get_device());
				l_query->m_free = false;
				l_query->m_light_id = l_light.get_id();

				_query_light(p_context, l_light, *l_query);
				
				continue;
			}

			if(!l_light_query_ite->m_free)
			{
				const auto l_query_result = p_context.m_render_thread.get_query_data(*l_light_query_ite->m_device_query);
				if(l_query_result.first && l_query_result.second > 0)
				{
					_bc_light_flare_instance l_flare;
					l_flare.m_light = &l_light;
					l_flare.m_intensity = l_light.get_flare()->get_intensity();
					l_flare.m_size = l_light.get_flare()->get_size();
					l_flare.m_texture_index = 0;
					l_flare.m_u0 = l_light.get_flare()->get_u0();
					l_flare.m_v0 = l_light.get_flare()->get_v0();
					l_flare.m_u1 = l_light.get_flare()->get_u1();
					l_flare.m_v1 = l_light.get_flare()->get_v1();

					switch (l_light.get_type())
					{
						case game::bc_light_type::point:
						{
							const auto* l_point_light = l_light.as_point_light();
							l_flare.m_position = l_point_light->get_position();
							l_flare.m_color = l_point_light->get_color();
							break;
						}
						case game::bc_light_type::spot:
						{
							const auto* l_spot_light = l_light.as_spot_light();
							l_flare.m_position = l_spot_light->get_position();
							l_flare.m_color = l_spot_light->get_color();
							l_flare.m_intensity *= l_spot_light->calculate_intensity(p_context.m_render_camera.get_position(), s_flare_intensity_spot_angle);
							break;
						}
						default:
						{
							BC_ASSERT(false);
							continue;
						}
					}

					const auto l_flare_distance = (l_flare.m_position - p_context.m_render_camera.get_position()).magnitude();
					l_flare.m_size *= _calculate_flare_size_ratio(l_flare_distance);

					m_ready_to_draw_instances.push_back(l_flare);
				}

				if (l_query_result.first)
				{
					_query_light(p_context, l_light, *l_light_query_ite);
				}
			}
		}

		p_context.m_render_thread.unbind_render_state(*m_query_render_state);
		p_context.m_render_thread.unbind_render_pass_state(*m_query_render_pass_state);
	}

	void bc_light_flare_pass::_draw_flares(const game::bc_render_pass_render_context& p_context)
	{
		core::bc_unordered_map_frame<game::bc_mesh_material*, core::bc_vector_frame<_bc_light_flare_instance*>> l_instance_groups;

		for (_bc_light_flare_instance& l_instance : m_ready_to_draw_instances)
		{
			auto& l_group = l_instance_groups[l_instance.m_light->get_flare()->get_mask_material()];
			l_group.push_back(&l_instance);
		}

		for (auto& l_texture_mask_parameter : m_flare_texture_parameters)
		{
			l_texture_mask_parameter.set_as_resource_view(graphic::bc_resource_view());
		}

		bcUINT32 l_used_texture_slot = 0;
		for (auto& l_group : l_instance_groups)
		{
			m_flare_texture_parameters[l_used_texture_slot].set_as_resource_view(l_group.first->get_diffuse_map_view());

			for (_bc_light_flare_instance* l_group_instance : l_group.second)
			{
				l_group_instance->m_texture_index = l_used_texture_slot;
			}

			++l_used_texture_slot;
		}

		core::bc_vector_frame<_bc_light_flare_draw_instance> l_draw_call_buffer(s_per_draw_flare_count);
		const auto l_draw_call_count = static_cast<bcUINT32>(std::ceilf(m_ready_to_draw_instances.size() / static_cast<bcFLOAT>(s_per_draw_flare_count)));

		p_context.m_render_thread.bind_render_pass_state(*m_flare_render_pass_state);
		p_context.m_render_thread.bind_render_state(*m_flare_render_state);

		for (auto l_draw_call = 0U; l_draw_call < l_draw_call_count; ++l_draw_call)
		{
			const auto l_instance_count_drawn = l_draw_call * s_per_draw_flare_count;
			const auto l_instance_count_to_draw = std::min(m_ready_to_draw_instances.size(), (l_draw_call + 1) * s_per_draw_flare_count);
			std::transform
			(
				std::begin(m_ready_to_draw_instances) + l_instance_count_drawn,
				std::begin(m_ready_to_draw_instances) + l_instance_count_to_draw,
				std::begin(l_draw_call_buffer),
				[](const _bc_light_flare_instance& p_flare_instance)
				{
					_bc_light_flare_draw_instance l_flare_draw_instance;
					l_flare_draw_instance.m_position = p_flare_instance.m_position;
					l_flare_draw_instance.m_color = p_flare_instance.m_color;
					l_flare_draw_instance.m_size = p_flare_instance.m_size;
					l_flare_draw_instance.m_intensity = p_flare_instance.m_intensity;
					l_flare_draw_instance.m_texture_index = p_flare_instance.m_texture_index;
					l_flare_draw_instance.m_u0 = p_flare_instance.m_u0;
					l_flare_draw_instance.m_v0 = p_flare_instance.m_v0;
					l_flare_draw_instance.m_u1 = p_flare_instance.m_u1;
					l_flare_draw_instance.m_v1 = p_flare_instance.m_v1;

					return l_flare_draw_instance;
				}
			);

			p_context.m_render_thread.update_subresource(m_flare_instances_buffer.get(), 0, l_draw_call_buffer.data(), 0, 0);
			p_context.m_render_thread.draw_instanced(1, l_instance_count_to_draw, 0, 0);
		}

		p_context.m_render_thread.unbind_render_state(*m_flare_render_state);
		p_context.m_render_thread.unbind_render_pass_state(*m_flare_render_pass_state);
	}

	void bc_light_flare_pass::_query_light(const game::bc_render_pass_render_context& p_context, game::bc_light_instance& p_light, _bc_light_flare_query_instance& p_query)
	{
		core::bc_vector3f l_light_position;

		if(p_light.get_type() == game::bc_light_type::point)
		{
			const auto* l_point_light = p_light.as_point_light();
			l_light_position = l_point_light->get_position();
		}
		else if(p_light.get_type() == game::bc_light_type::spot)
		{
			const auto* l_spot_light = p_light.as_spot_light();
			l_light_position = l_spot_light->get_position();
		}
		else
		{
			BC_ASSERT(false);
			return;
		}

		const auto l_light_distance = (l_light_position - p_context.m_render_camera.get_position()).magnitude();
		const auto l_light_distance_surface_ratio = _calculate_flare_surface_ratio(l_light_distance);

		auto l_light_flare_surface_world = core::bc_matrix4f::scale_matrix_xyz
		(
			p_light.get_flare()->get_surface_width() * l_light_distance_surface_ratio,
			p_light.get_flare()->get_surface_height() * l_light_distance_surface_ratio,
			p_light.get_flare()->get_surface_depth() * l_light_distance_surface_ratio
		);
		l_light_flare_surface_world.set_translation(l_light_position);
		
		p_context.m_frame_renderer.update_per_object_cbuffer
		(
			p_context.m_render_thread, 
			p_context.m_render_camera, 
			game::bc_render_instance(l_light_flare_surface_world, game::bc_actor_render_group::unknown)
		);

		p_context.m_render_thread.start_query(*p_query.m_device_query);
		
		p_context.m_render_thread.draw_indexed
		(
			m_query_render_state->get_index_buffer_offset(), 
			m_query_render_state->get_index_count(), 
			m_query_render_state->get_vertex_buffer_offset()
		);

		p_context.m_render_thread.end_query(*p_query.m_device_query);
	}

	_bc_light_flare_query_instance* bc_light_flare_pass::_find_free_query(graphic::bc_device& p_device)
	{
		auto l_free_query = std::find_if
		(
			std::cbegin(m_occlusion_queries),
			std::cend(m_occlusion_queries),
			[](const _bc_light_flare_query_instance& p_query)
			{
				return p_query.m_free;
			}
		);

		if(l_free_query == std::cend(m_occlusion_queries))
		{
			_bc_light_flare_query_instance l_query;
			l_query.m_free = true;
			l_query.m_light_id = 0;
			l_query.m_device_query = p_device.create_occlusion_query();
			
			m_occlusion_queries.push_back(std::move(l_query));
			l_free_query = std::rbegin(m_occlusion_queries).base();
		}

		return &*l_free_query;
	}

	bcFLOAT bc_light_flare_pass::_calculate_flare_size_ratio(bcFLOAT p_distance) const noexcept
	{
		return std::max(0.3f, std::min(1.0f, p_distance / m_flare_size_shrink_distance));
	}

	bcFLOAT bc_light_flare_pass::_calculate_flare_surface_ratio(bcFLOAT p_distance) const noexcept
	{
		return (std::max(0.f, p_distance - m_flare_surface_grow_distance) / m_flare_surface_grow_distance) + 1;
	}
}
