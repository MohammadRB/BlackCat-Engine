// [02/08/2019 MRB]

#pragma once

#include "Core/Container/bcString.h"
#include "Game/Object/Scene/bcScene.h"
#include "Editor/Application/bcUICommand.h"

namespace black_cat
{
	namespace editor
	{
		enum class bc_ui_scene_command_type
		{
			get_scene,
			new_scene,
			load_scene,
			save_scene,
			save_as_scene,
		};

		class bc_ui_scene_command : public bc_iui_command
		{
		public:
			using command_type = bc_ui_scene_command_type;

		public:
			bc_ui_scene_command(const bc_ui_scene_command&) = default;

			bc_ui_scene_command(bc_ui_scene_command&&) = default;

			~bc_ui_scene_command() = default;

			bc_ui_scene_command& operator=(const bc_ui_scene_command&) = default;

			bc_ui_scene_command& operator=(bc_ui_scene_command&&) = default;

			core::bc_string title() const override;

			state_ptr create_state(state_context& p_context) const override;

			bool update(update_context& p_context) override;

			static bc_ui_scene_command for_get_scene();

			static bc_ui_scene_command for_new_scene();

			static bc_ui_scene_command for_load_scene(core::bc_estring p_path);

			static bc_ui_scene_command for_save_scene(game::bc_scene* p_scene);

			static bc_ui_scene_command for_save_as_scene(game::bc_scene* p_scene, core::bc_estring p_path);

		private:
			bc_ui_scene_command(command_type p_type, core::bc_estring p_scene_path);

			bc_ui_scene_command(command_type p_type, game::bc_scene* p_scene);

			bc_ui_scene_command(command_type p_type, core::bc_estring p_scene_path, game::bc_scene* p_scene);

			command_type m_type;
			core::bc_estring m_scene_path;
			game::bc_scene* m_scene;
		};
	}
}