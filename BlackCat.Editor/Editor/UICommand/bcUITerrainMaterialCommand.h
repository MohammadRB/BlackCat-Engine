// [03/21/2017 MRB]

#pragma once

#include "GraphicImp/Device/bcDeviceComputeState.h"
#include "GraphicImp/Device/Command/bcDeviceCommandList.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "Game/System/Render/bcRenderTask.h"
#include "Editor/Application/bcUICommand.h"

namespace black_cat
{
	namespace editor
	{
		class bc_editor_height_map_dx11;

		struct bc_ui_terrain_material_command_parameter_cbuffer
		{
			BC_CBUFFER_ALIGN
			bcUINT32 m_tool_center_x;
			bcUINT32 m_tool_center_z;
			bcUINT32 m_tool_radius;
			bcUINT32 m_tool_material;
		};

		class bc_ui_terrain_material_commnad_state : public bc_iui_command_state
		{
		public:
			graphic::bc_buffer_ptr m_parameter_cbuffer;
			graphic::bc_device_compute_state_ptr m_device_compute_state;
			graphic::bc_device_command_list_ptr m_device_command_list;
		};

		class bc_ui_terrain_material_command : public bc_iui_commnad
		{
		public:
			bc_ui_terrain_material_command(bcUINT16 p_screen_width,
				bcUINT16 p_screen_height,
				bcUINT16 p_point_left,
				bcUINT16 p_point_top,
				bcUINT16 p_radius,
				bcUINT16 p_material);

			bc_ui_terrain_material_command(const bc_ui_terrain_material_command&);

			~bc_ui_terrain_material_command();

			bc_ui_terrain_material_command& operator=(const bc_ui_terrain_material_command&);

			core::bc_string title() const override;

			state_ptr create_state(state_context& p_context) const override;

			bool update(update_context& p_context) override;

		protected:

		private:
			bcUINT16 m_screen_width;
			bcUINT16 m_screen_height;
			bcUINT16 m_point_left;
			bcUINT16 m_point_top;
			bcUINT16 m_radius;
			bcUINT16 m_material;

			game::bc_actor m_terrain;
			core::bc_vector3f m_hit_position;
		};

		class bc_ui_terrain_material_command_render_task : public game::bc_irender_task
		{
		public:
			bc_ui_terrain_material_command_render_task(const bc_editor_height_map_dx11& p_height_map,
				bc_ui_terrain_material_commnad_state& p_command_state,
				const bc_ui_terrain_material_command_parameter_cbuffer& p_shader_parameter);

			void execute(game::bc_render_system& p_render_system, game::bc_render_thread& p_render_thread) override;

			const bc_editor_height_map_dx11& m_height_map;
			bc_ui_terrain_material_commnad_state& m_command_state;
			bc_ui_terrain_material_command_parameter_cbuffer m_shader_parameter;
		};
	}
}