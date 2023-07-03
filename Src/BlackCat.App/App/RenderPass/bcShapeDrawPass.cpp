// [06/12/2018 MRB]

#include "App/AppPCH.h"

#include "Core/Messaging/Event/bcEventManager.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcDefaultRenderThread.h"
#include "Game/System/Input/bcGlobalConfig.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Query/bcMainCameraSceneSharedQuery.h"
#include "Game/bcEvent.h"
#include "App/RenderPass/bcShapeDrawPass.h"
#include "App/bcConstant.h"

namespace black_cat
{
	bc_scene_debug_shape_query::bc_scene_debug_shape_query(game::bc_shape_drawer& p_shape_drawer, 
		game::bc_actor p_hovered_actor, 
		game::bc_actor p_selected_actor,
		game::bc_decal_instance* p_hovered_decal,
		game::bc_decal_instance* p_selected_decal,
		bool p_draw_scene_graph) noexcept
		: bc_query(message_name()),
		m_shape_drawer(&p_shape_drawer),
		m_hovered_actor(std::move(p_hovered_actor)),
		m_selected_actor(std::move(p_selected_actor)),
		m_hovered_decal(std::move(p_hovered_decal)),
		m_selected_decal(std::move(p_selected_decal)),
		m_draw_scene_graph(p_draw_scene_graph)
	{
	}

	void bc_scene_debug_shape_query::execute(const game::bc_scene_query_context& p_context) noexcept
	{
		if(!p_context.m_scene)
		{
			return;
		}
		
		if (m_hovered_actor.is_valid())
		{
			const auto* l_mediate_component = m_hovered_actor.get_component<game::bc_mediate_component>();
			m_shape_drawer->draw_wired_bound_box(l_mediate_component->get_bound_box());
		}

		if(m_selected_actor.is_valid())
		{
			m_selected_actor.draw_debug(*m_shape_drawer);
		}

		if(m_hovered_decal)
		{
			const auto l_box = m_hovered_decal->get_box();
			m_shape_drawer->draw_wired_box(l_box.first, l_box.second);
		}

		if (m_selected_decal)
		{
			const auto l_box = m_selected_decal->get_box();
			m_shape_drawer->draw_wired_box(l_box.first, l_box.second);
		}

		if(m_draw_scene_graph)
		{
			p_context.m_scene->draw_debug_shapes(*m_shape_drawer);
		}
	}
	
	bc_shape_draw_pass::bc_shape_draw_pass(game::bc_render_pass_variable_t p_render_target_view)
		: m_render_target_view_variable(p_render_target_view),
		m_hovered_decal(nullptr),
		m_selected_decal(nullptr)
	{
		auto& l_event_manager = *core::bc_get_service<core::bc_event_manager>();
		m_editor_event_handle = l_event_manager.register_event_listener<game::bc_event_editor_mode>
		(
			core::bc_event_manager::delegate_type(*this, &bc_shape_draw_pass::_event_handler)
		);
		m_scene_change_event_handle = l_event_manager.register_event_listener<game::bc_event_scene_change>
		(
			core::bc_event_manager::delegate_type(*this, &bc_shape_draw_pass::_event_handler)
		);
	}

	bc_shape_draw_pass::bc_shape_draw_pass(bc_shape_draw_pass&& p_other) noexcept
		: m_render_target_view_variable(p_other.m_render_target_view_variable),
		m_device_pipeline_state(std::move(p_other.m_device_pipeline_state)),
		m_render_pass_state(std::move(p_other.m_render_pass_state)),
		m_draw_scene_graph_debug(p_other.m_draw_scene_graph_debug),
		m_scene_debug_query(std::move(p_other.m_scene_debug_query)),
		m_hovered_actor(std::move(p_other.m_hovered_actor)),
		m_selected_actor(std::move(p_other.m_selected_actor)),
		m_hovered_decal(p_other.m_hovered_decal),
		m_selected_decal(p_other.m_selected_decal),
		m_editor_event_handle(std::move(p_other.m_editor_event_handle)),
		m_scene_change_event_handle(std::move(p_other.m_scene_change_event_handle))
	{
		m_editor_event_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_shape_draw_pass::_event_handler));
		m_scene_change_event_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_shape_draw_pass::_event_handler));
	}

	bc_shape_draw_pass& bc_shape_draw_pass::operator=(bc_shape_draw_pass&& p_other) noexcept
	{
		m_render_target_view_variable = p_other.m_render_target_view_variable;
		m_device_pipeline_state = std::move(p_other.m_device_pipeline_state);
		m_render_pass_state = std::move(p_other.m_render_pass_state);

		m_draw_scene_graph_debug = p_other.m_draw_scene_graph_debug;
		m_scene_debug_query = std::move(p_other.m_scene_debug_query);
		m_hovered_actor = std::move(p_other.m_hovered_actor);
		m_selected_actor = std::move(p_other.m_selected_actor);
		m_hovered_decal = p_other.m_hovered_decal;
		m_selected_decal = p_other.m_selected_decal;

		m_editor_event_handle = std::move(p_other.m_editor_event_handle);
		m_scene_change_event_handle = std::move(p_other.m_scene_change_event_handle);
		m_editor_event_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_shape_draw_pass::_event_handler));
		m_scene_change_event_handle.reassign(core::bc_event_manager::delegate_type(*this, &bc_shape_draw_pass::_event_handler));

		return *this;
	}

	void bc_shape_draw_pass::set_hovered_actor(const game::bc_actor& p_actor)
	{
		m_hovered_actor = p_actor;
	}

	void bc_shape_draw_pass::set_selected_actor(const game::bc_actor& p_actor)
	{
		m_selected_actor = p_actor;
	}

	void bc_shape_draw_pass::set_hovered_decal(game::bc_decal_instance* p_decal)
	{
		m_hovered_decal = p_decal;
	}

	void bc_shape_draw_pass::set_selected_decal(game::bc_decal_instance* p_decal)
	{
		m_selected_decal = p_decal;
	}

	void bc_shape_draw_pass::initialize_resources(game::bc_render_system& p_render_system)
	{
		auto& l_device = p_render_system.get_device();
		auto& l_device_swap_buffer = p_render_system.get_device_swap_buffer();

		m_device_pipeline_state = p_render_system.create_device_pipeline_state
		(
			"shape_draw_vs",
			nullptr,
			nullptr,
			nullptr,
			"shape_draw_ps",
			game::bc_vertex_type::pos,
			game::bc_blend_type::opaque,
			core::bc_enum::mask_or({ game::bc_depth_stencil_type::depth_less, game::bc_depth_stencil_type::stencil_off }),
			game::bc_rasterizer_type::fill_solid_cull_none,
			1,
			{ l_device_swap_buffer.get_back_buffer_texture().get_format()},
			game::bc_surface_format_type::D32_FLOAT,
			game::bc_multi_sample_type::c1_q1
		);
		
		after_reset(game::bc_render_pass_reset_context
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
		));
	}

	void bc_shape_draw_pass::update(const game::bc_render_pass_update_context& p_context)
	{
	}

	void bc_shape_draw_pass::initialize_frame(const game::bc_render_pass_render_context& p_context)
	{
		m_scene_debug_query = p_context.m_query_manager.queue_query
		(
			bc_scene_debug_shape_query
			(
				p_context.m_render_system.get_shape_drawer(),
				m_hovered_actor,
				m_selected_actor,
				m_hovered_decal,
				m_selected_decal,
				m_draw_scene_graph_debug
			)
		);
	}

	void bc_shape_draw_pass::execute(const game::bc_render_pass_render_context& p_context)
	{
		auto& l_shape_drawer = p_context.m_render_system.get_shape_drawer();
		
		p_context.m_render_thread.start();
		p_context.m_render_thread.bind_render_pass_state(*m_render_pass_state.get());

		auto l_render_state_buffer = p_context.m_frame_renderer.create_buffer();
		l_shape_drawer.render(p_context.m_render_system, p_context.m_render_thread, l_render_state_buffer);
		p_context.m_frame_renderer.render_buffer(p_context.m_render_thread, l_render_state_buffer, p_context.m_render_camera);

		p_context.m_render_thread.unbind_render_pass_state(*m_render_pass_state.get());
		p_context.m_render_thread.finish();
	}

	void bc_shape_draw_pass::cleanup_frame(const game::bc_render_pass_render_context& p_context)
	{
	}

	void bc_shape_draw_pass::before_reset(const game::bc_render_pass_reset_context& p_context)
	{
		m_render_pass_state.reset();
	}

	void bc_shape_draw_pass::after_reset(const game::bc_render_pass_reset_context& p_context)
	{
		const auto l_back_buffer_texture = p_context.m_device_swap_buffer.get_back_buffer_texture();

		const auto l_depth_stencil_view = *get_shared_resource<graphic::bc_depth_stencil_view>(constant::g_rpass_depth_stencil_render_view);
		const auto l_render_target_view = *get_shared_resource<graphic::bc_render_target_view>(m_render_target_view_variable);
		const auto l_viewport = graphic::bc_viewport::default_config(l_back_buffer_texture.get_width(), l_back_buffer_texture.get_height());
			
		m_render_pass_state = p_context.m_render_system.create_render_pass_state
		(
			m_device_pipeline_state.get(),
			l_viewport,
			{
				graphic::bc_render_target_view_parameter(l_render_target_view)
			},
			l_depth_stencil_view,
			{
			},
			{
			},
			{
			},
			{
				p_context.m_render_system.get_global_cbuffer()
			}
		);
	}

	void bc_shape_draw_pass::config_changed(const game::bc_render_pass_config_change_context& p_context)
	{
		set_parameters(p_context.m_global_config.get_scene_graph_debug_draw());
	}

	void bc_shape_draw_pass::destroy(game::bc_render_system& p_render_system)
	{
		m_render_pass_state.reset();
		m_device_pipeline_state.reset();
	}

	void bc_shape_draw_pass::set_parameters(bool p_draw_scene_debug)
	{
		m_draw_scene_graph_debug = p_draw_scene_debug;
	}

	void bc_shape_draw_pass::_event_handler(core::bci_event& p_event)
	{
		if (const auto* l_editor_event = core::bci_message::as<game::bc_event_editor_mode>(p_event))
		{
			if(const auto l_is_game_mode = !l_editor_event->get_editor_mode())
			{
				// Because selected decal can be deleted by removing its owner actor during checkpoint restore process
				// we should clear these weak references when switching to game mode
				m_hovered_actor = game::bc_actor();
				m_hovered_decal = nullptr;
				m_selected_decal = nullptr;
			}
			return;
		}

		if(const auto* l_scene_change_event = core::bci_message::as<game::bc_event_scene_change>(p_event))
		{
			m_selected_actor = game::bc_actor();
			m_hovered_actor = game::bc_actor();
			m_hovered_decal = nullptr;
			m_selected_decal = nullptr;
		}
	}
}
