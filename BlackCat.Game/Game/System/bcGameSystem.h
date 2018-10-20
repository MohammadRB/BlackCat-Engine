// [04/29/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/bcConstant.h"
#include "Core/Container/bcString.h"
#include "Core/File/bcContentStreamManager.h"
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

		class bc_game_system : public core::bc_iservice, public core::bc_initializable< bc_game_system_parameter >
		{
			BC_SERVICE(game_system)

		public:
			bc_game_system();

			bc_game_system(bc_game_system&&) = default;

			~bc_game_system();

			bc_game_system& operator=(bc_game_system&&) = default;

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
				return m_console;
			}

			const bc_game_console& get_console() const
			{
				return m_console;
			}

			bc_scene* get_scene()
			{
				return m_scene.get();
			}

			bc_scene* get_scene() const
			{
				return m_scene.get();
			}

			void set_scene(core::bc_unique_ptr<bc_scene> p_scene);

			void render();

		protected:
			void update(core_platform::bc_clock::update_param p_clock_update_param) override;

		private:
			void _initialize(bc_game_system_parameter) override;

			void _destroy() override;

			bc_input_system m_input_system;
			bc_file_system m_file_system;
			bc_physics_system m_physics_system;
			bc_script_system m_script_system;
			bc_render_system m_render_system;
			bc_game_console m_console;
			core::bc_unique_ptr<bc_scene> m_scene;
		};

		inline bc_game_system::bc_game_system()
			: m_input_system(),
			m_file_system(),
			m_physics_system(),
			m_script_system(),
			m_render_system(),
			m_console(m_script_system)
		{	
		}

		inline bc_game_system::~bc_game_system()
		{
			if (m_initialized)
			{
				destroy();
			}
		}

		inline void bc_game_system::set_scene(core::bc_unique_ptr<bc_scene> p_scene)
		{
			m_scene = std::move(p_scene);
		}

		inline void bc_game_system::render()
		{
			m_render_system.render(*m_scene.get());
		}

		inline void bc_game_system::update(core_platform::bc_clock::update_param p_clock_update_param)
		{
			bc_icamera::extend l_camera_extends;

			m_input_system.update(p_clock_update_param);

			m_input_system.get_camera().get_extend_points(l_camera_extends);
			auto l_render_system_update_params = bc_render_system::update_param
			(
				p_clock_update_param,
				m_input_system.get_camera().get_position(),
				m_input_system.get_camera().get_view(),
				m_input_system.get_camera().get_projection(),
				l_camera_extends
			);

			m_physics_system.update(p_clock_update_param);
			m_script_system.update(p_clock_update_param);
			m_render_system.update(l_render_system_update_params);
			m_console.update(p_clock_update_param);

			m_scene->update(m_physics_system, p_clock_update_param);
		}

		inline void bc_game_system::_initialize(bc_game_system_parameter p_parameter)
		{
			m_render_system.initialize(*core::bc_get_service<core::bc_content_stream_manager>(), std::move(p_parameter.m_render_system_parameter));
			m_physics_system.initialize();

			auto l_px_scene_builder = std::move(physics::bc_scene_builder()
				.enable_ccd());
			auto l_px_scene = m_physics_system.get_physics().create_scene(std::move(l_px_scene_builder));
			auto l_scene_graph = bc_scene_graph();

			m_scene = core::bc_make_unique<bc_scene>(std::move(l_scene_graph), std::move(l_px_scene));
		}

		inline void bc_game_system::_destroy()
		{
			m_scene.reset();
			m_physics_system.destroy();
			m_render_system.destroy();
		}
	}
}