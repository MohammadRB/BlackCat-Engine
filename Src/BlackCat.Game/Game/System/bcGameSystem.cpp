// [04/29/2016 MRB]

#include "Game/GamePCH.h"

#include "Core/Messaging/Event/bcEventManager.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/System/bcGameSystem.h"

#include "Game/Object/Animation/bcAnimationManager.h"
#include "Game/Query/bcQueryContext.h"
#include "Render/Particle/bcParticleManager.h"

namespace black_cat
{
	namespace game
	{
		bc_game_system::bc_game_system()
			: m_file_system(*core::bc_get_service<core::bc_content_manager>(), *core::bc_get_service<core::bc_content_stream_manager>()),
			m_input_system(),
			m_physics_system(),
			m_script_system(),
			m_render_system(),
			m_console(),
			m_scene()
		{
		}

		bc_game_system::~bc_game_system()
		{
			if (m_initialized)
			{
				bc_initializable::destroy();
			}
		}

		void bc_game_system::set_scene(bc_scene_ptr p_scene)
		{
			m_scene = std::move(p_scene);
		}

		void bc_game_system::update_game(const core_platform::bc_clock::update_param& p_clock, bool p_is_partial_update)
		{
			auto& l_event_manager = *core::bc_get_service<core::bc_event_manager>();
			auto& l_actor_component_manager = *core::bc_get_service<bc_actor_component_manager>();
			auto& l_query_manager = *core::bc_get_service< core::bc_query_manager >();
			auto& l_animation_manager = m_render_system.get_animation_manager();
			auto& l_particle_manager = m_render_system.get_particle_manager();
			auto& l_render_system = m_render_system;
			auto& l_script_system = m_script_system;
			auto& l_input_system = m_input_system;
			auto& l_physics_system = m_physics_system;
			auto& l_console = m_console;
			auto* l_scene = m_scene.get();

			if (p_is_partial_update)
			{
				l_physics_system.update(p_clock);
				if (l_scene)
				{
					l_scene->update_px(l_physics_system, p_clock);
				}

				return;
			}

			l_input_system.update(p_clock);
			l_physics_system.update(p_clock);

			core::bc_task<void> l_scene_task;
			core::bc_task<void> l_animations_task;

			if (l_scene)
			{
				l_scene_task = l_scene->update_px_async(l_physics_system, p_clock);
			}

			l_event_manager.process_event_queue(p_clock);

			if (l_scene)
			{
				l_scene_task.wait();
			}

			l_actor_component_manager.update_actors(p_clock);
			l_query_manager.process_query_queue(p_clock);

			if (l_scene)
			{
				l_scene_task = l_scene->update_graph_async();
			}

			l_animations_task = l_animation_manager.run_scheduled_jobs_async(p_clock);

			l_particle_manager.update(p_clock);
			l_script_system.update(p_clock);
			l_console->update(p_clock);

			if (l_scene)
			{
				l_scene_task.wait();
			}
			l_animations_task.wait();

			l_render_system.update(bc_render_system::update_context(p_clock, m_input_system.get_camera()));
		}
		
		void bc_game_system::render_game(const core_platform::bc_clock::update_param& p_clock)
		{
			core::bc_get_service< core::bc_event_manager >()->process_render_event_queue(p_clock);
			
			if(m_scene)
			{
				m_render_system.render(bc_render_system::render_context(p_clock, *core::bc_get_service< core::bc_query_manager >()));
			}
		}

		void bc_game_system::swap_frame(const core_platform::bc_clock::update_param& p_clock)
		{
			auto* l_query_manager = core::bc_get_service< core::bc_query_manager >();
			l_query_manager->process_query_queue(p_clock);
			l_query_manager->swap_frame();
		}

		void bc_game_system::_initialize(bc_game_system_parameter p_parameter)
		{
			m_physics_system.initialize();
			m_script_system.initialize(true);
			m_render_system.initialize(*core::bc_get_service<core::bc_content_stream_manager>(), m_physics_system, std::move(p_parameter.m_render_system_parameter));
			m_console = core::bc_make_unique<bc_game_console>(core::bc_alloc_type::program, m_script_system);

			m_scene_query_context_provider = core::bc_get_service< core::bc_query_manager >()->register_query_provider<bc_scene_query_context>
			(
				[=]()
				{
					bc_scene_query_context l_context;
					l_context.m_scene = m_scene.get();
				
					return core::bc_make_query_context(l_context);
				}
			);
		}

		void bc_game_system::_destroy()
		{
			m_scene_query_context_provider.reset();
			
			m_scene.reset();
			m_console.reset();
			m_render_system.destroy();
			m_script_system.destroy();
			m_physics_system.destroy();
		}
	}
}
