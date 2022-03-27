// [01/29/2022 MRB]

#include "Editor/EditorPCH.h"

#include "Game/System/Input/bcFileSystem.h"
#include "Game/Object/Scene/bcScene.h"
#include "App/RenderPass/PostProcess/bcGlowPass.h"
#include "App/bcConstant.h"
#include "BoX.Game/Application/bxApplicationHookFunctions.h"
#include "BoX.Game/RenderPass/bxBulletTrailPass.h"
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
		bx_register_game_loaders(p_context, true);
		bx_register_game_actor_components();
		bx_bind_game_scripts(*m_game_system);
		bx_register_game_particle_emitters(*m_game_system);
		bx_load_game_shaders(*core::bc_get_service<core::bc_content_stream_manager>(), *m_game_system);
	}

	void bx_editor_render_app::application_initialize(const bc_application_initialize_context& p_context)
	{
		bc_editor_render_app::application_initialize(p_context);

		auto& l_render_system = m_game_system->get_render_system();

		l_render_system.add_render_pass_before<bx_bullet_trail_pass, bc_glow_pass>
		(
			bx_bullet_trail_pass(constant::g_rpass_back_buffer_texture,constant::g_rpass_back_buffer_render_view, bcL("Texture\\Flare\\BulletTrail.dds"))
		);
	}

	void bx_editor_render_app::application_load_content(const bc_application_load_context& p_context)
	{
		bc_editor_render_app::application_load_content(p_context);

		bx_load_game_resources(p_context.m_stream_manager, *m_game_system);

		auto* l_content_manager = core::bc_get_service<core::bc_content_manager>();
		auto& l_file_system = m_game_system->get_file_system();

		auto l_scene = l_content_manager->load<game::bc_scene>
		(
			l_file_system.get_content_scene_path(bcL("BattleGround.json")),
			{},
			core::bc_content_loader_parameter()
		);
		m_game_system->set_scene(std::move(l_scene));
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
