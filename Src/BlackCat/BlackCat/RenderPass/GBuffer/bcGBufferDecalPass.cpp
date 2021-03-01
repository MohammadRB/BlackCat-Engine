// [02/25/2021 MRB]

#include "BlackCat/BlackCatPCH.h"

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
#include "BlackCat/RenderPass/GBuffer/bcGBufferDecalPass.h"
#include "BlackCat/bcConstant.h"

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
	
	void bc_gbuffer_decal_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
		auto& l_device = p_render_system.get_device();

		core::bc_array< core::bc_vector3f, 8 > l_cube_vertices
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
		core::bc_array< bcINT16, 36 > l_cube_indices
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

		auto l_cube_vb_config = graphic::bc_resource_builder()
			.as_buffer(8, sizeof(core::bc_vector3f), graphic::bc_resource_usage::gpu_r)
			.as_vertex_buffer();
		auto l_cube_ib_config = graphic::bc_resource_builder()
			.as_buffer(36, sizeof(bcINT16), graphic::bc_resource_usage::gpu_r)
			.as_index_buffer();
		auto l_cube_vb_data = graphic::bc_subresource_data(&l_cube_vertices[0], 0, 0);
		auto l_cube_ib_data = graphic::bc_subresource_data(&l_cube_indices[0], 0, 0);

		m_cube_vb = l_device.create_buffer(l_cube_vb_config, &l_cube_vb_data);
		m_cube_ib = l_device.create_buffer(l_cube_ib_config, &l_cube_ib_data);
				
		after_reset
		(
			game::bc_render_pass_reset_context
			(
				p_render_system, 
				l_device, 
				graphic::bc_device_parameters
				(
					0,
					0,
					graphic::bc_format::unknown,
					graphic::bc_texture_ms_config(1, 0)
				),
				graphic::bc_device_parameters
				(
					l_device.get_back_buffer_width(),
					l_device.get_back_buffer_height(),
					l_device.get_back_buffer_format(),
					l_device.get_back_buffer_texture().get_sample_count()
				)
			)
		);
	}

	void bc_gbuffer_decal_pass::update(const game::bc_render_pass_update_context& p_param)
	{
	}

	void bc_gbuffer_decal_pass::initialize_frame(const game::bc_render_pass_render_context& p_param)
	{
		if(m_decals_query.is_executed())
		{
			m_decals_buffer = m_decals_query.get().get_render_state_buffer();
		}

		m_decals_query = p_param.m_query_manager.queue_query
		(
			game::bc_main_camera_render_state_query
			(
				game::bc_actor_render_camera(p_param.m_update_camera),
				p_param.m_frame_renderer.create_buffer()
			).only< game::bc_decal_component >()
		);
	}

	void bc_gbuffer_decal_pass::execute(const game::bc_render_pass_render_context& p_param)
	{
		p_param.m_render_thread.start();
		p_param.m_render_thread.bind_render_pass_state(*m_render_pass_state);

		for (const auto& l_entry : m_decals_buffer.get_decal_instances())
		{
			auto& l_decal = *l_entry.first;
			auto* l_decal_render_state = l_decal.get_render_state();

			if(!l_decal_render_state)
			{
				_create_decal_render_state(p_param.m_render_system, l_decal);
				l_decal_render_state = l_decal.get_render_state();
			}

			p_param.m_render_thread.bind_render_state(*l_decal_render_state);
			
			for(auto& l_instance : l_entry.second)
			{
				p_param.m_frame_renderer.update_per_object_cbuffer(p_param.m_render_thread, p_param.m_render_camera, l_instance);

				p_param.m_render_thread.draw_indexed
				(
					l_decal_render_state->get_index_buffer_offset(),
					l_decal_render_state->get_index_count(),
					l_decal_render_state->get_vertex_buffer_offset()
				);
			}

			p_param.m_render_thread.unbind_render_state(*l_decal_render_state);
		}
		
		p_param.m_render_thread.unbind_render_pass_state(*m_render_pass_state);
		p_param.m_render_thread.finish();
	}

	void bc_gbuffer_decal_pass::before_reset(const game::bc_render_pass_reset_context& p_param)
	{
	}

	void bc_gbuffer_decal_pass::after_reset(const game::bc_render_pass_reset_context& p_param)
	{
		if
		(
			p_param.m_old_parameters.m_width == p_param.m_new_parameters.m_width &&
			p_param.m_old_parameters.m_height == p_param.m_new_parameters.m_height
		)
		{
			return;
		}
		
		const auto l_diffuse_map = get_shared_resource_throw<graphic::bc_texture2d>(constant::g_rpass_render_target_texture_1);
		const auto l_normal_map = get_shared_resource_throw<graphic::bc_texture2d>(constant::g_rpass_render_target_texture_2);
		const auto l_depth_buffer = get_shared_resource_throw<graphic::bc_texture2d>(constant::g_rpass_depth_stencil_texture);
		const auto l_diffuse_map_view = get_shared_resource_throw<graphic::bc_render_target_view>(constant::g_rpass_render_target_render_view_1);
		const auto l_normal_map_view = get_shared_resource_throw<graphic::bc_render_target_view>(constant::g_rpass_render_target_render_view_2);
		const auto l_depth_buffer_view = get_shared_resource_throw<graphic::bc_depth_stencil_view>(constant::g_rpass_depth_stencil_render_view);
		const auto l_viewport = graphic::bc_viewport::default_config(l_diffuse_map.get_width(), l_diffuse_map.get_height());
		
		m_device_pipeline_state = p_param.m_render_system.create_device_pipeline_state
		(
			"decal_vs",
			nullptr,
			nullptr,
			nullptr,
			"decal_ps",
			game::bc_vertex_type::pos,
			game::bc_blend_type::alpha,
			game::bc_depth_stencil_type::depth_less_no_write_stencil_off,
			game::bc_rasterizer_type::fill_solid_cull_back,
			0x1,
			{ l_diffuse_map.get_format(), l_normal_map.get_format() },
			l_depth_buffer.get_format(),
			game::bc_multi_sample_type::c1_q1
		);
		m_render_pass_state = p_param.m_render_system.create_render_pass_state
		(
			m_device_pipeline_state.get(),
			l_viewport,
			{ l_diffuse_map_view, l_normal_map_view },
			l_depth_buffer_view,
			{},
			{},
			{},
			{ p_param.m_render_system.get_global_cbuffer() }
		);
	}

	void bc_gbuffer_decal_pass::destroy(game::bc_render_system& p_render_system)
	{
		m_render_pass_state.reset();
		m_device_pipeline_state.reset();
		m_cube_vb.reset();
		m_cube_ib.reset();
	}

	void bc_gbuffer_decal_pass::_create_decal_render_state(game::bc_render_system& p_render_system, game::bc_decal& p_decal)
	{
		auto l_cbuffer_parameter = bc_cbuffer_parameter
		{
			p_decal.get_u0(),
			p_decal.get_v0(),
			p_decal.get_u1(),
			p_decal.get_v1(),
			p_decal.get_width(),
			p_decal.get_height(),
			static_cast<bcUINT32>(p_decal.get_group())
		};
		auto l_cbuffer_config = graphic::bc_resource_builder()
			.as_buffer(1, sizeof(bc_cbuffer_parameter), graphic::bc_resource_usage::gpu_r)
			.as_constant_buffer();
		auto l_cbuffer_data = graphic::bc_subresource_data(&l_cbuffer_parameter, 0, 0);
		auto l_cbuffer = p_render_system.get_device().create_buffer(l_cbuffer_config, &l_cbuffer_data);
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
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, p_decal.get_material().get_diffuse_map_view()),
				graphic::bc_resource_view_parameter(1, graphic::bc_shader_type::pixel, p_decal.get_material().get_normal_map_view()),
				graphic::bc_resource_view_parameter(2, graphic::bc_shader_type::pixel, p_decal.get_material().get_specular_map_view()),
			},
			{
				p_render_system.get_per_object_cbuffer(),
				graphic::bc_constant_buffer_parameter(1, graphic::bc_shader_type::pixel, l_cbuffer.get())
			}
		);

		p_decal.set_cbuffer(std::move(l_cbuffer));
		p_decal.set_render_state(std::move(l_render_state));
	}
}