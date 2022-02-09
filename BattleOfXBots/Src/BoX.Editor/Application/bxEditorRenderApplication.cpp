// [01/29/2022 MRB]

#include "Editor/EditorPCH.h"
#include "BoX/Application/bxApplicationHookFunctions.h"
#include "BoX.Editor/Application/bxEditorRenderApplication.h"

namespace box
{
	bx_editor_render_app::bx_editor_render_app()
		: bc_editor_render_app()
	{
	}

	bx_editor_render_app::~bx_editor_render_app() = default;

	void bx_editor_render_app::application_start_engine_components(game::bc_engine_application_parameter& p_parameters)
	{
		bc_editor_render_app::application_start_engine_components(p_parameters);

		bx_start_game_services(p_parameters);
		bx_register_game_loaders(p_parameters);
		bx_register_game_actor_components();
		bx_register_game_network_messages(m_game_system->get_network_system());
		bx_bind_game_scripts(*m_game_system);
		bx_register_game_particle_emitters(*m_game_system);
		bx_load_game_shaders(*core::bc_get_service<core::bc_content_stream_manager>(), *m_game_system);
	}

	void bx_editor_render_app::application_initialize(game::bc_engine_application_parameter& p_parameters)
	{
		bc_editor_render_app::application_initialize(p_parameters);
	}

	void bx_editor_render_app::application_load_content(core::bc_content_stream_manager& p_stream_manager)
	{
		bc_editor_render_app::application_load_content(p_stream_manager);

		bx_load_game_resources(p_stream_manager, *m_game_system);
	}

	void bx_editor_render_app::application_update(const core_platform::bc_clock::update_param& p_clock_update_param, bool p_is_partial_update)
	{
		bc_editor_render_app::application_update(p_clock_update_param, p_is_partial_update);
	}

	void bx_editor_render_app::application_render(const core_platform::bc_clock::update_param& p_clock_update_param)
	{
		bc_editor_render_app::application_render(p_clock_update_param);
	}

	bool bx_editor_render_app::application_event(core::bci_event& p_event)
	{
		return bc_editor_render_app::application_event(p_event);
	}

	void bx_editor_render_app::application_unload_content(core::bc_content_stream_manager& p_stream_manager)
	{
		bx_unload_game_resources(p_stream_manager);

		bc_editor_render_app::application_unload_content(p_stream_manager);
	}

	void bx_editor_render_app::application_destroy()
	{
		bc_editor_render_app::application_destroy();
	}

	void bx_editor_render_app::application_close_engine_components()
	{
		bx_close_game_services();

		bc_editor_render_app::application_close_engine_components();
	}
}
