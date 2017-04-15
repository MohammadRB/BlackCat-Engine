// [02/20/2017 MRB]

#pragma once

#include "Game/System/Render/bcRenderTask.h"
#include "Editor/Application/bcUICommand.h"
#include "Editor/UICommand/bcUITerrainCommand.h"

namespace black_cat
{
	namespace editor
	{
		class bc_editor_height_map_dx11;

		struct bc_ui_terrain_height_command_parameter_cbuffer
		{
			BC_CBUFFER_ALIGN
			bcUINT32 m_tool_center_x;
			bcUINT32 m_tool_center_z;
			bcUINT32 m_tool_radius;
			bcINT32 m_tool_height;
		};

		using bc_ui_terrain_height_commnad_state = bc_ui_terrain_commnad_state;

		class bc_ui_terrain_height_command : public bc_ui_terrain_command
		{
		public:
			bc_ui_terrain_height_command(bcUINT16 p_screen_width, 
				bcUINT16 p_screen_height,
				bcUINT16 p_point_left,
				bcUINT16 p_point_top,
				bcUINT16 p_radius,
				bcINT16 p_height);

			bc_ui_terrain_height_command(const bc_ui_terrain_height_command&);

			~bc_ui_terrain_height_command();

			bc_ui_terrain_height_command& operator=(const bc_ui_terrain_height_command&);

			core::bc_string title() const override;

			state_ptr create_state(state_context& p_context) const override;

		protected:
			bool update(terrain_update_context& p_context) override;

		private:
			bcINT16 m_height;
		};

		class bc_ui_terrain_height_command_render_task : public game::bc_irender_task
		{
		public:
			bc_ui_terrain_height_command_render_task(const bc_editor_height_map_dx11& p_height_map, 
				bc_ui_terrain_height_commnad_state& p_command_state,
				const bc_ui_terrain_height_command_parameter_cbuffer& p_shader_parameter);

			void execute(game::bc_render_system& p_render_system, game::bc_render_thread& p_render_thread) override;

			const bc_editor_height_map_dx11& m_height_map;
			bc_ui_terrain_height_commnad_state& m_command_state;
			bc_ui_terrain_height_command_parameter_cbuffer m_shader_parameter;
		};
	}
}