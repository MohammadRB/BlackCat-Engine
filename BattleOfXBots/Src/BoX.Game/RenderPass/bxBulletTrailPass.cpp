// [03/01/2022 MRB]

#include "Core/Content/bcContentManager.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/System/Input/bcFileSystem.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcDefaultRenderThread.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Scene/bcScene.h"
#include "App/bcConstant.h"
#include "BoX.Game/RenderPass/bxBulletTrailPass.h"

namespace box
{
	struct _bx_bullet_trail_cbuffer
	{
		BC_CBUFFER_ALIGN
		bcFLOAT m_length;
		bcFLOAT m_width;
	};

	bx_bullet_trail_pass::bx_bullet_trail_pass(game::bc_render_pass_variable_t p_render_target_texture,
		game::bc_render_pass_variable_t p_render_target_view,
		core::bc_estring_view p_texture_path)
		: m_render_target_texture(p_render_target_texture),
		m_render_target_view(p_render_target_view),
		m_texture_path(p_texture_path)
	{
	}

	void bx_bullet_trail_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
		auto& l_content_manager = *core::bc_get_service<core::bc_content_manager>();
		auto& l_device = p_render_system.get_device();
		auto& l_device_swap_buffer = p_render_system.get_device_swap_buffer();
		auto& l_game_system = *core::bc_get_service<game::bc_game_system>();

		m_texture = l_content_manager.load<graphic::bc_texture2d_content>
		(
			core::bc_alloc_type::program,
			l_game_system.get_file_system().get_content_path(m_texture_path.data()),
			{},
			core::bc_content_loader_parameter()
		);

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
		const auto l_texture_view_config = graphic::bc_graphic_resource_builder()
			.as_resource_view()
			.as_texture_view(m_texture->get_resource().get_format())
			.as_tex2d_shader_view(0, 1)
			.on_texture2d();
		const auto l_cbuffer_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_buffer(1, sizeof(_bx_bullet_trail_cbuffer), graphic::bc_resource_usage::gpu_rw)
			.as_constant_buffer();
		const auto l_cbuffer_params = _bx_bullet_trail_cbuffer
		{
			bc_get_global_config().get_scene_global_scale() * .8f,
			bc_get_global_config().get_scene_global_scale() * .1f
		};
		const auto l_cbuffer_data = graphic::bc_subresource_data(&l_cbuffer_params, 0, 0);
		
		m_linear_sampler = l_device.create_sampler_state(l_linear_sampler_config);
		m_texture_view = l_device.create_resource_view(m_texture->get_resource(), l_texture_view_config);
		m_cbuffer = l_device.create_buffer(l_cbuffer_config, &l_cbuffer_data);

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

	void bx_bullet_trail_pass::update(const game::bc_render_pass_update_context& p_context)
	{
	}

	void bx_bullet_trail_pass::initialize_frame(const game::bc_render_pass_render_context& p_context)
	{
		if(m_query_result.is_executed())
		{
			auto l_bullets = m_query_result.get().get_result().release_as<core::bc_vector<_bx_bullet>>();
			m_bullets.assign(std::begin(l_bullets), std::end(l_bullets));
		}

		auto l_scene_query = game::bc_scene_query([](const game::bc_scene_query_context& p_context)
		{
			if (!p_context.m_scene)
			{
				return core::bc_any(core::bc_vector<_bx_bullet>());
			}

			const auto l_bullets = p_context.m_scene->get_bullet_manager().get_bullets();
			core::bc_vector<_bx_bullet> l_copied_bullets;
			l_copied_bullets.reserve(l_bullets.size());

			std::transform
			(
				std::begin(l_bullets),
				std::end(l_bullets),
				std::back_inserter(l_copied_bullets),
				[](const game::bc_bullet& l_bullet)
				{
					return _bx_bullet{ l_bullet.get_position(), l_bullet.get_direction() };
				}
			);

			return core::bc_any(std::move(l_copied_bullets));
		});
		m_query_result = p_context.m_query_manager.queue_query(std::move(l_scene_query));
	}

	void bx_bullet_trail_pass::execute(const game::bc_render_pass_render_context& p_context)
	{
		if(m_bullets.empty())
		{
			return;
		}

		p_context.m_render_thread.start();

		_update_bullets_buffer(p_context);

		p_context.m_render_thread.bind_render_pass_state(*m_render_pass_state);

		p_context.m_render_thread.get_pipeline().bind_ia_primitive_topology(graphic::bc_primitive::pointlist);
		p_context.m_render_thread.get_pipeline().pipeline_apply_states(graphic::bc_pipeline_stage::input_assembler_stage);
		p_context.m_render_thread.draw_instanced(1, m_bullets.size(), 0, 0);
		p_context.m_render_thread.get_pipeline().unbind_ia_primitive_topology();
		p_context.m_render_thread.get_pipeline().pipeline_apply_states(graphic::bc_pipeline_stage::input_assembler_stage);

		p_context.m_render_thread.unbind_render_pass_state(*m_render_pass_state);

		p_context.m_render_thread.finish();
	}

	void bx_bullet_trail_pass::before_reset(const game::bc_render_pass_reset_context& p_context)
	{
	}

	void bx_bullet_trail_pass::after_reset(const game::bc_render_pass_reset_context& p_context)
	{
		const auto l_depth_stencil = get_shared_resource_throw<graphic::bc_texture2d>(constant::g_rpass_depth_stencil_texture);
		const auto l_depth_stencil_view = get_shared_resource_throw<graphic::bc_depth_stencil_view>(constant::g_rpass_depth_stencil_render_view);
		const auto l_render_target = get_shared_resource_throw<graphic::bc_texture2d>(m_render_target_texture);
		const auto l_render_target_view = get_shared_resource_throw<graphic::bc_render_target_view>(m_render_target_view);
		const auto l_viewport = graphic::bc_viewport::default_config(l_render_target.get_width(), l_render_target.get_height());

		m_device_pipeline_state = p_context.m_render_system.create_device_pipeline_state
		(
			"bullet_trail_vs",
			nullptr,
			nullptr,
			"bullet_trail_gs",
			"bullet_trail_ps",
			game::bc_vertex_type::none,
			game::bc_blend_type::add_overwrite_alpha,
			core::bc_enum::mask_or({ game::bc_depth_stencil_type::depth_less_no_write, game::bc_depth_stencil_type::stencil_off }),
			game::bc_rasterizer_type::fill_solid_cull_none,
			0xffffffff,
			{
				l_render_target.get_format()
			},
			l_depth_stencil.get_format(),
			game::bc_multi_sample_type::c1_q1
		);
		m_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			*m_device_pipeline_state,
			l_viewport,
			{
				graphic::bc_render_target_view_parameter(l_render_target_view)
			},
			l_depth_stencil_view,
			{
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, *m_linear_sampler)
			},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::geometry, m_bullets_parameter_link),
				graphic::bc_resource_view_parameter(1, graphic::bc_shader_type::pixel, *m_texture_view)
			},
			{
			},
			{
				p_context.m_render_system.get_global_cbuffer(),
				graphic::bc_constant_buffer_parameter(1, graphic::bc_shader_type::geometry, *m_cbuffer)
			}
		);
	}

	void bx_bullet_trail_pass::destroy(game::bc_render_system& p_render_system)
	{
		m_linear_sampler.reset();
		m_texture.reset();
		m_texture_view.reset();
		m_cbuffer.reset();
		m_bullets_buffer.reset();
		m_bullets_buffer_view.reset();
		m_device_pipeline_state.reset();
		m_render_pass_state.reset();
	}

	void bx_bullet_trail_pass::_update_bullets_buffer(const game::bc_render_pass_render_context& p_context)
	{
		const auto l_buffer_capacity = m_bullets_buffer->is_valid() ? m_bullets_buffer->get_byte_width() / sizeof(_bx_bullet) : 0;

		if(!m_bullets_buffer->is_valid() || m_bullets.size() > l_buffer_capacity)
		{
			auto& l_device = p_context.m_render_system.get_device();
			const auto l_new_buffer_capacity = std::max(50U, m_bullets.size());
			const auto l_bullets_buffer_config = graphic::bc_graphic_resource_builder()
				.as_resource()
				.as_buffer(l_new_buffer_capacity, sizeof(_bx_bullet), graphic::bc_resource_usage::gpu_rw, graphic::bc_resource_view_type::shader)
				.with_structured_buffer(sizeof(_bx_bullet))
				.as_buffer();
			const auto l_bullets_buffer_view_config = graphic::bc_graphic_resource_builder()
				.as_resource_view()
				.as_buffer_view(graphic::bc_format::unknown)
				.as_shader_view(0, l_new_buffer_capacity)
				.as_structured_buffer();

			m_bullets.reserve(l_new_buffer_capacity);
			m_bullets_buffer = l_device.create_buffer(l_bullets_buffer_config, nullptr);
			m_bullets_buffer_view = l_device.create_resource_view(*m_bullets_buffer, l_bullets_buffer_view_config);

			m_bullets_buffer->set_debug_name("bullets_buffer");
			m_bullets_buffer_view->set_debug_name("bullets_buffer_view");
			m_bullets_parameter_link.set_as_resource_view(*m_bullets_buffer_view);
		}

		p_context.m_render_thread.update_subresource(*m_bullets_buffer, 0, m_bullets.data(), 0, 0);
	}
}
