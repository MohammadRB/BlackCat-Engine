// [04/29/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/bcConstant.h"
#include "Core/Container/bcString.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Core/Messaging/Query/bcQueryProviderHandle.h"
#include "Core/Utility/bcServiceManager.h"
#include "Game/bcExport.h"
#include "Game/System/Input/bcInputSystem.h"
#include "Game/System/Input/bcFileSystem.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Physics/bcPhysicsSystem.h"
#include "Game/System/Script/bcScriptSystem.h"
#include "Game/System/Script/bcGameConsole.h"
#include "Game/Object/Scene/bcScene.h"

namespace black_cat
{
	namespace game
	{
		struct bc_game_system_parameter
		{
			explicit bc_game_system_parameter(bc_render_system_parameter&& p_render_system_parameter)
				: m_render_system_parameter(std::move(p_render_system_parameter))
			{
			}

			bc_render_system_parameter m_render_system_parameter;
		};

		class BC_GAME_DLL bc_game_system : public core::bc_iservice, public core::bc_initializable< bc_game_system_parameter >
		{
			BC_SERVICE(gme_sys)

		public:
			bc_game_system();

			bc_game_system(bc_game_system&&) noexcept = delete;

			~bc_game_system();

			bc_game_system& operator=(bc_game_system&&) noexcept = delete;

			bc_input_system& get_input_system()
			{
				return m_input_system;
			}

			const bc_input_system& get_input_system() const
			{
				return m_input_system;
			}

			bc_file_system& get_file_system()
			{
				return m_file_system;
			}

			const bc_file_system& get_file_system() const
			{
				return m_file_system;
			}

			bc_render_system& get_render_system()
			{
				return m_render_system;
			}

			const bc_render_system& get_render_system() const
			{
				return m_render_system;
			}

			bc_physics_system& get_physics_system()
			{
				return m_physics_system;
			}

			const bc_physics_system& get_physics_system() const
			{
				return m_physics_system;
			}

			bc_script_system& get_script_system()
			{
				return m_script_system;
			}

			const bc_script_system& get_script_system() const
			{
				return m_script_system;
			}

			bc_game_console& get_console()
			{
				return *m_console;
			}

			const bc_game_console& get_console() const
			{
				return *m_console;
			}

			bc_scene* get_scene()
			{
				return m_scene.get();
			}

			const bc_scene* get_scene() const
			{
				return m_scene.get();
			}

			void set_scene(bc_scene_ptr p_scene);
			
			/**
			 * \brief Start updating game core logic
			 * \param p_clock 
			 */
			void update_game(const core_platform::bc_clock::update_param& p_clock);
			
			/**
			 * \brief Start rendering game core logic
			 * \param p_clock 
			 */
			void render_game(const core_platform::bc_clock::update_param& p_clock);

			void swap_frame(const core_platform::bc_clock::update_param& p_clock);

		private:
			void _initialize(bc_game_system_parameter) override;

			void _destroy() override;

			bc_file_system m_file_system;
			bc_input_system m_input_system;
			bc_physics_system m_physics_system;
			bc_script_system m_script_system;
			bc_render_system m_render_system;
			core::bc_unique_ptr<bc_game_console> m_console;
			bc_scene_ptr m_scene;

			core_platform::bc_clock::big_delta_time m_last_total_elapsed;

			core::bc_query_provider_handle m_scene_query_context_provider;
		};
	}
}