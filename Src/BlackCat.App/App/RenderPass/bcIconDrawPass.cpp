// [01/21/2022 MRB]

#include "App/AppPCH.h"

#include "Core/Math/bcVector3f.h"
#include "Core/Content/bcContentManager.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "Core/Messaging/Event/bcEventManager.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/System/Render/bcDefaultRenderThread.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/bcGameSystem.h"
#include "Game/bcEvent.h"
#include "App/RenderPass/bcIconDrawPass.h"
#include "App/bcConstant.h"

namespace black_cat
{
	bc_icon_draw_pass::bc_icon_draw_pass(game::bc_render_pass_variable_t p_render_target_texture,
		game::bc_render_pass_variable_t p_render_target_view,
		std::initializer_list<std::string_view> p_icons,
		const bcECHAR* p_sprites_texture_name,
		bcUINT32 p_sprites_width,
		bcUINT32 p_sprites_count) noexcept
		: m_render_target_texture(p_render_target_texture),
		m_render_target_view(p_render_target_view),
		m_sprites_texture_name(p_sprites_texture_name),
		m_sprites_width(p_sprites_width),
		m_sprites_count_per_row(std::sqrtf(static_cast<bcFLOAT>(p_sprites_count))),
		m_editor_mode(false)
	{
		auto l_icon_ite = 0U;
		for (auto& l_icon : p_icons)
		{
			m_sprites_map.insert(std::make_pair(l_icon, _bc_sprite_texcoord{ l_icon_ite, 0, 0, 0, 0 }));
			++l_icon_ite;
		}

		m_editor_event_handle = core::bc_get_service<core::bc_event_manager>()->register_event_listener<game::bc_event_editor_mode>
		(
			core::bc_event_manager::delegate_type(*this, &bc_icon_draw_pass::_event_handler)
		);
	}

	bc_icon_draw_pass::bc_icon_draw_pass(bc_icon_draw_pass&& p_other) noexcept
		: m_render_target_texture(p_other.m_render_target_texture),
		m_render_target_view(p_other.m_render_target_view),
		m_sprites_texture_name(p_other.m_sprites_texture_name),
		m_sprites_map(std::move(p_other.m_sprites_map)),
		m_sprites_width(p_other.m_sprites_width),
		m_sprites_count_per_row(p_other.m_sprites_count_per_row),
		m_sprites_texture(std::move(p_other.m_sprites_texture)),
		m_sprites_texture_view(std::move(p_other.m_sprites_texture_view)),
		m_sprites_buffer(std::move(p_other.m_sprites_buffer)),
		m_sprites_buffer_view(std::move(p_other.m_sprites_buffer_view)),
		m_sprites_buffer_view_link(std::move(p_other.m_sprites_buffer_view_link)),
		m_linear_sampler(std::move(p_other.m_linear_sampler)),
		m_device_pipeline_state(std::move(p_other.m_device_pipeline_state)),
		m_render_pass_state(std::move(p_other.m_render_pass_state)),
		m_editor_mode(p_other.m_editor_mode),
		m_icons_query(std::move(p_other.m_icons_query)),
		m_sprites(std::move(p_other.m_sprites)),
		m_editor_event_handle(std::move(p_other.m_editor_event_handle))
	{
		m_editor_event_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_icon_draw_pass::_event_handler));
	}

	bc_icon_draw_pass& bc_icon_draw_pass::operator=(bc_icon_draw_pass&& p_other) noexcept
	{
		m_render_target_texture = p_other.m_render_target_texture;
		m_render_target_view = p_other.m_render_target_view;
		m_sprites_texture_name = p_other.m_sprites_texture_name;
		m_sprites_map = std::move(p_other.m_sprites_map);
		m_sprites_width = p_other.m_sprites_width;
		m_sprites_count_per_row = p_other.m_sprites_count_per_row;

		m_sprites_texture = std::move(p_other.m_sprites_texture);
		m_sprites_texture_view = std::move(p_other.m_sprites_texture_view);
		m_sprites_buffer = std::move(p_other.m_sprites_buffer);
		m_sprites_buffer_view = std::move(p_other.m_sprites_buffer_view);
		m_sprites_buffer_view_link = std::move(p_other.m_sprites_buffer_view_link);
		m_linear_sampler = std::move(p_other.m_linear_sampler);
		m_device_pipeline_state = std::move(p_other.m_device_pipeline_state);
		m_render_pass_state = std::move(p_other.m_render_pass_state);

		m_editor_mode = p_other.m_editor_mode;
		m_icons_query = std::move(p_other.m_icons_query);
		m_sprites = std::move(p_other.m_sprites);

		m_editor_event_handle = std::move(p_other.m_editor_event_handle);
		m_editor_event_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_icon_draw_pass::_event_handler));

		return *this;
	}

	void bc_icon_draw_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
		auto& l_device = p_render_system.get_device();
		auto& l_device_swap_buffer = p_render_system.get_device_swap_buffer();
		auto& l_content_manager = *core::bc_get_service<core::bc_content_manager>();
		auto& l_game_system = *core::bc_get_service<game::bc_game_system>();

		const auto l_sprites_texture_path = l_game_system.get_file_system().get_content_texture_path(m_sprites_texture_name);
		m_sprites_texture = l_content_manager.load<graphic::bc_texture2d_content>
		(
			core::bc_alloc_type::program,
			l_sprites_texture_path.c_str(),
			nullptr,
			core::bc_content_loader_parameter()
		);
		const auto l_sprites_texture_view_config = graphic::bc_graphic_resource_builder()
		                                   .as_resource_view()
		                                   .as_texture_view(m_sprites_texture->get_resource().get_format())
		                                   .as_tex2d_shader_view(0, -1)
		                                   .on_texture2d();
		m_sprites_texture_view = l_device.create_resource_view(m_sprites_texture->get_resource(), l_sprites_texture_view_config);

		auto l_linear_sampler_config = graphic::bc_graphic_resource_builder()
			.as_resource()
			.as_sampler_state
			(
				graphic::bc_filter::min_mag_mip_linear,
				graphic::bc_texture_address_mode::clamp,
				graphic::bc_texture_address_mode::clamp,
				graphic::bc_texture_address_mode::clamp
			).as_sampler_state();
		m_linear_sampler = l_device.create_sampler_state(l_linear_sampler_config);

		const auto l_sprite_texture_size = m_sprites_texture->get_resource().get_width();
		for (auto& l_icon : m_sprites_map)
		{
			m_sprites_map.insert_or_assign(l_icon.first, _map_icon_index_to_uv_coordinates(l_icon.second.m_index, l_sprite_texture_size));
		}

		graphic::bc_device_parameters l_old_parameters
		(
			0,
			0,
			graphic::bc_format::unknown,
			graphic::bc_texture_ms_config(1, 0)
		);
		graphic::bc_device_parameters l_new_parameters
		(
			l_device_swap_buffer.get_back_buffer_width(),
			l_device_swap_buffer.get_back_buffer_height(),
			l_device_swap_buffer.get_back_buffer_format(),
			l_device_swap_buffer.get_back_buffer_texture().get_sample_count()
		);

		after_reset(game::bc_render_pass_reset_context(p_render_system, l_device, l_device_swap_buffer, l_old_parameters, l_new_parameters));
	}

	void bc_icon_draw_pass::update(const game::bc_render_pass_update_context& p_context)
	{
	}

	void bc_icon_draw_pass::initialize_frame(const game::bc_render_pass_render_context& p_context)
	{
		if(m_icons_query.is_executed())
		{
			const auto l_icons = m_icons_query.get().get_icons();
			for(const auto& l_icon : l_icons)
			{
				if
				(
					(l_icon.m_type == game::bc_icon_type::editor && !m_editor_mode) ||
					(l_icon.m_type != game::bc_icon_type::editor && m_editor_mode)
				)
				{
					continue;
				}

				const auto l_sprite_texcoord = m_sprites_map.find(l_icon.m_name);

				if(l_sprite_texcoord != std::end(m_sprites_map))
				{
					m_sprites.push_back
					({
						l_icon.m_position,
						l_icon.m_size,
						l_sprite_texcoord->second.m_u0,
						l_sprite_texcoord->second.m_v0,
						l_sprite_texcoord->second.m_u1,
						l_sprite_texcoord->second.m_v1
					});
				}
			}
		}

		m_icons_query = p_context.m_query_manager.queue_query
		(
			game::bc_main_camera_scene_icon_query()
		);
	}

	void bc_icon_draw_pass::execute(const game::bc_render_pass_render_context& p_context)
	{
		if(m_sprites.empty())
		{
			return;
		}

		p_context.m_render_thread.start();

		_resize_sprites_buffer(p_context.m_render_system.get_device());
		p_context.m_render_thread.update_subresource(m_sprites_buffer.get(), 0, m_sprites.data(), 1, 1);

		p_context.m_render_thread.bind_render_pass_state(*m_render_pass_state);
		p_context.m_render_thread.get_pipeline().bind_ia_primitive_topology(graphic::bc_primitive::pointlist);
		p_context.m_render_thread.get_pipeline().pipeline_apply_states(graphic::bc_pipeline_stage::input_assembler_stage);
		p_context.m_render_thread.draw_instanced(1, m_sprites.size(), 0, 0);
		p_context.m_render_thread.get_pipeline().unbind_ia_primitive_topology();
		p_context.m_render_thread.get_pipeline().pipeline_apply_states(graphic::bc_pipeline_stage::input_assembler_stage);
		p_context.m_render_thread.unbind_render_pass_state(*m_render_pass_state);

		p_context.m_render_thread.finish();

		m_sprites.clear();
	}

	void bc_icon_draw_pass::before_reset(const game::bc_render_pass_reset_context& p_context)
	{
	}

	void bc_icon_draw_pass::after_reset(const game::bc_render_pass_reset_context& p_context)
	{
		const auto l_depth_stencil_texture = get_shared_resource_throw<graphic::bc_texture2d>(constant::g_rpass_depth_stencil_texture);
		const auto l_depth_buffer_view = get_shared_resource_throw<graphic::bc_depth_stencil_view>(constant::g_rpass_depth_stencil_render_view);
		const auto l_render_target = get_shared_resource_throw<graphic::bc_texture2d>(m_render_target_texture);
		const auto l_render_render_view = get_shared_resource_throw<graphic::bc_render_target_view>(m_render_target_view);
		const auto l_viewport = graphic::bc_viewport::default_config(l_render_target.get_width(), l_render_target.get_height());

		m_device_pipeline_state = p_context.m_render_system.create_device_pipeline_state
		(
			"sprite_icon_vs",
			nullptr,
			nullptr,
			"sprite_icon_gs",
			"sprite_icon_ps",
			game::bc_vertex_type::none,
			game::bc_blend_type::blending_overwrite_alpha,
			core::bc_enum::mask_or({ game::bc_depth_stencil_type::depth_less_no_write, game::bc_depth_stencil_type::stencil_off }),
			game::bc_rasterizer_type::fill_solid_cull_none,
			0x1,
			{
				l_render_target.get_format()
			},
			l_depth_stencil_texture.get_format(),
			game::bc_multi_sample_type::c1_q1
		);
		m_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			m_device_pipeline_state.get(),
			l_viewport,
			{
				graphic::bc_render_target_view_parameter(l_render_render_view)
			},
			l_depth_buffer_view,
			{
				graphic::bc_sampler_parameter(0, graphic::bc_shader_type::pixel, m_linear_sampler.get())
			},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::geometry, m_sprites_buffer_view_link),
				graphic::bc_resource_view_parameter(1, graphic::bc_shader_type::pixel, m_sprites_texture_view.get()),
			},
			{},
			{
				p_context.m_render_system.get_global_cbuffer()
			}
		);

		m_sprites_buffer_view_link.set_as_resource_view(m_sprites_buffer_view.get());
	}

	void bc_icon_draw_pass::destroy(game::bc_render_system& p_render_system)
	{
		m_sprites_texture.reset();
		m_sprites_texture_view.reset();
		m_sprites_buffer.reset();
		m_sprites_buffer_view.reset();
		m_linear_sampler.reset();
		m_device_pipeline_state.reset();
		m_render_pass_state.reset();
	}

	void bc_icon_draw_pass::_event_handler(core::bci_event& p_event)
	{
		const auto* l_editor_event = core::bci_message::as<game::bc_event_editor_mode>(p_event);
		if (l_editor_event)
		{
			m_editor_mode = l_editor_event->get_editor_mode();
			return;
		}
	}

	void bc_icon_draw_pass::_resize_sprites_buffer(graphic::bc_device& p_device)
	{
		const auto l_buffer_size = m_sprites_buffer->is_valid() ? m_sprites_buffer->get_byte_width() / sizeof(_bc_sprite_icon) : 0U;

		if(l_buffer_size < m_sprites.capacity())
		{
			const auto l_sprites_buffer_config = graphic::bc_graphic_resource_builder()
				.as_resource()
				.as_buffer(m_sprites.capacity(), sizeof(_bc_sprite_icon), graphic::bc_resource_usage::gpu_rw, graphic::bc_resource_view_type::shader)
				.with_structured_buffer(sizeof(_bc_sprite_icon))
				.as_buffer();
			const auto l_sprites_view_config = graphic::bc_graphic_resource_builder()
				.as_resource_view()
				.as_buffer_view(graphic::bc_format::unknown)
				.as_shader_view(0, m_sprites.capacity())
				.as_structured_buffer();

			const auto l_sprites_buffer_data = graphic::bc_subresource_data(m_sprites.data(), 0, 0);
			m_sprites_buffer = p_device.create_buffer(l_sprites_buffer_config, &l_sprites_buffer_data);
			m_sprites_buffer_view = p_device.create_resource_view(m_sprites_buffer.get(), l_sprites_view_config);

			m_sprites_buffer_view_link.set_as_resource_view(m_sprites_buffer_view.get());

			m_sprites_buffer->set_debug_name("sprite_icon_buffer");
			m_sprites_buffer_view->set_debug_name("sprite_icon_buffer_view");
		}
	}

	_bc_sprite_texcoord bc_icon_draw_pass::_map_icon_index_to_uv_coordinates(bcUINT32 p_sprite_icon_index, bcUINT32 p_sprites_texture_size) const
	{
		_bc_sprite_texcoord l_texcoord;

		const auto l_sprite_row = p_sprite_icon_index / m_sprites_count_per_row;
		const auto l_sprite_column = p_sprite_icon_index % m_sprites_count_per_row;
		const auto l_sprite_x0 = l_sprite_column * m_sprites_width;
		const auto l_sprite_x1 = l_sprite_x0 + m_sprites_width;
		const auto l_sprite_y0 = l_sprite_row * m_sprites_width;
		const auto l_sprite_y1 = l_sprite_y0 + m_sprites_width;

		l_texcoord.m_u0 = static_cast<bcFLOAT>(l_sprite_x0) / static_cast<bcFLOAT>(p_sprites_texture_size);
		l_texcoord.m_u1 = static_cast<bcFLOAT>(l_sprite_x1) / static_cast<bcFLOAT>(p_sprites_texture_size);
		l_texcoord.m_v0 = static_cast<bcFLOAT>(l_sprite_y0) / static_cast<bcFLOAT>(p_sprites_texture_size);
		l_texcoord.m_v1 = static_cast<bcFLOAT>(l_sprite_y1) / static_cast<bcFLOAT>(p_sprites_texture_size);

		if constexpr (!graphic::bc_render_api_info::use_top_left_texcoord())
		{
			l_texcoord.m_u0 = 1.f - l_texcoord.m_u0;
			l_texcoord.m_u1 = 1.f - l_texcoord.m_u1;
		}

		return l_texcoord;
	}
}
