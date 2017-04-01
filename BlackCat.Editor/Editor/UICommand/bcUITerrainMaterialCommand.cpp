// [03/21/2017 MRB]

#include "Editor/EditorPCH.h"

#include "Game/System/Physics/bcPxWrap.h"
#include "Game/Object/Scene/Component/bcRigidStaticComponent.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"
#include "Editor/Application/bcEditorHeightMapLoaderDx11.h"
#include "Editor/UICommand/bcUITerrainMaterialCommand.h"

namespace black_cat
{
	namespace editor
	{
		// == bc_ui_terrain_material_command ==================================================================================

		bc_ui_terrain_material_command::bc_ui_terrain_material_command(bcUINT16 p_screen_width,
			bcUINT16 p_screen_height,
			bcUINT16 p_point_left,
			bcUINT16 p_point_top,
			bcUINT16 p_radius,
			bcUINT16 p_material)
			: m_screen_width(p_screen_width),
			m_screen_height(p_screen_height),
			m_point_left(p_point_left),
			m_point_top(p_point_top),
			m_radius(p_radius),
			m_material(p_material)
		{
		}

		bc_ui_terrain_material_command::bc_ui_terrain_material_command(const bc_ui_terrain_material_command&) = default;

		bc_ui_terrain_material_command::~bc_ui_terrain_material_command() = default;

		bc_ui_terrain_material_command& bc_ui_terrain_material_command::operator=(const bc_ui_terrain_material_command&) = default;

		core::bc_string bc_ui_terrain_material_command::title() const
		{
			return "Terrain Material";
		}

		bc_iui_commnad::state_ptr bc_ui_terrain_material_command::create_state(state_context& p_context) const
		{
			auto& l_render_system = p_context.m_game_system.get_render_system();

			auto l_cb_config = graphic::bc_graphic_resource_configure().as_resource()
				.as_buffer
				(
					1,
					sizeof(bc_ui_terrain_material_command_parameter_cbuffer),
					graphic::bc_resource_usage::gpu_rw,
					graphic::bc_resource_view_type::none
				)
				.as_constant_buffer();

			auto l_cbuffer = l_render_system.get_device().create_buffer(l_cb_config, nullptr);
			auto l_device_compute_state = l_render_system.create_device_compute_state("terrain_material_cs");
			auto l_device_command_list = l_render_system.get_device().create_command_list();

			bc_ui_terrain_material_commnad_state l_state;
			l_state.m_device_compute_state = l_device_compute_state;
			l_state.m_parameter_cbuffer = l_cbuffer;
			l_state.m_device_command_list = l_device_command_list;

			return core::bc_make_unique< bc_ui_terrain_material_commnad_state >(std::move(l_state));
		}

		bool bc_ui_terrain_material_command::update(update_context& p_context)
		{
			auto& l_camera = p_context.m_game_system.get_input_system().get_camera();

			auto l_pointing_ray = l_camera.project_clip_point_to_3d_ray(m_screen_width, m_screen_height, m_point_left, m_point_top);
			physics::bc_scene_ray_query_buffer l_buffer(0);
			bool l_px_hit_result = p_context.m_game_system.get_scene()->get_px_scene().raycast
			(
				physics::bc_ray(l_camera.get_position(), l_pointing_ray, l_camera.get_far_clip()),
				l_buffer,
				physics::bc_hit_flag::hit_info,
				core::bc_enum:: or ({ physics::bc_query_flags::statics }),
				static_cast< physics::bc_query_group >(game::bc_query_group::terrain)
			);

			if (!l_px_hit_result && !l_buffer.has_block())
			{
				return false;
			}

			game::bc_ray_hit l_hit = l_buffer.get_block();

			m_terrain = l_hit.get_actor();

			//auto* l_rigid_component = m_terrain.get_component<game::bc_rigid_static_component>();
			auto* l_height_map_component = m_terrain.get_component<game::bc_height_map_component>();
			auto* l_dx11_height_map = static_cast<const bc_editor_height_map_dx11*>(l_height_map_component->get_height_map());
			//auto l_px_height_map = l_dx11_height_map->get_px_height_field();

			m_hit_position = l_hit.get_position() - l_dx11_height_map->get_position();

			bcUINT16 l_half_width = l_dx11_height_map->get_width() * l_dx11_height_map->get_xz_multiplier() / 2;
			bcUINT16 l_half_height = l_dx11_height_map->get_height() * l_dx11_height_map->get_xz_multiplier() / 2;

			bc_ui_terrain_material_command_parameter_cbuffer l_cbuffer_parameters;
			l_cbuffer_parameters.m_tool_center_x = (static_cast< bcINT32 >(m_hit_position.x) + l_half_width) / l_dx11_height_map->get_xz_multiplier();
			l_cbuffer_parameters.m_tool_center_z = l_dx11_height_map->get_height() - ((static_cast< bcINT32 >(m_hit_position.z) + l_half_height) / l_dx11_height_map->get_xz_multiplier());
			l_cbuffer_parameters.m_tool_radius = m_radius;
			l_cbuffer_parameters.m_tool_material = m_material;

			bc_ui_terrain_material_command_render_task l_render_task
			(
				*l_dx11_height_map,
				*static_cast< bc_ui_terrain_material_commnad_state* >(p_context.m_state),
				l_cbuffer_parameters
			);
			p_context.m_game_system.get_render_system().add_render_task(l_render_task);

			l_render_task.wait();

			return false;
		}

		// == bc_ui_terrain_material_command_render_task ======================================================================

		bc_ui_terrain_material_command_render_task::bc_ui_terrain_material_command_render_task(const bc_editor_height_map_dx11& p_height_map,
			bc_ui_terrain_material_commnad_state& p_command_state,
			const bc_ui_terrain_material_command_parameter_cbuffer& p_shader_parameter)
			: m_height_map(p_height_map),
			m_command_state(p_command_state),
			m_shader_parameter(p_shader_parameter)
		{
		}

		void bc_ui_terrain_material_command_render_task::execute(game::bc_render_system& p_render_system, game::bc_render_thread& p_render_thread)
		{
			auto l_tool_diameter = m_shader_parameter.m_tool_radius * 2;
			auto l_thread_group_count = (l_tool_diameter / 32) + 1;

			auto l_compute_state = p_render_system.create_compute_state
			(
				m_command_state.m_device_compute_state.get(),
				l_thread_group_count,
				l_thread_group_count,
				1,
				{},
				{},
				{
					graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, m_height_map.get_texture_map_unordered_view()) 
				},
				{
					graphic::bc_constant_buffer_parameter(0, graphic::bc_shader_type::compute, m_height_map.get_parameter_cbuffer()),
					graphic::bc_constant_buffer_parameter(0, graphic::bc_shader_type::compute, m_command_state.m_parameter_cbuffer)
				}
			);

			p_render_thread.start(m_command_state.m_device_command_list.get());

			p_render_thread.update_subresource
			(
				m_command_state.m_parameter_cbuffer.get(),
				0,
				&m_shader_parameter,
				0,
				0
			);
			p_render_thread.run_compute_shader(l_compute_state.get());

			p_render_thread.finish();
			m_command_state.m_device_command_list->reset();
		}
	}
}