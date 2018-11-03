// [03/03/2017 MRB]

#include "Editor/EditorPCH.h"

#include "GraphicImp/Resource/bcResourceConfig.h"
#include "Game/System/Physics/bcPxWrap.h"
#include "Game/Object/Scene/Component/bcRigidStaticComponent.h"
#include "Game/Object/Scene/Component/bcHeightMapComponent.h"
#include "Editor/Application/bcEditorHeightMapLoaderDx11.h"
#include "Editor/UICommand/bcUITerrainSmoothCommand.h"

namespace black_cat
{
	namespace editor
	{
		// == bc_ui_terrain_smooth_command =============================================================================================

		bc_ui_terrain_smooth_command::bc_ui_terrain_smooth_command(bcUINT16 p_screen_width,
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

		bc_ui_terrain_smooth_command::bc_ui_terrain_smooth_command(const bc_ui_terrain_smooth_command&) = default;

		bc_ui_terrain_smooth_command::~bc_ui_terrain_smooth_command() = default;

		bc_ui_terrain_smooth_command& bc_ui_terrain_smooth_command::operator=(const bc_ui_terrain_smooth_command&) = default;

		core::bc_string bc_ui_terrain_smooth_command::title() const
		{
			return "Terrain Smooth";
		}

		bc_iui_command::state_ptr bc_ui_terrain_smooth_command::create_state(state_context& p_context) const
		{
			auto& l_render_system = p_context.m_game_system.get_render_system();

			auto l_cb_config = graphic::bc_graphic_resource_configure().as_resource()
				.as_buffer
				(
					1,
					sizeof(bc_ui_terrain_smooth_command_parameter_cbuffer),
					graphic::bc_resource_usage::gpu_rw,
					graphic::bc_resource_view_type::none
				)
				.as_constant_buffer();

			bc_ui_terrain_smooth_commnad_state l_state;
			l_state.m_device_compute_state = l_render_system.create_device_compute_state("terrain_smooth_cs");
			l_state.m_parameter_cbuffer = l_render_system.get_device().create_buffer(l_cb_config, nullptr);
			l_state.m_device_command_list = l_render_system.get_device().create_command_list();

			return core::bc_make_unique< bc_ui_terrain_smooth_commnad_state >(std::move(l_state));
		}

		bool bc_ui_terrain_smooth_command::update(terrain_update_context& p_context)
		{
			auto* l_rigid_component = p_context.m_terrain.get_component< game::bc_rigid_static_component >();
			auto* l_height_map_component = p_context.m_terrain.get_component< game::bc_height_map_component >();
			auto* l_dx11_height_map = static_cast< const bc_editor_height_map_dx11* >(l_height_map_component->get_height_map());
			auto l_px_height_map = l_dx11_height_map->get_px_height_field();

			bc_ui_terrain_smooth_command_parameter_cbuffer l_cbuffer_parameters;
			l_cbuffer_parameters.m_tool_center_x = p_context.m_tool_center_x;
			l_cbuffer_parameters.m_tool_center_z = p_context.m_tool_center_z;
			l_cbuffer_parameters.m_tool_smooth = m_smooth;
			l_cbuffer_parameters.m_tool_radius = m_radius;

			bc_ui_terrain_smooth_command_render_task l_render_task
			(
				*l_dx11_height_map,
				*static_cast< bc_ui_terrain_smooth_commnad_state* >(p_context.m_state),
				l_cbuffer_parameters
			);
			p_context.m_game_system.get_render_system().add_render_task(l_render_task);

			bcINT32 l_diameter = l_cbuffer_parameters.m_tool_radius * 2;
			bcUINT32 l_sample_count = l_diameter * l_diameter;
			core::bc_unique_ptr< bcINT16 > l_sample_buffer(static_cast< bcINT16* >(bcAlloc(l_sample_count * sizeof(bcINT16), core::bc_alloc_type::frame)));
			bcINT16* l_samples = l_sample_buffer.get();

			core::bc_vector2f l_tool_center(l_cbuffer_parameters.m_tool_center_x, l_cbuffer_parameters.m_tool_center_z);
			core::bc_vector2f l_circle_coord;
			core::bc_vector2f l_global_coord;
			for (bcINT32 l_z = 0; l_z < l_diameter; ++l_z)
			{
				for (bcINT32 l_x = 0; l_x < l_diameter; ++l_x)
				{
					bcUINT32 l_sample_index = l_z * l_diameter + l_x;
					l_circle_coord.x = l_x - m_radius;
					l_circle_coord.y = l_z - m_radius;
					l_global_coord = l_tool_center + l_circle_coord;
					bcFLOAT l_center_distance = (l_tool_center - l_global_coord).magnitude();

					if (l_center_distance > m_radius)
					{
						l_samples[l_sample_index] = l_px_height_map.get_height(l_global_coord.x, l_global_coord.y);
						continue;
					}

					bcFLOAT l_heights = 0;

					bcUINT32 l_min_x = std::max(0U, static_cast< bcUINT32 >(l_global_coord.x) - s_smooth_radius);
					bcUINT32 l_min_z = std::max(0U, static_cast< bcUINT32 >(l_global_coord.y) - s_smooth_radius);
					bcUINT32 l_max_x = std::min(static_cast< bcUINT32 >(l_dx11_height_map->get_width()), static_cast< bcUINT32 >(l_global_coord.x) + s_smooth_radius);
					bcUINT32 l_max_z = std::min(static_cast< bcUINT32 >(l_dx11_height_map->get_height()), static_cast< bcUINT32 >(l_global_coord.y) + s_smooth_radius);
					bcUINT32 l_num_sample = 0;

					for (bcUINT32 l_x1 = l_min_x; l_x1 <= l_max_x; ++l_x1)
					{
						for (bcUINT32 l_z1 = l_min_z; l_z1 <= l_max_z; ++l_z1)
						{
							l_heights += l_px_height_map.get_height(l_x1, l_z1) * l_dx11_height_map->get_y_multiplier();

							++l_num_sample;
						}
					}

					l_heights /= l_num_sample;
					bcFLOAT l_source_height = l_px_height_map.get_height(l_global_coord.x, l_global_coord.y) * l_dx11_height_map->get_y_multiplier();
					bcFLOAT l_smooth_ratio = (s_smooth_max - l_cbuffer_parameters.m_tool_smooth * 1.0f) / s_smooth_max;
					bcFLOAT l_final_height = (l_heights * (1 - l_smooth_ratio)) + (l_source_height * l_smooth_ratio);

					l_samples[l_sample_index] = l_final_height / l_dx11_height_map->get_y_multiplier();
				}
			}

			physics::bc_bounded_strided_typed_data< bcINT16 > l_px_samples(l_samples, sizeof(bcINT16), l_sample_count);
			physics::bc_bounded_strided_typed_data< physics::bc_material_index > l_px_sample_materials;
			physics::bc_height_field_desc l_px_height_map_desc(l_diameter, l_diameter, l_px_samples, l_px_sample_materials);

			physics::bc_shape l_terrain_shape;
			l_rigid_component->get_static_body().get_shapes(&l_terrain_shape, 1);

			l_px_height_map.modify_samples
			(
				l_cbuffer_parameters.m_tool_center_z - m_radius,
				l_cbuffer_parameters.m_tool_center_x - m_radius,
				l_px_height_map_desc,
				&l_terrain_shape,
				1
			);

			l_render_task.wait();

			return false;
		}

		// == bc_ui_terrain_smooth_command_render_task ==================================================================================

		bc_ui_terrain_smooth_command_render_task::bc_ui_terrain_smooth_command_render_task(const bc_editor_height_map_dx11& p_height_map,
			bc_ui_terrain_smooth_commnad_state& p_command_state,
			const bc_ui_terrain_smooth_command_parameter_cbuffer& p_shader_parameter)
			: m_height_map(p_height_map),
			m_command_state(p_command_state),
			m_shader_parameter(p_shader_parameter)
		{
		}

		void bc_ui_terrain_smooth_command_render_task::execute(game::bc_render_system& p_render_system, game::bc_render_thread& p_render_thread)
		{
			const auto l_tool_diameter = m_shader_parameter.m_tool_radius * 2;
			const auto l_thread_group_count = (l_tool_diameter / 32) + 1;

			auto l_compute_state = p_render_system.create_compute_state
			(
				m_command_state.m_device_compute_state.get(),
				l_thread_group_count,
				l_thread_group_count,
				1,
				{},
				{},
				{ graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::compute, m_height_map.get_height_map_unordered_view()) },
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
			p_render_thread.run_compute_shader(l_compute_state.get());

			p_render_thread.finish();
			m_command_state.m_device_command_list->reset();
		}
	}
}