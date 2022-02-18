// [01/29/2022 MRB]

#include "Editor/EditorPCH.h"
#include "BoX.Game/Application/bxApplicationHookFunctions.h"
#include "BoX.Editor/Application/bxEditorRenderApplication.h"

namespace box
{
	bx_editor_render_app::bx_editor_render_app()
		: bc_editor_render_app()
	{
	}

	bx_editor_render_app::~bx_editor_render_app() = default;

	void bx_editor_render_app::application_start_engine_components(const bc_application_start_context& p_context)
	{
		bc_editor_render_app::application_start_engine_components(p_context);

		bx_start_game_services(p_context);
		bx_register_game_loaders(p_context);
		bx_register_game_actor_components();
		bx_bind_game_scripts(*m_game_system);
		bx_register_game_particle_emitters(*m_game_system);
		bx_load_game_shaders(*core::bc_get_service<core::bc_content_stream_manager>(), *m_game_system);
	}

	void bx_editor_render_app::application_initialize(const bc_application_initialize_context& p_context)
	{
		bc_editor_render_app::application_initialize(p_context);
	}

	void bx_editor_render_app::application_load_content(const bc_application_load_context& p_context)
	{
		bc_editor_render_app::application_load_content(p_context);

		bx_load_game_resources(p_context.m_stream_manager, *m_game_system);
	}

	void bx_editor_render_app::application_update(const bc_application_update_context& p_context)
	{
		bc_editor_render_app::application_update(p_context);
	}

	void bx_editor_render_app::application_render(const bc_application_render_context& p_context)
	{
		bc_editor_render_app::application_render(p_context);
	}

	void bx_editor_render_app::application_event(core::bci_event& p_event)
	{
		return bc_editor_render_app::application_event(p_event);
	}

	void bx_editor_render_app::application_unload_content(const bc_application_load_context& p_context)
	{
		bx_unload_game_resources(p_context.m_stream_manager);

		bc_editor_render_app::application_unload_content(p_context);
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
