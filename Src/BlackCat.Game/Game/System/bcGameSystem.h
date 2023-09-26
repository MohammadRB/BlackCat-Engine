// [29/04/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/bcConstant.h"
#include "Core/Container/bcString.h"
#include "Core/Messaging/Event/bcEventListenerHandle.h"
#include "Core/Messaging/Query/bcQueryProviderHandle.h"
#include "Core/Content/bcContent.h"
#include "Core/Utility/bcServiceManager.h"
#include "Core/Utility/bcOptional.h"
#include "Game/System/bcGameSystemParameter.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace core
	{
		class bc_query_manager;
		class bc_event_manager;
	}
	
	namespace game
	{
		class bc_file_system;
		class bc_input_system;
		class bc_physics_system;
		class bc_network_system;
		class bc_sound_system;
		class bc_animation_system;
		class bc_script_system;
		class bc_render_system;
		class bc_game_console;
		class bc_scene;
		using bc_scene_ptr = core::bc_content_ptr<bc_scene>;

		class BC_GAME_DLL bc_game_system : public core::bci_service, public core::bc_initializable<bc_game_system_parameter>
		{
			BC_SERVICE(gme_sys)

		public:
			bc_game_system();

			bc_game_system(bc_game_system&&) noexcept = delete;

			~bc_game_system() override;

			bc_game_system& operator=(bc_game_system&&) noexcept = delete;

			bc_input_system& get_input_system() noexcept
			{
				return *m_input_system;
			}

			const bc_input_system& get_input_system() const noexcept
			{
				return *m_input_system;
			}

			bc_file_system& get_file_system() noexcept
			{
				return *m_file_system;
			}

			const bc_file_system& get_file_system() const noexcept
			{
				return *m_file_system;
			}

			bc_physics_system& get_physics_system() noexcept
			{
				return *m_physics_system;
			}

			const bc_physics_system& get_physics_system() const noexcept
			{
				return *m_physics_system;
			}

			bc_network_system& get_network_system() noexcept
			{
				return *m_network_system;
			}

			const bc_network_system& get_network_system() const noexcept
			{
				return *m_network_system;
			}

			bc_script_system& get_script_system() noexcept
			{
				return *m_script_system;
			}

			const bc_script_system& get_script_system() const noexcept
			{
				return *m_script_system;
			}

			bc_render_system& get_render_system() noexcept
			{
				return *m_render_system;
			}

			const bc_render_system& get_render_system() const noexcept
			{
				return *m_render_system;
			}

			bc_sound_system& get_sound_system() noexcept
			{
				return *m_sound_system;
			}

			const bc_sound_system& get_sound_system() const noexcept
			{
				return *m_sound_system;
			}

			bc_animation_system& get_animation_system() noexcept
			{
				return *m_animation_system;
			}

			const bc_animation_system& get_animation_system() const noexcept
			{
				return *m_animation_system;
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
			void _initialize(bc_game_system_parameter) override;

			void _destroy() override;

			void _change_scene(const platform::bc_clock::update_param& p_clock);

			void _event_handler(core::bci_event& p_event);

			core::bc_query_manager* m_query_manager;
			core::bc_event_manager* m_event_manager;
			core::bc_unique_ptr<bc_file_system> m_file_system;
			core::bc_unique_ptr<bc_input_system> m_input_system;
			core::bc_unique_ptr<bc_physics_system> m_physics_system;
			core::bc_unique_ptr<bc_sound_system> m_sound_system;
			core::bc_unique_ptr<bc_animation_system> m_animation_system;
			core::bc_unique_ptr<bc_network_system> m_network_system;
			core::bc_unique_ptr<bc_script_system> m_script_system;
			core::bc_unique_ptr<bc_render_system> m_render_system;
			core::bc_unique_ptr<bc_game_console> m_console;

			bool m_scene_changed;
			bc_scene_ptr m_scene;
			bc_scene_ptr m_new_scene;

			core::bc_event_listener_handle m_pause_event_handle;
			core::bc_event_listener_handle m_editor_event_handle;
			core::bc_query_provider_handle m_scene_query_context_provider;

			bool m_paused;
			bool m_editor_mode;
			core::bc_optional<platform::bc_clock::big_time> m_pause_last_total_elapsed;
		};
	}
}