// [04/29/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/bcConstant.h"
#include "Core/Container/bcString.h"
#include "Core/Messaging/Event/bcEventListenerHandle.h"
#include "Core/Messaging/Query/bcQueryProviderHandle.h"
#include "Core/Utility/bcServiceManager.h"
#include "Game/bcExport.h"
#include "Game/System/Input/bcInputSystem.h"
#include "Game/System/Input/bcFileSystem.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Physics/bcPhysicsSystem.h"
#include "Game/System/Network/bcNetworkSystem.h"
#include "Game/System/Script/bcScriptSystem.h"
#include "Game/System/Script/bcGameConsole.h"
#include "Game/Object/Scene/bcScene.h"

namespace black_cat
{
	namespace core
	{
		class bc_query_manager;
		class bc_event_manager;
	}
	
	namespace game
	{
		class bc_sound_system;

		struct bc_game_system_init_params
		{
			explicit bc_game_system_init_params(core::bc_query_manager& p_query_manager,
				core::bc_event_manager& p_event_manager,
				bc_render_system_parameter p_render_system_parameter)
				: m_query_manager(&p_query_manager),
				m_event_manager(&p_event_manager),
				m_render_system_parameter(std::move(p_render_system_parameter))
			{
			}

			core::bc_query_manager* m_query_manager;
			core::bc_event_manager* m_event_manager;
			bc_render_system_parameter m_render_system_parameter;
		};

		class BC_GAME_DLL bc_game_system : public core::bci_service, public core::bc_initializable<bc_game_system_init_params>
		{
			BC_SERVICE(gme_sys)

		public:
			bc_game_system();

			bc_game_system(bc_game_system&&) noexcept = delete;

			~bc_game_system() override;

			bc_game_system& operator=(bc_game_system&&) noexcept = delete;

			bc_input_system& get_input_system() noexcept
			{
				return m_input_system;
			}

			const bc_input_system& get_input_system() const noexcept
			{
				return m_input_system;
			}

			bc_file_system& get_file_system() noexcept
			{
				return m_file_system;
			}

			const bc_file_system& get_file_system() const noexcept
			{
				return m_file_system;
			}

			bc_physics_system& get_physics_system() noexcept
			{
				return m_physics_system;
			}

			const bc_physics_system& get_physics_system() const noexcept
			{
				return m_physics_system;
			}

			bc_network_system& get_network_system() noexcept
			{
				return m_network_system;
			}

			const bc_network_system& get_network_system() const noexcept
			{
				return m_network_system;
			}

			bc_script_system& get_script_system() noexcept
			{
				return m_script_system;
			}

			const bc_script_system& get_script_system() const noexcept
			{
				return m_script_system;
			}

			bc_render_system& get_render_system() noexcept
			{
				return m_render_system;
			}

			const bc_render_system& get_render_system() const noexcept
			{
				return m_render_system;
			}

			bc_sound_system& get_sound_system() noexcept
			{
				return *m_sound_system;
			}

			const bc_sound_system& get_sound_system() const noexcept
			{
				return *m_sound_system;
			}

			bc_game_console& get_console() noexcept
			{
				return *m_console;
			}

			const bc_game_console& get_console() const noexcept
			{
				return *m_console;
			}

			bc_scene* get_scene() noexcept
			{
				return m_scene.get();
			}

			const bc_scene* get_scene() const noexcept
			{
				return m_scene.get();
			}

			void set_scene(bc_scene_ptr p_scene) noexcept;
			
			/**
			 * \brief Start updating game core logic
			 * \param p_clock
			 * \param p_is_partial_update 
			 */
			void update_game(const platform::bc_clock::update_param& p_clock, bool p_is_partial_update);
			
			/**
			 * \brief Start rendering game core logic
			 * \param p_clock 
			 */
			void render_game(const platform::bc_clock::update_param& p_clock);

			/**
			 * \brief Time before swap frame which main thread is idle 
			 * \param p_clock 
			 */
			void swap_frame_idle(const platform::bc_clock::update_param& p_clock);
			
			void swap_frame(const platform::bc_clock::update_param& p_clock);

			void render_swap_frame(const platform::bc_clock::update_param& p_clock);

		private:
			void _initialize(bc_game_system_init_params) override;

			void _destroy() override;

			void _event_handler(core::bci_event& p_event);

			core::bc_query_manager* m_query_manager;
			core::bc_event_manager* m_event_manager;
			bc_file_system m_file_system;
			bc_input_system m_input_system;
			bc_physics_system m_physics_system;
			bc_network_system m_network_system;
			bc_script_system m_script_system;
			bc_render_system m_render_system;
			core::bc_unique_ptr<bc_sound_system> m_sound_system;
			core::bc_unique_ptr<bc_game_console> m_console;

			bool m_scene_changed;
			bc_scene_ptr m_scene;
			bc_scene_ptr m_new_scene;

			core::bc_event_listener_handle m_pause_event_handle;
			core::bc_event_listener_handle m_editor_event_handle;
			core::bc_query_provider_handle m_scene_query_context_provider;

			bool m_paused;
			bool m_editor_mode;
			core::bc_nullable<platform::bc_clock::big_delta_time> m_pause_last_total_elapsed;
		};
	}
}