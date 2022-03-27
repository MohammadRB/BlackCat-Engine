// [02/25/2021 MRB]

#include "App/AppPCH.h"

#include "Core/Container/bcArray.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "GraphicImp/Resource/View/bcDepthStencilView.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcDefaultRenderThread.h"
#include "Game/Object/Scene/Component/bcDecalComponent.h"
#include "App/RenderPass/GBuffer/bcGBufferDecalPass.h"
#include "App/bcConstant.h"

namespace black_cat
{
	struct bc_cbuffer_parameter
	{
		BC_CBUFFER_ALIGN
		bcFLOAT m_u0;
		bcFLOAT m_v0;
		bcFLOAT m_u1;
		bcFLOAT m_v1;
		BC_CBUFFER_ALIGN
		bcFLOAT m_width;
		bcFLOAT m_height;
		bcUINT32 m_group;
	};

	struct bc_decal_instance_parameter
	{
		core::bc_matrix4f m_world_inv;
		core::bc_matrix4f m_world_view_projection;
		bcFLOAT m_u0;
		bcFLOAT m_v0;
		bcFLOAT m_u1;
		bcFLOAT m_v1;
		bcUINT32 m_group;
	};
	
	void bc_gbuffer_decal_pass::initialize_resources(game::bc_render_system& p_render_system)
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
		auto l_instance_buffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer(s_max_instance_per_draw, sizeof(bc_decal_instance_parameter), graphic::bc_resource_usage::gpu_rw, graphic::bc_resource_view_type::shader)
			.with_structured_buffer(sizeof(bc_decal_instance_parameter))
			.as_buffer();
		auto l_instance_buffer_view_config = graphic::bc_graphic_resource_builder()
			.as_resource_view()
			.as_buffer_view(graphic::bc_format::unknown)
			.as_shader_view(0, s_max_instance_per_draw)
			.as_structured_buffer();
		auto l_cube_vb_data = graphic::bc_subresource_data(&l_cube_vertices[0], 0, 0);
		auto l_cube_ib_data = graphic::bc_subresource_data(&l_cube_indices[0], 0, 0);

		m_cube_vb = l_device.create_buffer(l_cube_vb_config, &l_cube_vb_data);
		m_cube_ib = l_device.create_buffer(l_cube_ib_config, &l_cube_ib_data);
		m_instance_buffer = l_device.create_buffer(l_instance_buffer_config, nullptr);
		m_instance_buffer_view = l_device.create_resource_view(m_instance_buffer.get(), l_instance_buffer_view_config);

		const auto l_point_sampler_config = graphic::bc_graphic_resource_builder()
		                                    .as_resource()
		                                    .as_sampler_state
		                                    (
			                                    graphic::bc_filter::min_mag_mip_point,
			                                    graphic::bc_texture_address_mode::wrap,
			                                    graphic::bc_texture_address_mode::wrap,
			                                    graphic::bc_texture_address_mode::wrap
		                                    )
		                                    .as_sampler_state();
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

		m_point_sampler = l_device.create_sampler_state(l_point_sampler_config);
		m_linear_sampler = l_device.create_sampler_state(l_linear_sampler_config);
				
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

	void bc_gbuffer_decal_pass::update(const game::bc_render_pass_update_context& p_context)
	{
	}

	void bc_gbuffer_decal_pass::initialize_frame(const game::bc_render_pass_render_context& p_context)
	{
		if(m_decals_query.is_executed())
		{
			m_decals_buffer = m_decals_query.get().get_render_state_buffer();
		}

		m_decals_query = p_context.m_query_manager.queue_query
		(
			game::bc_scene_decal_query
			(
				p_context.m_update_camera.get_position(),
				p_context.m_frame_renderer.create_buffer()
			)
		);
	}

	void bc_gbuffer_decal_pass::execute(const game::bc_render_pass_render_context& p_context)
	{
		decal_group_container l_decal_groups;
		decal_group_container l_non_culling_decals;

		for (const auto& l_entry : m_decals_buffer.get_decal_instances())
		{
			auto& l_instances = l_decal_groups[&l_entry.first->get_material()];
			l_instances.reserve(l_instances.size() + l_entry.second.size());

			std::transform
			(
				std::begin(l_entry.second),
				std::end(l_entry.second),
				std::back_inserter(l_instances),
				[&l_entry](game::bc_render_instance& p_instance)
				{
					return std::make_pair(l_entry.first.get(), &p_instance);
				}
			);
		}

		_render_decals(p_context, *m_render_pass_state, l_decal_groups, &l_non_culling_decals);
		
		if(!l_non_culling_decals.empty())
		{
			_render_decals(p_context, *m_render_pass_state_for_non_culling, l_non_culling_decals, nullptr);
		}

		if(m_draw_decal_bounds)
		{
			_render_decals(p_context, *m_render_pass_state_for_debug_bounds, l_decal_groups, nullptr);
		}
	}

	void bc_gbuffer_decal_pass::before_reset(const game::bc_render_pass_reset_context& p_context)
	{
	}

	void bc_gbuffer_decal_pass::after_reset(const game::bc_render_pass_reset_context& p_context)
	{
		if
		(
			p_context.m_old_parameters.m_width == p_context.m_new_parameters.m_width &&
			p_context.m_old_parameters.m_height == p_context.m_new_parameters.m_height
		)
		{
			return;
		}

		auto l_depth_stencil = get_shared_resource_throw<graphic::bc_texture2d>(constant::g_rpass_depth_stencil_texture);
		const auto l_depth_stencil_view = get_shared_resource_throw<graphic::bc_depth_stencil_view>(constant::g_rpass_depth_stencil_render_view);
		const auto l_diffuse_map = get_shared_resource_throw<graphic::bc_texture2d>(constant::g_rpass_render_target_texture_1);
		const auto l_normal_map = get_shared_resource_throw<graphic::bc_texture2d>(constant::g_rpass_render_target_texture_2);
		const auto l_specular_map = get_shared_resource_throw<graphic::bc_texture2d>(constant::g_rpass_render_target_texture_3);
		const auto l_diffuse_map_view = get_shared_resource_throw<graphic::bc_render_target_view>(constant::g_rpass_render_target_render_view_1);
		const auto l_normal_map_view = get_shared_resource_throw<graphic::bc_render_target_view>(constant::g_rpass_render_target_render_view_2);
		const auto l_specular_map_view = get_shared_resource_throw<graphic::bc_render_target_view>(constant::g_rpass_render_target_render_view_3);
		const auto l_viewport = graphic::bc_viewport::default_config(l_diffuse_map.get_width(), l_diffuse_map.get_height());

		auto l_depth_view_config = graphic::bc_graphic_resource_builder()
			.as_resource_view()
			.as_texture_view(graphic::bc_format::R24_UNORM_X8_TYPELESS)
			.as_tex2d_shader_view(0, 1)
			.on_texture2d();
		auto l_stencil_view_config = graphic::bc_graphic_resource_builder()
			.as_resource_view()
			.as_texture_view(graphic::bc_format::X24_TYPELESS_G8_UINT)
			.as_tex2d_shader_view(0, 1)
			.on_texture2d();
		m_depth_view = p_context.m_device.create_resource_view(l_depth_stencil, l_depth_view_config);
		m_stencil_view = p_context.m_device.create_resource_view(l_depth_stencil, l_stencil_view_config);
		
		m_device_pipeline_state = p_context.m_render_system.create_device_pipeline_state
		(
			"gbuffer_decal_vs",
			nullptr,
			nullptr,
			nullptr,
			"gbuffer_decal_ps",
			game::bc_vertex_type::pos,
			game::bc_blend_type::blending_overwrite_alpha,
			core::bc_enum::mask_or({ game::bc_depth_stencil_type::depth_off, game::bc_depth_stencil_type::stencil_off }),
			game::bc_rasterizer_type::fill_solid_cull_back,
			0x1,
			{
				l_diffuse_map.get_format(), l_normal_map.get_format(), l_specular_map.get_format()
			},
			l_depth_stencil.get_format(),
			game::bc_multi_sample_type::c1_q1
		);
		m_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			m_device_pipeline_state.get(),
			l_viewport,
			{
				graphic::bc_render_target_view_parameter(l_diffuse_map_view),
				graphic::bc_render_target_view_parameter(l_normal_map_view),
				graphic::bc_render_target_view_parameter(l_specular_map_view)
			},
			graphic::bc_depth_stencil_view(),
			{
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, m_point_sampler.get()),
				graphic::bc_sampler_parameter(1, graphic::bc_shader_type::pixel, m_linear_sampler.get())
			},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, m_depth_view.get()),
				graphic::bc_resource_view_parameter(1, graphic::bc_shader_type::pixel, m_stencil_view.get()),
				graphic::bc_resource_view_parameter(2, graphic::bc_shader_type::vertex, m_instance_buffer_view.get())
			},
			{
			},
			{
				p_context.m_render_system.get_global_cbuffer()
			}
		);
		
		m_device_pipeline_state_for_non_culling = p_context.m_render_system.create_device_pipeline_state
		(
			"gbuffer_decal_vs",
			nullptr,
			nullptr,
			nullptr,
			"gbuffer_non_culling_decal_ps",
			game::bc_vertex_type::pos,
			game::bc_blend_type::blending_preserve_alpha,
			core::bc_enum::mask_or({ game::bc_depth_stencil_type::depth_off, game::bc_depth_stencil_type::stencil_off }),
			game::bc_rasterizer_type::fill_solid_cull_front,
			0x1,
			{
				l_diffuse_map.get_format(), l_normal_map.get_format(), l_specular_map.get_format()
			},
			l_depth_stencil.get_format(),
			game::bc_multi_sample_type::c1_q1
		);
		m_render_pass_state_for_non_culling = p_context.m_render_system.create_render_pass_state
		(
			m_device_pipeline_state_for_non_culling.get(),
			l_viewport,
			{
				graphic::bc_render_target_view_parameter(l_diffuse_map_view),
				graphic::bc_render_target_view_parameter(l_normal_map_view),
				graphic::bc_render_target_view_parameter(l_specular_map_view)
			},
			graphic::bc_depth_stencil_view(),
			{
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, m_point_sampler.get()),
				graphic::bc_sampler_parameter(1, graphic::bc_shader_type::pixel, m_linear_sampler.get())
			},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, m_depth_view.get()),
				graphic::bc_resource_view_parameter(1, graphic::bc_shader_type::pixel, m_stencil_view.get()),
				graphic::bc_resource_view_parameter(2, graphic::bc_shader_type::vertex, m_instance_buffer_view.get())
			},
			{
			},
			{
				p_context.m_render_system.get_global_cbuffer()
			}
		);

		m_device_pipeline_state_for_debug_bounds = p_context.m_render_system.create_device_pipeline_state
		(
			"gbuffer_decal_vs",
			nullptr,
			nullptr,
			nullptr,
			"decal_debug_ps",
			game::bc_vertex_type::pos,
			game::bc_blend_type::blending_overwrite_alpha,
			core::bc_enum::mask_or({ game::bc_depth_stencil_type::depth_off, game::bc_depth_stencil_type::stencil_off }),
			game::bc_rasterizer_type::fill_solid_cull_back,
			0x1,
			{
				l_diffuse_map.get_format()
			},
			l_depth_stencil.get_format(),
			game::bc_multi_sample_type::c1_q1
		);
		m_render_pass_state_for_debug_bounds = p_context.m_render_system.create_render_pass_state
		(
			m_device_pipeline_state_for_debug_bounds.get(),
			l_viewport,
			{
				graphic::bc_render_target_view_parameter(l_diffuse_map_view),
				graphic::bc_render_target_view_parameter(l_normal_map_view),
				graphic::bc_render_target_view_parameter(l_specular_map_view)
			},
			graphic::bc_depth_stencil_view(),
			{
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, m_point_sampler.get()),
				graphic::bc_sampler_parameter(1, graphic::bc_shader_type::pixel, m_linear_sampler.get())
			},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, m_depth_view.get()),
				graphic::bc_resource_view_parameter(1, graphic::bc_shader_type::pixel, m_stencil_view.get()),
				graphic::bc_resource_view_parameter(2, graphic::bc_shader_type::vertex, m_instance_buffer_view.get())
			},
			{
			},
			{
				p_context.m_render_system.get_global_cbuffer()
			}
		);
	}

	void bc_gbuffer_decal_pass::destroy(game::bc_render_system& p_render_system)
	{
		m_render_states.clear();
		m_render_pass_state.reset();
		m_render_pass_state_for_non_culling.reset();
		m_device_pipeline_state.reset();
		m_device_pipeline_state_for_non_culling.reset();
		m_linear_sampler.reset();
		m_point_sampler.reset();
		m_depth_view.reset();
		m_stencil_view.reset();
		m_instance_buffer_view.reset();
		m_instance_buffer.reset();
		m_cube_vb.reset();
		m_cube_ib.reset();
	}

	void bc_gbuffer_decal_pass::_render_decals(const game::bc_render_pass_render_context& p_param,
		game::bc_render_pass_state& p_render_pass_state,
		decal_group_container& p_instances,
		decal_group_container* p_non_culling_instances)
	{
		const auto l_view_proj = p_param.m_render_camera.get_view() * p_param.m_render_camera.get_projection();
		
		p_param.m_render_thread.start();
		p_param.m_render_thread.bind_render_pass_state(p_render_pass_state);

		for (const auto& l_entry : p_instances)
		{
			const auto* l_decal_material = l_entry.first;

			auto l_render_state_ite = m_render_states.find(l_decal_material);
			if (l_render_state_ite == std::end(m_render_states))
			{
				_create_decal_render_state(p_param.m_render_system, *l_decal_material);
				l_render_state_ite = m_render_states.find(l_decal_material);
			}

			p_param.m_render_thread.bind_render_state(*l_render_state_ite->second);

			core::bc_vector_frame<bc_decal_instance_parameter> l_instance_buffer_data(s_max_instance_per_draw);
			auto l_instance_buffer_ite = 0U;
			auto l_instance_ite = std::begin(l_entry.second);
			auto l_instances_end = std::end(l_entry.second);

			while (l_instance_ite != l_instances_end)
			{
				game::bc_decal& l_decal = *l_instance_ite->first;
				game::bc_render_instance& l_render_instance = *l_instance_ite->second;
				const auto l_instance_world_inv = l_render_instance.get_transform().inverse();

				// if decal is not flat it can intersect with camera
				if (p_non_culling_instances && l_decal.get_depth() > 0.5f)
				{
					const auto l_local_camera_pos = l_instance_world_inv * core::bc_vector4f(p_param.m_render_camera.get_position(), 1);
					const auto l_local_camera_culling_distance = 0.5f + p_param.m_render_camera.get_near_clip();
					if (std::abs(l_local_camera_pos.x) < l_local_camera_culling_distance && 
						std::abs(l_local_camera_pos.y) < l_local_camera_culling_distance && 
						std::abs(l_local_camera_pos.z) < l_local_camera_culling_distance)
					{
						(*p_non_culling_instances)[l_entry.first].push_back(*l_instance_ite);
						++l_instance_ite;
						continue;
					}
				}

				l_instance_buffer_data[l_instance_buffer_ite].m_world_inv = l_instance_world_inv;
				l_instance_buffer_data[l_instance_buffer_ite].m_world_view_projection = l_render_instance.get_transform() * l_view_proj;
				l_instance_buffer_data[l_instance_buffer_ite].m_u0 = l_decal.get_u0();
				l_instance_buffer_data[l_instance_buffer_ite].m_v0 = l_decal.get_v0();
				l_instance_buffer_data[l_instance_buffer_ite].m_u1 = l_decal.get_u1();
				l_instance_buffer_data[l_instance_buffer_ite].m_v1 = l_decal.get_v1();
				l_instance_buffer_data[l_instance_buffer_ite].m_group = static_cast<bcUINT32>(l_render_instance.get_render_group());
				
				// Because matrices are put in regular buffer rather than cbuffer they must be stored in row major format
				if constexpr (!p_param.m_frame_renderer.need_matrix_transpose())
				{
					l_instance_buffer_data[l_instance_buffer_ite].m_world_inv.make_transpose();
					l_instance_buffer_data[l_instance_buffer_ite].m_world_view_projection.make_transpose();
				}

				++l_instance_buffer_ite;
				++l_instance_ite;

				if (l_instance_buffer_ite % s_max_instance_per_draw == 0)
				{
					p_param.m_render_thread.update_subresource(*m_instance_buffer, 0, l_instance_buffer_data.data(), 0, 0);
					p_param.m_render_thread.draw_indexed_instanced
					(
						l_render_state_ite->second->get_index_count(),
						l_instance_buffer_ite,
						0,
						0,
						0
					);

					l_instance_buffer_ite = 0;
				}
			}

			if (l_instance_buffer_ite > 0)
			{
				p_param.m_render_thread.update_subresource(*m_instance_buffer, 0, l_instance_buffer_data.data(), 0, 0);
				p_param.m_render_thread.draw_indexed_instanced
				(
					l_render_state_ite->second->get_index_count(),
					l_instance_buffer_ite,
					0,
					0,
					0
				);
			}

			p_param.m_render_thread.unbind_render_state(*l_render_state_ite->second);
		}

		p_param.m_render_thread.unbind_render_pass_state(p_render_pass_state);
		p_param.m_render_thread.finish();
	}

	void bc_gbuffer_decal_pass::_create_decal_render_state(game::bc_render_system& p_render_system, const game::bc_mesh_material& p_material)
	{
		auto l_render_state = p_render_system.create_render_state
		(
			graphic::bc_primitive::trianglelist,
			m_cube_vb.get(),
			sizeof(core::bc_vector3f),
			0,
			m_cube_ib.get(),
			game::i16bit,
			m_cube_ib->get_byte_width() / sizeof(bcUINT16),
			0,
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, p_material.get_diffuse_map_view()),
				graphic::bc_resource_view_parameter(1, graphic::bc_shader_type::pixel, p_material.get_normal_map_view()),
				graphic::bc_resource_view_parameter(2, graphic::bc_shader_type::pixel, p_material.get_specular_map_view()),
			},
			{
			}
		);

		m_render_states.insert(std::make_pair(&p_material, std::move(l_render_state)));
	}
}