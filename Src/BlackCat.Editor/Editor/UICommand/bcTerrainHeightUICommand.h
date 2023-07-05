// [20/02/2017 MRB]

#pragma once

#include "Game/System/Render/bcRenderTask.h"
#include "Editor/Application/bcUICommand.h"
#include "Editor/UICommand/bcTerrainUICommand.h"

namespace black_cat::editor
{
	class bc_editor_height_map_dx11;

	struct bc_terrain_height_ui_command_parameter_cbuffer
	{
		BC_CBUFFER_ALIGN
		bcUINT32 m_tool_center_x;
		bcUINT32 m_tool_center_z;
		bcUINT32 m_tool_radius;
		bcFLOAT m_tool_height;
	};

	using bc_terrain_height_ui_command_state = bc_terrain_ui_command_state;

	class bc_terrain_height_ui_command : public bc_ui_terrain_command
	{
	public:
		bc_terrain_height_ui_command(bcUINT16 p_screen_width, 
		                             bcUINT16 p_screen_height,
		                             bcUINT16 p_point_left,
		                             bcUINT16 p_point_top,
		                             bcUINT16 p_radius,
		                             bcFLOAT p_height);

		bc_terrain_height_ui_command(const bc_terrain_height_ui_command&);

		~bc_terrain_height_ui_command() override;

		bc_terrain_height_ui_command& operator=(const bc_terrain_height_ui_command&);

		core::bc_string title() const override;

		state_ptr create_state(state_context& p_context) const override;

	protected:
		bool update(terrain_update_context& p_context) override;

	private:
		bcFLOAT m_height;
	};

	class bc_terrain_height_ui_command_render_task : public game::bci_render_task
	{
	public:
		bc_terrain_height_ui_command_render_task(const bc_editor_height_map_dx11& p_height_map, 
		                                         bc_terrain_height_ui_command_state& p_command_state,
		                                         const bc_terrain_height_ui_command_parameter_cbuffer& p_shader_parameter);

		void execute(game::bc_render_system& p_render_system, game::bc_render_thread& p_render_thread) override;

		const bc_editor_height_map_dx11& m_height_map;
		bc_terrain_height_ui_command_state& m_command_state;
		bc_terrain_height_ui_command_parameter_cbuffer m_shader_parameter;
	};
}
