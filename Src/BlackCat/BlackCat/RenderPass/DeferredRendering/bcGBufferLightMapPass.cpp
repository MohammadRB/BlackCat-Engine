// [03/15/2019 MRB]

#include "BlackCat/BlackCatPCH.h"

#include "Core/Math/bcCoordinate.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "Core/Utility/bcEnumOperand.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcDefaultRenderThread.h"
#include "Game/System/Render/Light/bcLightManager.h"
#include "Game/System/Input/bcCameraFrustum.h"
#include "BlackCat/RenderPass/ShadowMap/bcCascadedShadowMapBufferContainer.h"
#include "BlackCat/RenderPass/DeferredRendering/bcGBufferLightMapPass.h"
#include "BlackCat/bcConstant.h"

namespace black_cat
{
	struct _bc_base_light_struct
	{
		core::bc_vector3f m_min_bound;
		core::bc_vector3f m_max_bound;
		bcINT32 m_shadow_map_index;
	};
	
	struct _bc_direct_light_struct : _bc_base_light_struct
	{
		core::bc_vector3f m_direction;
		core::bc_vector3f m_color;
		bcFLOAT m_intensity;
		core::bc_vector3f m_ambient_color;
		bcFLOAT m_ambient_intensity;
	};

	struct _bc_point_light_struct : _bc_base_light_struct
	{
		core::bc_vector3f m_position;
		bcFLOAT m_radius;
		core::bc_vector3f m_color;
		bcFLOAT m_intensity;
	};
	
	struct _bc_spot_light_struct : _bc_base_light_struct
	{
		core::bc_vector3f m_position;
		bcFLOAT m_angle;
		core::bc_vector3f m_direction;
		bcFLOAT m_length;
		core::bc_vector3f m_color;
		bcFLOAT m_intensity;
		bcFLOAT m_angle_cos;
	};

	struct _bc_cascade_shadow_map_struct
	{
		core::bc_array<core::bc_matrix4f, 4> m_view_projections;
		bcSIZE m_shadow_map_size;
		bcSIZE m_shadow_map_count;
		core::bc_array<bcSIZE, 4> m_cascade_sizes;
	};

	struct _bc_parameters_cbuffer
	{
		BC_CBUFFER_ALIGN
		bcUINT32 m_direct_lights_num;
		bcUINT32 m_point_lights_num;
		bcUINT32 m_spot_lights_num;
		BC_CBUFFER_ALIGN
		core::bc_matrix4f m_view_proj_inv;
	};
	
	bc_gbuffer_light_map_pass::bc_gbuffer_light_map_pass(constant::bc_render_pass_variable_t p_csm_buffers_container, constant::bc_render_pass_variable_t p_output_texture)
		: m_csm_buffers_container_share_slot(p_csm_buffers_container),
		m_output_texture_share_slot(p_output_texture)
	{
	}

	void bc_gbuffer_light_map_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
		auto& l_device = p_render_system.get_device();

		m_device_compute_state = p_render_system.create_device_compute_state("gbuffer_light_map");

		auto l_resource_configure = graphic::bc_graphic_resource_builder();

		auto l_direct_lights_buffer_config = l_resource_configure
			.as_resource()
			.as_buffer
			(
				m_num_direct_lights,
				sizeof(_bc_direct_light_struct),
				graphic::bc_resource_usage::gpu_rw,
				graphic::bc_resource_view_type::shader
			)
			.with_structured_buffer(sizeof(_bc_direct_light_struct))
			.as_buffer();
		auto l_point_lights_buffer_config = l_resource_configure
			.as_resource()
			.as_buffer
			(
				m_num_point_lights,
				sizeof(_bc_point_light_struct),
				graphic::bc_resource_usage::gpu_rw,
				graphic::bc_resource_view_type::shader
			)
			.with_structured_buffer(sizeof(_bc_point_light_struct))
			.as_buffer();
		auto l_spot_lights_buffer_config = l_resource_configure
			.as_resource()
			.as_buffer
			(
				m_num_spot_lights,
				sizeof(_bc_spot_light_struct),
				graphic::bc_resource_usage::gpu_rw,
				graphic::bc_resource_view_type::shader
			)
			.with_structured_buffer(sizeof(_bc_spot_light_struct))
			.as_buffer();
		auto l_shadow_maps_buffer_config = l_resource_configure
			.as_resource()
			.as_buffer
			(
				m_shader_shadow_map_count,
				sizeof(_bc_cascade_shadow_map_struct),
				graphic::bc_resource_usage::gpu_rw,
				graphic::bc_resource_view_type::shader
			)
			.with_structured_buffer(sizeof(_bc_cascade_shadow_map_struct))
			.as_buffer();

		auto l_direct_lights_buffer_view_config = l_resource_configure.as_resource_view()
			.as_buffer_view(graphic::bc_format::unknown)
			.as_shader_view(0, m_num_direct_lights)
			.as_structured_buffer();
		auto l_point_lights_buffer_view_config = l_resource_configure.as_resource_view()
			.as_buffer_view(graphic::bc_format::unknown)
			.as_shader_view(0, m_num_point_lights)
			.as_structured_buffer();
		auto l_spot_lights_buffer_view_config = l_resource_configure.as_resource_view()
			.as_buffer_view(graphic::bc_format::unknown)
			.as_shader_view(0, m_num_spot_lights)
			.as_structured_buffer();
		auto l_shadow_maps_buffer_view_config = l_resource_configure.as_resource_view()
			.as_buffer_view(graphic::bc_format::unknown)
			.as_shader_view(0, m_shader_shadow_map_count)
			.as_structured_buffer();

		m_direct_lights_buffer = l_device.create_buffer(l_direct_lights_buffer_config, nullptr);
		m_point_lights_buffer = l_device.create_buffer(l_point_lights_buffer_config, nullptr);
		m_spot_lights_buffer = l_device.create_buffer(l_spot_lights_buffer_config, nullptr);
		m_shadow_maps_buffer = l_device.create_buffer(l_shadow_maps_buffer_config, nullptr);

		m_direct_lights_buffer_view = l_device.create_resource_view(m_direct_lights_buffer.get(), l_direct_lights_buffer_view_config);
		m_point_lights_buffer_view = l_device.create_resource_view(m_point_lights_buffer.get(), l_point_lights_buffer_view_config);
		m_spot_lights_buffer_view = l_device.create_resource_view(m_spot_lights_buffer.get(), l_spot_lights_buffer_view_config);
		m_shadow_maps_buffer_view = l_device.create_resource_view(m_shadow_maps_buffer.get(), l_shadow_maps_buffer_view_config);

		graphic::bc_device_parameters l_old_parameters
		(
			0,
			0,
			graphic::bc_format::unknown,
			graphic::bc_texture_ms_config(1, 0)
		);
		graphic::bc_device_parameters l_new_parameters
		(
			l_device.get_back_buffer_width(),
			l_device.get_back_buffer_height(),
			l_device.get_back_buffer_format(),
			l_device.get_back_buffer_texture().get_sample_count()
		);

		after_reset(game::bc_render_pass_reset_param(p_render_system, l_device, l_old_parameters, l_new_parameters));
	}

	void bc_gbuffer_light_map_pass::update(const game::bc_render_pass_update_param& p_param)
	{
	}

	void bc_gbuffer_light_map_pass::initialize_frame(const game::bc_render_pass_render_param& p_param)
	{
		if (m_lights_query.is_executed())
		{
			m_lights = m_lights_query.get().get_lights();
		}

		m_lights_query = core::bc_get_service<core::bc_query_manager>()->queue_query
		(
			game::bc_scene_light_query
			(
				core::bc_enum::or({ game::bc_light_type::direct, game::bc_light_type::point, game::bc_light_type::spot })
			).with(game::bc_camera_frustum(p_param.m_current_camera))
		);
	}

	void bc_gbuffer_light_map_pass::execute(const game::bc_render_pass_render_param& p_param)
	{
		core::bc_vector_frame<_bc_direct_light_struct> l_direct_lights;
		core::bc_vector_frame<_bc_point_light_struct> l_point_lights;
		core::bc_vector_frame<_bc_spot_light_struct> l_spot_lights;
		core::bc_vector_frame<_bc_cascade_shadow_map_struct> l_shadow_maps;

		l_direct_lights.reserve(m_num_direct_lights);
		l_point_lights.reserve(m_num_point_lights);
		l_spot_lights.reserve(m_num_spot_lights);
		l_shadow_maps.reserve(m_shader_shadow_map_count);
		
		for (game::bc_light_instance& l_light : m_lights)
		{
			switch (l_light.get_type())
			{
			case game::bc_light_type::direct:
			{
				auto* l_direct_light = l_light.as_direct_light();
				_bc_direct_light_struct l_direct_light_cbuffer;

				l_direct_light_cbuffer.m_min_bound = l_light.get_min_bound();
				l_direct_light_cbuffer.m_max_bound = l_light.get_max_bound();
				l_direct_light_cbuffer.m_shadow_map_index = -1;
				l_direct_light_cbuffer.m_direction = l_direct_light->get_direction();
				l_direct_light_cbuffer.m_color = l_direct_light->get_color();
				l_direct_light_cbuffer.m_intensity = l_direct_light->get_intensity();
				l_direct_light_cbuffer.m_ambient_color = l_direct_light->get_ambient_color();
				l_direct_light_cbuffer.m_ambient_intensity = l_direct_light->get_ambient_intensity();

				l_direct_lights.push_back(l_direct_light_cbuffer);
				break;
			}
			case game::bc_light_type::point:
			{
				auto l_point_light = l_light.as_point_light();
				_bc_point_light_struct l_point_light_cbuffer;

				l_point_light_cbuffer.m_min_bound = l_light.get_min_bound();
				l_point_light_cbuffer.m_max_bound = l_light.get_max_bound();
				l_point_light_cbuffer.m_shadow_map_index = -1;
				l_point_light_cbuffer.m_position = l_point_light->get_position();
				l_point_light_cbuffer.m_radius = l_point_light->get_radius();
				l_point_light_cbuffer.m_color = l_point_light->get_color();
				l_point_light_cbuffer.m_intensity = l_point_light->get_intensity();

				l_point_lights.push_back(l_point_light_cbuffer);
				break;
			}
			case game::bc_light_type::spot:
			{
				auto l_spot_light = l_light.as_spot_light();
				_bc_spot_light_struct l_spot_light_cbuffer;

				l_spot_light_cbuffer.m_min_bound = l_light.get_min_bound();
				l_spot_light_cbuffer.m_max_bound = l_light.get_max_bound();
				l_spot_light_cbuffer.m_shadow_map_index = -1;
				l_spot_light_cbuffer.m_position = l_spot_light->get_position();
				l_spot_light_cbuffer.m_angle = l_spot_light->get_angle();
				l_spot_light_cbuffer.m_direction = l_spot_light->get_direction();
				l_spot_light_cbuffer.m_length = l_spot_light->get_length();
				l_spot_light_cbuffer.m_color = l_spot_light->get_color();
				l_spot_light_cbuffer.m_intensity = l_spot_light->get_intensity();
				l_spot_light_cbuffer.m_angle_cos = std::cos(core::bc_to_radian(l_spot_light_cbuffer.m_angle / 2));

				l_spot_lights.push_back(l_spot_light_cbuffer);
				break;
			}
			}
		}

		bcAssert(l_direct_lights.size() <= m_num_direct_lights);
		bcAssert(l_point_lights.size() <= m_num_point_lights);
		bcAssert(l_spot_lights.size() <= m_num_spot_lights);

		// Associate light depth maps to their structures
		auto* l_shadow_map_buffer_container = get_shared_resource<bc_cascaded_shadow_map_buffer_container>(m_csm_buffers_container_share_slot);

		if (l_shadow_map_buffer_container != nullptr)
		{
			bcAssert(l_shadow_map_buffer_container->size() <= m_shader_shadow_map_array_count);

			for (bcSIZE l_shadow_map_buffer_ite = 0, l_end = l_shadow_map_buffer_container->size(); l_shadow_map_buffer_ite < l_end; ++l_shadow_map_buffer_ite)
			{
				auto& l_shadow_map_buffer_entry = l_shadow_map_buffer_container->get(l_shadow_map_buffer_ite);

				bcAssert(l_shadow_map_buffer_entry.second.m_cascade_sizes.size() <= m_shader_shadow_map_cascade_count);
				bcAssert(l_shadow_map_buffer_entry.second.m_view_projections.size() <= m_shader_shadow_map_cascade_count);

				auto l_direct_light_ite = std::find_if(std::cbegin(l_direct_lights), std::cend(l_direct_lights), [&](const _bc_direct_light_struct& p_direct_light)
					{
						return p_direct_light.m_direction == l_shadow_map_buffer_entry.first;
					});

				bcAssert(l_direct_light_ite != std::cend(l_direct_lights));

				l_direct_light_ite->m_shadow_map_index = l_shadow_map_buffer_ite;

				_bc_cascade_shadow_map_struct l_shadow_map_struct;
				l_shadow_map_struct.m_shadow_map_size = l_shadow_map_buffer_entry.second.m_shadow_map_size;
				l_shadow_map_struct.m_shadow_map_count = l_shadow_map_buffer_entry.second.m_shadow_map_count;
				std::copy
				(
					std::begin(l_shadow_map_buffer_entry.second.m_cascade_sizes),
					std::end(l_shadow_map_buffer_entry.second.m_cascade_sizes),
					std::begin(l_shadow_map_struct.m_cascade_sizes)
				);
				std::transform
				(
					std::begin(l_shadow_map_buffer_entry.second.m_view_projections),
					std::end(l_shadow_map_buffer_entry.second.m_view_projections),
					std::begin(l_shadow_map_struct.m_view_projections),
					[](const core::bc_matrix4f& p_mat) { return p_mat.transpose(); }
				);

				l_shadow_maps.push_back(std::move(l_shadow_map_struct));
				m_shadow_map_parameters[l_shadow_map_buffer_ite].set_as_resource_view(l_shadow_map_buffer_entry.second.m_resource_view);
			}
		}

		p_param.m_render_thread.start();

		_bc_parameters_cbuffer l_parameters_cbuffer_data
		{
			l_direct_lights.size(),
			l_point_lights.size(),
			l_spot_lights.size(),
			(p_param.m_render_camera.get_view() * p_param.m_render_camera.get_projection()).inverse().transpose()
		};

		p_param.m_render_thread.update_subresource(m_parameters_cbuffer.get(), 0, &l_parameters_cbuffer_data, 0, 0);
		p_param.m_render_thread.update_subresource(m_direct_lights_buffer.get(), 0, l_direct_lights.data(), 0, 0);
		p_param.m_render_thread.update_subresource(m_point_lights_buffer.get(), 0, l_point_lights.data(), 0, 0);
		p_param.m_render_thread.update_subresource(m_spot_lights_buffer.get(), 0, l_spot_lights.data(), 0, 0);
		p_param.m_render_thread.update_subresource(m_shadow_maps_buffer.get(), 0, l_shadow_maps.data(), 0, 0);
		
		p_param.m_render_thread.bind_compute_state(*m_compute_state.get());
		p_param.m_render_thread.dispatch
		(
			p_param.m_render_system.get_device().get_back_buffer_width() / m_shader_thread_group_size + 1,
			p_param.m_render_system.get_device().get_back_buffer_height() / m_shader_thread_group_size + 1,
			1
		);
		p_param.m_render_thread.unbind_compute_state(*m_compute_state.get());
		
		p_param.m_render_thread.finish();
	}

	void bc_gbuffer_light_map_pass::before_reset(const game::bc_render_pass_reset_param& p_param)
	{
	}

	void bc_gbuffer_light_map_pass::after_reset(const game::bc_render_pass_reset_param& p_param)
	{
		if
		(
			p_param.m_old_parameters.m_width == p_param.m_new_parameters.m_width
			&& 
			p_param.m_old_parameters.m_height == p_param.m_new_parameters.m_height
		)
		{
			return;
		}
		
		auto* l_depth_stencil = get_shared_resource<graphic::bc_texture2d>(constant::g_rpass_depth_stencil_texture);
		auto* l_diffuse_map = get_shared_resource<graphic::bc_texture2d>(constant::g_rpass_render_target_texture_1);
		auto* l_normal_map = get_shared_resource<graphic::bc_texture2d>(constant::g_rpass_render_target_texture_2);

		auto l_resource_configure = graphic::bc_graphic_resource_builder();

		auto l_parameters_cbuffer_config = l_resource_configure
			.as_resource()
			.as_buffer(1, sizeof(_bc_parameters_cbuffer), graphic::bc_resource_usage::gpu_rw, graphic::bc_resource_view_type::none)
			.as_constant_buffer();
		m_parameters_cbuffer = p_param.m_device.create_buffer(l_parameters_cbuffer_config, nullptr);

		auto l_depth_resource_view_config = l_resource_configure
			.as_resource_view()
			.as_texture_view(graphic::bc_format::R32_FLOAT)
			.as_tex2d_shader_view(0, 1)
			.on_texture2d();
		auto l_diffuse_map_view_config = l_resource_configure
			.as_resource_view()
			.as_texture_view(l_diffuse_map->get_format())
			.as_tex2d_shader_view(0, 1)
			.on_texture2d();
		auto l_normal_map_view_config = l_resource_configure
			.as_resource_view()
			.as_texture_view(l_normal_map->get_format())
			.as_tex2d_shader_view(0, 1)
			.on_texture2d();

		m_depth_stencil_view = p_param.m_device.create_resource_view(*l_depth_stencil, l_depth_resource_view_config);
		m_diffuse_map_view = p_param.m_device.create_resource_view(*l_diffuse_map, l_diffuse_map_view_config);
		m_normal_map_view = p_param.m_device.create_resource_view(*l_normal_map, l_normal_map_view_config);

		auto l_pcf_sampler_config = l_resource_configure.as_resource().as_sampler_state
		(
			graphic::bc_filter::comparison_min_mag_linear_mip_point,
			graphic::bc_texture_address_mode::clamp,
			graphic::bc_texture_address_mode::clamp,
			graphic::bc_texture_address_mode::clamp
		).with_comparison(graphic::bc_comparison_func::less).as_sampler_state();

		m_pcf_sampler = p_param.m_device.create_sampler_state(l_pcf_sampler_config);

		auto l_output_texture_config = l_resource_configure
			.as_resource()
			.as_texture2d
			(
				p_param.m_new_parameters.m_width,
				p_param.m_new_parameters.m_height,
				false,
				1,
				graphic::bc_format::R8G8B8A8_UNORM,
				graphic::bc_resource_usage::gpu_rw,
				core::bc_enum:: or ({ graphic::bc_resource_view_type::shader, graphic::bc_resource_view_type::unordered }))
			.as_normal_texture();
		auto l_output_texture_view_config = l_resource_configure
			.as_resource_view()
			.as_texture_view(l_output_texture_config.get_format())
			.as_tex2d_unordered_shader_view(0)
			.on_texture2d();

		m_output_texture = p_param.m_device.create_texture2d(l_output_texture_config, nullptr);
		m_output_texture_unordered_view = p_param.m_device.create_resource_view(m_output_texture.get(), l_output_texture_view_config);

		game::bc_compute_state_resource_view_array l_resource_views = 
		{
			graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, m_depth_stencil_view.get()),
			graphic::bc_resource_view_parameter(1, graphic::bc_shader_type::compute, m_diffuse_map_view.get()),
			graphic::bc_resource_view_parameter(2, graphic::bc_shader_type::compute, m_normal_map_view.get()),
			graphic::bc_resource_view_parameter(3, graphic::bc_shader_type::compute, m_direct_lights_buffer_view.get()),
			graphic::bc_resource_view_parameter(4, graphic::bc_shader_type::compute, m_point_lights_buffer_view.get()),
			graphic::bc_resource_view_parameter(5, graphic::bc_shader_type::compute, m_spot_lights_buffer_view.get()),
			graphic::bc_resource_view_parameter(6, graphic::bc_shader_type::compute, m_shadow_maps_buffer_view.get())
		};
		for(bcSIZE l_shadow_map_ite = 0; l_shadow_map_ite < m_shadow_map_parameters.size(); ++l_shadow_map_ite)
		{
			auto l_resource_view_index = 7 + l_shadow_map_ite;
			auto l_resource_view_parameter = graphic::bc_resource_view_parameter
			(
				l_resource_view_index, 
				graphic::bc_shader_type::compute, 
				m_shadow_map_parameters[l_shadow_map_ite]
			);
			l_resource_views[l_resource_view_index] = l_resource_view_parameter;
		}

		m_compute_state = p_param.m_render_system.create_compute_state
		(
			m_device_compute_state.get(),
			{
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::compute, m_pcf_sampler.get())
			},
			std::move(l_resource_views),
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, m_output_texture_unordered_view.get())
			},
			{
				p_param.m_render_system.get_global_cbuffer(),
				graphic::bc_constant_buffer_parameter(1, graphic::bc_shader_type::compute, m_parameters_cbuffer.get())
			}
		);

		share_resource(constant::g_rpass_depth_stencil_read_view, m_depth_stencil_view.get());
		share_resource(constant::g_rpass_render_target_read_view_1, m_diffuse_map_view.get());
		share_resource(constant::g_rpass_render_target_read_view_2, m_normal_map_view.get());
		share_resource(m_output_texture_share_slot, m_output_texture.get());
	}

	void bc_gbuffer_light_map_pass::destroy(game::bc_render_system& p_render_system)
	{
		m_compute_state.reset();
		m_device_compute_state.reset();

		m_output_texture_unordered_view.reset();
		m_output_texture.reset();

		m_direct_lights_buffer.reset();
		m_direct_lights_buffer_view.reset();
		m_point_lights_buffer.reset();
		m_point_lights_buffer_view.reset();
		m_spot_lights_buffer.reset();
		m_spot_lights_buffer_view.reset();
		m_shadow_maps_buffer.reset();
		m_shadow_maps_buffer_view.reset();

		m_parameters_cbuffer.reset();

		m_pcf_sampler.reset();

		m_normal_map_view.reset();
		m_diffuse_map_view.reset();
		m_depth_stencil_view.reset();
	}
}