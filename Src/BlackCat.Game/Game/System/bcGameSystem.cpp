// [04/29/2016 MRB]

#include "Game/GamePCH.h"

#include "CorePlatformImp/Concurrency/bcThread.h"
#include "Core/Messaging/Event/bcEventManager.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Render/Particle/bcParticleManager.h"
#include "Game/System/Render/Decal/bcDecalManager.h"
#include "Game/Object/Animation/bcAnimationManager.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Query/bcQueryContext.h"
#include "Game/bcEvent.h"

namespace black_cat
{
	namespace game
	{
		bc_game_system::bc_game_system()
			: m_query_manager(nullptr),
			m_event_manager(nullptr),
			m_file_system
			(
				*core::bc_get_service<core::bc_content_manager>(),
				*core::bc_get_service<core::bc_content_stream_manager>()
			),
			m_input_system(),
			m_physics_system(),
			m_script_system(),
			m_render_system(),
			m_console(),
			m_scene(),
			m_editor_mode(false)
		{
			m_console = core::bc_make_unique<bc_game_console>(core::bc_alloc_type::program, m_script_system);
		}

		bc_game_system::~bc_game_system()
		{
			if (is_initialized())
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
			auto& l_event_manager = *m_event_manager;
			auto& l_actor_component_manager = *core::bc_get_service<bc_actor_component_manager>();
			auto& l_query_manager = *m_query_manager;
			auto& l_animation_manager = m_render_system.get_animation_manager();
			auto& l_render_system = m_render_system;
			auto& l_script_system = m_script_system;
			auto& l_input_system = m_input_system;
			auto& l_physics_system = m_physics_system;
			auto& l_console = m_console;
			auto* l_camera = m_input_system.get_camera();
			auto* l_scene = m_scene.get();
			auto* l_particle_manager = l_scene ? &l_scene->get_particle_manager() : static_cast<bc_particle_manager*>(nullptr);
			auto* l_decal_manager = l_scene ? &l_scene->get_decal_manager() : static_cast<bc_decal_manager*>(nullptr);

			if(!m_editor_mode)
			{
				if (p_is_partial_update)
				{
					l_physics_system.update(p_clock);
					if (l_scene)
					{
						l_scene->update_physics(p_clock);
					}

					return;
				}

				core::bc_task<void> l_scene_task;
				core::bc_task<void> l_scene_task1;

				l_input_system.update(p_clock);
				l_physics_system.update(p_clock);

				if (l_scene)
				{
					l_scene_task = l_scene->update_physics_async(p_clock);
				}

				l_event_manager.process_event_queue(p_clock);

				l_scene_task.wait();

				l_actor_component_manager.process_actor_events(p_clock);
				l_actor_component_manager.update_actors(p_clock);
				l_animation_manager.run_scheduled_jobs(p_clock);
				l_actor_component_manager.double_update_actors(p_clock);

				if (l_scene)
				{
					l_scene_task = l_scene->update_graph_async(p_clock);
					l_scene_task1 = l_scene->update_bullets_async(p_clock);

					l_particle_manager->update(p_clock);
					l_decal_manager->update_decal_lifespans(p_clock);
				}

				l_script_system.update(p_clock);
				l_console->update(p_clock);

				if(l_camera)
				{
					l_render_system.update(bc_render_system::update_context(p_clock, *l_camera));
				}

				core::bc_concurrency::when_all(l_scene_task, l_scene_task1);
			}
			else
			{
				l_input_system.update(p_clock);
				
				l_event_manager.process_event_queue(p_clock);

				l_actor_component_manager.process_actor_events(p_clock);

				core::bc_task<void> l_scene_task = l_scene->update_graph_async(p_clock);

				l_script_system.update(p_clock);
				l_console->update(p_clock);
				
				if (l_camera)
				{
					l_render_system.update(bc_render_system::update_context(p_clock, *l_camera));
				}

				core::bc_concurrency::when_all(l_scene_task);
			}

			l_query_manager.process_query_queue(p_clock);
		}
		
		void bc_game_system::render_game(const core_platform::bc_clock::update_param& p_clock)
		{
			m_event_manager->process_render_event_queue(p_clock);

			if(m_input_system.get_camera() && m_scene)
			{
				m_render_system.render(bc_render_system::render_context(p_clock, *m_query_manager));
			}
		}

		void bc_game_system::swap_frame_idle(const core_platform::bc_clock::update_param& p_clock)
		{
			const auto l_num_query = m_query_manager->process_query_queue(p_clock);

			if(!l_num_query)
			{
				core_platform::bc_thread::current_thread_yield();
			}
		}

		void bc_game_system::swap_frame(const core_platform::bc_clock::update_param& p_clock)
		{
			m_query_manager->process_query_queue(p_clock);
			m_query_manager->swap_frame();
		}

		void bc_game_system::render_swap_frame(const core_platform::bc_clock::update_param& p_clock)
		{
			m_render_system.swap(bc_render_system::swap_context(p_clock));
		}

		void bc_game_system::_initialize(bc_game_system_parameter p_parameter)
		{
			m_query_manager = p_parameter.m_query_manager;
			m_event_manager = p_parameter.m_event_manager;
			m_physics_system.initialize();
			m_network_system.initialize();
			m_script_system.initialize(true);
			m_render_system.initialize(std::move(p_parameter.m_render_system_parameter));

			m_editor_event_handle = m_event_manager->register_event_listener<bc_event_editor_mode>
			(
				core::bc_event_manager::delegate_type(*this, &bc_game_system::_event_handler)
			);
			m_scene_query_context_provider = m_query_manager->register_query_provider<bc_scene_query_context>
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
			m_editor_event_handle.reset();
			m_scene_query_context_provider.reset();

			m_render_system.destroy_render_passes();
			m_query_manager->clear_queries(); // Clear queries to release probable references to render states
			
			m_scene.reset();
			m_console.reset();
			m_render_system.destroy();
			m_script_system.destroy();
			m_network_system.destroy();
			m_physics_system.destroy();
		}

		bool bc_game_system::_event_handler(core::bci_event& p_event)
		{
			auto* l_editor_event = core::bci_message::as<bc_event_editor_mode>(p_event);
			if (l_editor_event)
			{
				m_editor_mode = l_editor_event->get_editor_mode();
			}
			return true;
		}
	}
}