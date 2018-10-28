// [04/02/2017 MRB]

#include "Editor/EditorPCH.h"

#include "Physics/Collision/bcQueryGroup.h"
#include "PhysicsImp/Collision/bcSceneQuery.h"
#include "Game/System/Physics/bcPxWrap.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"
#include "Editor/UICommand/bcUITerrainCommand.h"
#include "Editor/Application/bcEditorHeightMapLoaderDx11.h"
#include "BlackCat/RenderPass/bcTerrainPassDx11.h"

namespace black_cat
{
	namespace editor
	{
		bc_ui_terrain_command_update_context::bc_ui_terrain_command_update_context(const bc_ui_command_update_context& p_update_context,
			const game::bc_actor& p_terrain,
			const core::bc_vector3f& p_hit_position,
			bcUINT32 m_tool_center_x,
			bcUINT32 m_tool_center_z)
			: bc_ui_command_update_context(p_update_context),
			m_terrain(p_terrain),
			m_hit_position(p_hit_position),
			m_tool_center_x(m_tool_center_x),
			m_tool_center_z(m_tool_center_z)
		{
		}

		bc_ui_terrain_command::bc_ui_terrain_command(bcUINT16 p_screen_width,
			bcUINT16 p_screen_height,
			bcUINT16 p_point_left,
			bcUINT16 p_point_top,
			bcUINT16 p_radius)
			: m_screen_width(p_screen_width),
			m_screen_height(p_screen_height),
			m_point_left(p_point_left),
			m_point_top(p_point_top),
			m_radius(p_radius)
		{
		}

		bc_ui_terrain_command::bc_ui_terrain_command(const bc_ui_terrain_command&) = default;

		bc_ui_terrain_command::~bc_ui_terrain_command() = default;

		bc_ui_terrain_command& bc_ui_terrain_command::operator=(const bc_ui_terrain_command&) = default;

		bool bc_ui_terrain_command::update(update_context& p_context)
		{
			const auto& l_camera = p_context.m_game_system.get_input_system().get_camera();
			const auto l_pointing_ray = l_camera.project_clip_point_to_3d_ray(m_screen_width, m_screen_height, m_point_left, m_point_top);
			physics::bc_scene_ray_query_buffer l_buffer(0);
			const bool l_px_hit_result = p_context.m_game_system.get_scene()->get_px_scene().raycast
			(
				physics::bc_ray(l_camera.get_position(), l_pointing_ray, l_camera.get_far_clip()),
				l_buffer,
				physics::bc_hit_flag::hit_info,
				core::bc_enum::or({ physics::bc_query_flags::statics }),
				static_cast< physics::bc_query_group >(game::bc_query_group::terrain)
			);

			if (!l_px_hit_result && !l_buffer.has_block())
			{
				return false;
			}

			game::bc_ray_hit l_hit = l_buffer.get_block();
			game::bc_actor l_terrain = l_hit.get_actor();

			const auto* l_height_map_component = l_terrain.get_component< game::bc_height_map_component >();
			const auto* l_dx11_height_map = static_cast< const bc_editor_height_map_dx11* >(l_height_map_component->get_height_map());

			const auto l_hit_position = l_hit.get_position() - l_dx11_height_map->get_position();

			const bcUINT16 l_half_width = l_dx11_height_map->get_width() * l_dx11_height_map->get_xz_multiplier() / 2;
			const bcUINT16 l_half_height = l_dx11_height_map->get_height() * l_dx11_height_map->get_xz_multiplier() / 2;

			const bcUINT32 l_tool_center_x = (static_cast< bcINT32 >(l_hit_position.x) + l_half_width) / l_dx11_height_map->get_xz_multiplier();
			const bcUINT32 l_tool_center_z = l_dx11_height_map->get_height() - ((static_cast< bcINT32 >(l_hit_position.z) + l_half_height) / l_dx11_height_map->get_xz_multiplier());

			terrain_update_context l_context(p_context, l_terrain, l_hit_position, l_tool_center_x, l_tool_center_z);

			const bool l_result = update(l_context);

			p_context.m_game_system.get_render_system().get_render_pass<bc_terrain_pass_dx11>()->update_chunk_infoes();

			return l_result;
		}
	}
}