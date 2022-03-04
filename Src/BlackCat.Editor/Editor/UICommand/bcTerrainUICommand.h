// [04/02/2017 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "GraphicImp/Device/bcDeviceComputeState.h"
#include "GraphicImp/Device/Command/bcDeviceCommandList.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Editor/Application/bcUICommand.h"

namespace black_cat
{
	namespace editor
	{
		class bc_terrain_ui_command_state : public bci_ui_command_state
		{
		public:
			graphic::bc_buffer_ref m_parameter_cbuffer;
			graphic::bc_device_command_list_ref m_device_command_list;
			graphic::bc_device_compute_state_ref m_device_compute_state;
		};

		class bc_ui_terrain_command_update_context : public bc_ui_command_update_context
		{
		public:
			bc_ui_terrain_command_update_context(const bc_ui_command_update_context& p_update_context,
				const game::bc_actor& p_terrain,
				const core::bc_vector3f& p_hit_position,
				bcUINT32 p_tool_center_x,
				bcUINT32 p_tool_center_z);

			game::bc_actor m_terrain;
			core::bc_vector3f m_hit_position;	// pointed position in terrain local space
			bcUINT32 m_tool_center_x;			// pointed position in terrain local space converted to [0, width] range
			bcUINT32 m_tool_center_z;			// pointed position in terrain local space converted to [0, height] range
		};

		class bc_ui_terrain_command : public bci_ui_command
		{
		public:
			using terrain_update_context = bc_ui_terrain_command_update_context;

		public:
			bc_ui_terrain_command(bcUINT16 p_screen_width,
				bcUINT16 p_screen_height,
				bcUINT16 p_point_left,
				bcUINT16 p_point_top,
				bcUINT16 p_radius);

			bc_ui_terrain_command(const bc_ui_terrain_command&);

			~bc_ui_terrain_command() override;

			bc_ui_terrain_command& operator=(const bc_ui_terrain_command&);

			bool update(update_context& p_context) override;

		protected:
			virtual bool update(terrain_update_context& p_context) = 0;

			bcUINT16 m_screen_width;
			bcUINT16 m_screen_height;
			bcUINT16 m_point_left;
			bcUINT16 m_point_top;
			bcUINT16 m_radius;
		};
	}
}