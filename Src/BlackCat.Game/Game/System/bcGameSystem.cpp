// [04/29/2016 MRB]

#include "Game/GamePCH.h"

#include "CorePlatformImp/Concurrency/bcThread.h"
#include "Core/Messaging/Event/bcEventManager.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManagerContainer.h"
#include "Game/System/Input/bcFileSystem.h"
#include "Game/System/Input/bcInputSystem.h"
#include "Game/System/Physics/bcPhysicsSystem.h"
#include "Game/System/Sound/bcSoundSystem.h"
#include "Game/System/Sound/bcSoundManager.h"
#include "Game/System/Animation/bcAnimationSystem.h"
#include "Game/System/Network/bcNetworkSystem.h"
#include "Game/System/Script/bcScriptSystem.h"
#include "Game/System/Script/bcGameConsole.h"
#include "Game/System/Render/Particle/bcParticleManager.h"
#include "Game/System/Render/Decal/bcDecalManager.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Query/bcQueryContext.h"
#include "Game/bcEvent.h"

namespace black_cat
{
	namespace game
	{
		bc_game_system::bc_game_system()
			: m_query_manager(nullptr),
			m_event_manager(nullptr),
			m_scene_changed(false),
			m_paused(false),
			m_editor_mode(false)
		{
		}

		bc_game_system::~bc_game_system()
		{
			if (is_initialized())
			{
				bc_initializable::destroy();
			}
		}

		void bc_game_system::set_scene(bc_scene_ptr p_scene) noexcept
		{
			m_scene_changed = true;
			m_new_scene = std::move(p_scene);
		}

		void bc_game_system::update_game(const platform::bc_clock::update_param& p_clock, bool p_is_partial_update)
		{
			auto& l_event_manager = *m_event_manager;
			auto& l_file_system = m_file_system;
			auto& l_input_system = m_input_system;
			auto& l_physics_system = m_physics_system;
			auto& l_network_system = m_network_system;
			auto& l_script_system = m_script_system;
			auto& l_render_system = m_render_system;
			auto& l_sound_system = *m_sound_system;
			auto& l_animation_system = *m_animation_system;
			auto& l_console = m_console;
			auto* l_scene = m_scene.get();
			auto* l_actor_component_manager = l_scene ? &l_scene->get_actor_component_manager() : static_cast<bc_actor_component_manager_container*>(nullptr);
			auto* l_particle_manager = l_scene ? &l_scene->get_particle_manager() : static_cast<bc_particle_manager_container*>(nullptr);
			auto* l_decal_manager = l_scene ? &l_scene->get_decal_manager() : static_cast<bc_decal_manager_container*>(nullptr);
			auto* l_sound_manager = l_scene ? &l_scene->get_sound_manager() : static_cast<bc_sound_manager*>(nullptr);
			core::bc_nullable<bc_camera_instance> l_camera;

			if(m_input_system->get_camera())
			{
				l_camera.reset(bc_camera_instance(*m_input_system->get_camera()));
			}

			if(!m_editor_mode && !m_paused)
			{
				core::bc_task<void> l_scene_task;
				core::bc_task<void> l_scene_task1;
				core::bc_task<void> l_network_task;
				core::bc_task<void> l_animation_task;
				core::bc_task<void> l_sound_task;

				if (p_is_partial_update)
				{
					if (l_scene)
					{
						l_scene_task = l_scene->update_physics_async(p_clock, p_is_partial_update);
						l_particle_manager->update(p_clock);

						core::bc_concurrency::when_all(l_scene_task);
					}

					return;
				}

				l_file_system->update(p_clock);
				l_input_system->update(p_clock);
				l_physics_system->update(p_clock);

				if (l_scene)
				{
					l_scene_task = l_scene->update_physics_async(p_clock, p_is_partial_update);
				}

				l_event_manager.process_event_queue(p_clock);

				core::bc_concurrency::when_all(l_scene_task);

				if (l_scene)
				{
					l_actor_component_manager->process_actor_events(p_clock);
					l_actor_component_manager->update_actors(p_clock);
				}

				l_network_task = l_network_system->update_async(p_clock);
				l_animation_task = l_animation_system.run_scheduled_jobs_async(p_clock);

				core::bc_concurrency::when_all(l_network_task, l_animation_task);

				if (l_scene)
				{
					l_actor_component_manager->double_update_actors(p_clock);
				}

				if (l_camera.has_value())
				{
					l_sound_task = l_sound_system.update_async(bc_sound_system::update_context(p_clock, *l_camera));
				}

				if (l_scene)
				{
					l_scene_task = l_scene->update_graph_async(p_clock);
					l_scene_task1 = l_scene->update_bullets_async(p_clock);

					l_particle_manager->update(p_clock);
					l_decal_manager->update_decal_lifespans(p_clock);
					l_sound_manager->update_pitch(p_clock);
				}

				l_script_system->update(p_clock);
				l_console->update(p_clock);

				if(l_camera.has_value())
				{
					l_render_system->update(bc_render_system::update_context(p_clock, *l_camera));
				}

				core::bc_concurrency::when_all(l_sound_task, l_scene_task, l_scene_task1);
			}

			if(!m_editor_mode && m_paused)
			{
				if (p_is_partial_update)
				{
					return;
				}

				if(!m_pause_last_total_elapsed.has_value())
				{
					m_pause_last_total_elapsed.reset(p_clock.m_total_elapsed);
				}

				l_file_system->update(p_clock);
				l_input_system->update(p_clock);
				l_event_manager.process_event_queue(p_clock);

				l_console->update(p_clock);

				if (l_camera.has_value())
				{
					const auto l_paused_clock = platform::bc_clock::update_param(*m_pause_last_total_elapsed, 0, p_clock.m_average_elapsed);
					l_render_system->update(bc_render_system::update_context(l_paused_clock, *l_camera));
				}
			}

			if(m_editor_mode)
			{
				if (p_is_partial_update)
				{
					return;
				}

				l_file_system->update(p_clock);
				l_input_system->update(p_clock);
				l_event_manager.process_event_queue(p_clock);

				if(m_scene)
				{
					l_actor_component_manager->process_actor_events(p_clock);
				}

				core::bc_task<void> l_sound_task;
				core::bc_task<void> l_scene_task;

				if (l_camera.has_value())
				{
					l_sound_task = l_sound_system.update_async(bc_sound_system::update_context(p_clock, *l_camera));
				}

				if (l_scene)
				{
					l_scene_task = l_scene->update_graph_async(p_clock);
				}

				l_script_system->update(p_clock);
				l_console->update(p_clock);
				
				if (l_camera.has_value())
				{
					l_render_system->update(bc_render_system::update_context(p_clock, *l_camera));
				}

				core::bc_concurrency::when_all(l_sound_task, l_scene_task);
			}
		}
		
		void bc_game_system::render_game(const platform::bc_clock::update_param& p_clock)
		{
			m_event_manager->process_render_event_queue(p_clock);

			if(m_input_system->get_camera())
			{
				if(!m_paused)
				{
					m_render_system->render(bc_render_system::render_context(p_clock, *m_query_manager));
				}
				else
				{
					const auto l_paused_clock = platform::bc_clock::update_param(*m_pause_last_total_elapsed, 0, p_clock.m_average_elapsed);
					m_render_system->render(bc_render_system::render_context(l_paused_clock, *m_query_manager));
				}
			}
		}

		void bc_game_system::swap_frame_idle(const platform::bc_clock::update_param& p_clock)
		{
			if (m_scene_changed)
			{
				_change_scene(p_clock);
			}

			m_query_manager->process_query_queue(p_clock);
		}

		void bc_game_system::swap_frame(const platform::bc_clock::update_param& p_clock)
		{
			if (m_scene_changed)
			{
				_change_scene(p_clock);
			}

			m_query_manager->process_query_queue(p_clock);
			m_query_manager->swap_frame();
			m_render_system->swap_frame(bc_render_system::swap_context(p_clock));
		}

		void bc_game_system::render_swap_frame(const platform::bc_clock::update_param& p_clock)
		{
			m_render_system->render_swap_frame(bc_render_system::swap_context(p_clock));
		}

		void bc_game_system::_initialize(bc_game_system_parameter p_parameter)
		{
			m_query_manager = &p_parameter.m_query_manager;
			m_event_manager = &p_parameter.m_event_manager;

			m_file_system = core::bc_make_unique<bc_file_system>
			(
				core::bc_alloc_type::program,
				*core::bc_get_service<core::bc_content_manager>(),
				*core::bc_get_service<core::bc_content_stream_manager>()
			);
			m_input_system = core::bc_make_unique<bc_input_system>(core::bc_alloc_type::program, p_parameter.m_output_window);
			m_physics_system = core::bc_make_unique<bc_physics_system>(core::bc_alloc_type::program);
			m_physics_system->initialize();
			m_sound_system = core::bc_make_unique<bc_sound_system>(core::bc_alloc_type::program);
			m_sound_system->initialize(bc_sound_system_parameter{ 32 });
			m_animation_system = core::bc_make_unique<bc_animation_system>(core::bc_alloc_type::program);
			m_animation_system->initialize();
			m_network_system = core::bc_make_unique<bc_network_system>(core::bc_alloc_type::program);
			m_network_system->initialize(bc_network_system_parameter{ *m_event_manager, *this });
			m_script_system = core::bc_make_unique<bc_script_system>(core::bc_alloc_type::program);
			m_script_system->initialize(true);
			m_render_system = core::bc_make_unique<bc_render_system>(core::bc_alloc_type::program);
			m_render_system->initialize(bc_render_system_parameter
			(
				p_parameter.m_content_stream,
				*m_physics_system,
				p_parameter.m_device_backbuffer_width,
				p_parameter.m_device_backbuffer_height,
				p_parameter.m_device_backbuffer_format,
				p_parameter.m_output_window ? p_parameter.m_output_window->get_device_output() : graphic::bc_device_output()
			));
			m_console = core::bc_make_unique<bc_game_console>(core::bc_alloc_type::program, *m_script_system);

			m_pause_event_handle = m_event_manager->register_event_listener<bc_event_game_pause_state>
			(
				core::bc_event_manager::delegate_type(*this, &bc_game_system::_event_handler)
			);
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
			m_pause_event_handle.reset();
			m_editor_event_handle.reset();
			m_scene_query_context_provider.reset();

			m_render_system->destroy_render_passes();
			m_query_manager->clear_queries(); // Clear queries to release probable references to render states
			
			m_scene.reset();
			m_console.reset();

			m_render_system->destroy();
			m_script_system->destroy();
			m_network_system->destroy();
			m_animation_system->destroy();
			m_sound_system->destroy();
			m_physics_system->destroy();
		}

		void bc_game_system::_change_scene(const platform::bc_clock::update_param& p_clock)
		{
			const auto l_is_changed = m_scene != m_new_scene;
			if(!l_is_changed)
			{
				return;
			}
			
			bc_event_scene_change l_before_change_event(m_scene.get(), true);
			m_event_manager->process_event(l_before_change_event);

			m_scene = std::move(m_new_scene);
			m_new_scene = nullptr;
			m_scene_changed = false;

			if (m_scene)
			{
				// Process actor events to apply initial transforms and update scene graph so actors can be queryable in scene change event.
				m_scene->process_actor_events_and_reform_graph(p_clock);

				if (m_paused)
				{
					m_scene->paused();
				}
				else
				{
					m_scene->resumed();
				}
			}
			
			bc_event_scene_change l_after_change_event(m_scene.get(), false);
			m_event_manager->process_event(l_after_change_event);
		}

		void bc_game_system::_event_handler(core::bci_event& p_event)
		{
			if(const auto* l_pause_event = core::bci_message::as<bc_event_game_pause_state>(p_event))
			{
				const auto l_current_pause_state = m_paused;
				m_paused = l_pause_event->get_state() == bc_event_game_pause_state::state::paused;

				if(m_paused && !l_current_pause_state)
				{
					m_pause_last_total_elapsed.reset();
				}
			}

			if (const auto* l_editor_event = core::bci_message::as<bc_event_editor_mode>(p_event))
			{
				m_editor_mode = l_editor_event->get_editor_mode();
				if(m_editor_mode)
				{
					m_paused = false;
				}
			}

			if (m_scene)
			{
				if (m_paused)
				{
					m_scene->paused();
				}
				else
				{
					m_scene->resumed();
				}
			}
		}
	}
}
