// [24/03/2017 MRB]

#include "Editor/EditorPCH.h"

#include "Game/System/Physics/bcPxWrap.h"
#include "Game/System/Render/bcRenderThread.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Scene/Component/bcRigidStaticComponent.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"
#include "Editor/Application/bcEditorHeightMapLoaderDx11.h"
#include "Editor/UICommand/bcTerrainMaterialSmoothUICommand.h"

namespace black_cat::editor
{
	bc_terrain_material_smooth_ui_command::bc_terrain_material_smooth_ui_command(bcUINT16 p_screen_width,
		bcUINT16 p_screen_height,
		bcUINT16 p_point_left,
		bcUINT16 p_point_top,
		bcUINT16 p_radius,
		bcUINT16 p_smooth)
		: bc_ui_terrain_command
		  (
			  p_screen_width,
			  p_screen_height,
			  p_point_left,
			  p_point_top,
			  p_radius
		  ),
		  m_smooth(p_smooth)
	{
	}

	bc_terrain_material_smooth_ui_command::bc_terrain_material_smooth_ui_command(const bc_terrain_material_smooth_ui_command&) = default;

	bc_terrain_material_smooth_ui_command::~bc_terrain_material_smooth_ui_command() = default;

	bc_terrain_material_smooth_ui_command& bc_terrain_material_smooth_ui_command::operator=(const bc_terrain_material_smooth_ui_command&) = default;

	core::bc_string bc_terrain_material_smooth_ui_command::title() const
	{
		return "TerrainMaterialSmooth";
	}

	bci_ui_command::state_ptr bc_terrain_material_smooth_ui_command::create_state(state_context& p_context) const
	{
		auto& l_render_system = p_context.m_game_system.get_render_system();

		const auto l_cb_config = graphic::bc_graphic_resource_builder()
		                         .as_resource()
		                         .as_buffer
		                         (
			                         1,
			                         sizeof(bc_terrain_material_smooth_ui_command_parameter_cbuffer),
			                         graphic::bc_resource_usage::gpu_rw,
			                         graphic::bc_resource_view_type::none
		                         )
		                         .as_constant_buffer();

		bc_terrain_material_smooth_ui_command_state l_state;
		l_state.m_device_compute_state = l_render_system.create_device_compute_state("terrain_material_smooth_cs");;
		l_state.m_device_command_list = l_render_system.get_device().create_command_list();;
		l_state.m_parameter_cbuffer = l_render_system.get_device().create_buffer(l_cb_config, nullptr);;
		l_state.m_parameter_cbuffer->set_debug_name("terrain_material_smooth_cbuffer");

		return core::bc_make_unique< bc_terrain_material_smooth_ui_command_state >(std::move(l_state));
	}

	bool bc_terrain_material_smooth_ui_command::update(terrain_update_context& p_context)
	{
		const auto* l_height_map_component = p_context.m_terrain.get_component<game::bc_height_map_component>();
		auto& l_dx11_height_map = static_cast<const bc_editor_height_map_dx11&>(l_height_map_component->get_height_map());

		bc_terrain_material_smooth_ui_command_parameter_cbuffer l_cbuffer_parameters;
		l_cbuffer_parameters.m_tool_center_x = p_context.m_tool_center_x;
		l_cbuffer_parameters.m_tool_center_z = p_context.m_tool_center_z;
		l_cbuffer_parameters.m_tool_radius = m_radius;
		l_cbuffer_parameters.m_tool_smooth = m_smooth;

		bc_terrain_material_smooth_ui_command_render_task l_render_task
		(
			l_dx11_height_map,
			*static_cast< bc_terrain_material_smooth_ui_command_state* >(p_context.m_state),
			l_cbuffer_parameters
		);
		p_context.m_game_system.get_render_system().add_render_task(l_render_task);

		l_render_task.wait();

		return false;
	}

	bc_terrain_material_smooth_ui_command_render_task::bc_terrain_material_smooth_ui_command_render_task(const bc_editor_height_map_dx11& p_height_map,
		bc_terrain_material_smooth_ui_command_state& p_command_state,
		const bc_terrain_material_smooth_ui_command_parameter_cbuffer& p_shader_parameter)
		: m_height_map(p_height_map),
		  m_command_state(p_command_state),
		  m_shader_parameter(p_shader_parameter)
	{
	}

	void bc_terrain_material_smooth_ui_command_render_task::execute(game::bc_render_system& p_render_system, game::bc_render_thread& p_render_thread)
	{
		const auto l_height_map_width = m_height_map.get_width();
		const auto l_height_map_height = m_height_map.get_height();
		const auto l_texture_map_width = m_height_map.get_texture_map().get_width();
		const auto l_texture_map_height = m_height_map.get_texture_map().get_height();

		// covert coordinates from height map space to texture map space
		m_shader_parameter.m_tool_center_x = (static_cast<bcFLOAT>(m_shader_parameter.m_tool_center_x) / static_cast<bcFLOAT>(l_height_map_width)) * l_texture_map_width;
		m_shader_parameter.m_tool_center_z = (static_cast<bcFLOAT>(m_shader_parameter.m_tool_center_z) / static_cast<bcFLOAT>(l_height_map_height)) * l_texture_map_height;
		m_shader_parameter.m_tool_radius = (static_cast<bcFLOAT>(m_shader_parameter.m_tool_radius) / static_cast<bcFLOAT>(l_height_map_width)) * l_texture_map_width;
			
		const auto l_tool_diameter = m_shader_parameter.m_tool_radius * 2;
		const auto l_thread_group_count = (l_tool_diameter / 32) + 1;

		auto l_temp_texture_config = graphic::bc_graphic_resource_builder()
		                             .as_resource()
		                             .as_texture2d
		                             (
			                             m_height_map.get_texture_map().get_width(),
			                             m_height_map.get_texture_map().get_height(),
			                             false,
			                             1,
			                             m_height_map.get_texture_map().get_format(),
			                             graphic::bc_resource_usage::gpu_rw,
			                             graphic::bc_resource_view_type::shader
		                             )
		                             .as_normal_texture();
		auto l_temp_texture_view_config = graphic::bc_graphic_resource_builder()
		                                  .as_resource_view()
		                                  .as_texture_view(m_height_map.get_texture_map().get_format())
		                                  .as_tex2d_shader_view(0, 1)
		                                  .on_texture2d();

		auto l_texture_map = m_height_map.get_texture_map();
		auto l_temp_texture = p_render_system.get_device().create_texture2d(l_temp_texture_config, nullptr);
		auto l_temp_texture_view = p_render_system.get_device().create_resource_view(l_temp_texture.get(), l_temp_texture_view_config);

		p_render_thread.copy_resource(l_temp_texture.get(), l_texture_map);

		auto l_compute_state = p_render_system.create_compute_state
		(
			m_command_state.m_device_compute_state.get(),
			{},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, l_temp_texture_view.get())
			},
			{
				graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, m_height_map.get_texture_map_unordered_view())
			},
			{
				graphic::bc_constant_buffer_parameter(0, graphic::bc_shader_type::compute, m_height_map.get_parameter_cbuffer()),
				graphic::bc_constant_buffer_parameter(0, graphic::bc_shader_type::compute, m_command_state.m_parameter_cbuffer.get())
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
		p_render_thread.bind_compute_state(*l_compute_state);
		p_render_thread.dispatch(l_thread_group_count, l_thread_group_count, 1);
		p_render_thread.unbind_compute_state(*l_compute_state);

		p_render_thread.finish();
	}
}
