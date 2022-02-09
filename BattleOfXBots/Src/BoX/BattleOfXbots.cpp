// [06/13/2021 MRB]

#include <memory>
#include "CorePlatformImp/Utility/bcHardwareInfo.h"
#include "Game/Object/Scene/SceneGraph/bcOctalTreeSceneGraphNode.h"
#include "BoX/Application/bxApplication.h"

using namespace black_cat;

int WINAPI WinMain(HINSTANCE p_instance, HINSTANCE p_prev_instance, CHAR* p_cmd_line, int p_cmd_show)
{
	std::unique_ptr<game::bc_render_application_basic_output_window> l_output_window;
	
	core_platform::bc_basic_hardware_info l_hardware_info;
	core_platform::bc_hardware_info::get_basic_info(l_hardware_info);

	platform::bc_application_parameter l_app_parameters
	(
		p_instance,
		bcL("BattleOfXbots"),
		p_cmd_line
	);
	game::bc_render_application_parameter l_render_app_parameters
	(
		l_app_parameters,
		[&]()
		{
			l_output_window = std::make_unique<game::bc_render_application_basic_output_window>
			(
				game::bc_render_application_basic_output_window
				(
					platform::bc_basic_window(platform::bc_basic_window_parameter(p_instance, bcL("BattleOfXbots"), 1024, 700))
				)
			);
			return l_output_window.get();
		},
		[](game::bc_scene_graph_node_factory_parameter p_parameter)
		{
			return core::bc_make_unique<game::bc_octal_tree_graph_node>
			(
				physics::bc_bound_box(p_parameter.m_center, p_parameter.m_half_extends),
				10,
				128
			);
		}
	);
	game::bc_engine_component_parameter l_engine_component_parameters
	(
		32,
		32,
		BC_MEMORY_MIN_ALIGN,
		128,
		static_cast<bcUINT32>(core::bc_mem_size::mb) * 32,
		static_cast<bcUINT32>(core::bc_mem_size::mb) * 32,
		static_cast<bcUINT32>(core::bc_mem_size::mb) * 128,
		std::max(4U, l_hardware_info.m_processor_count),
		std::max(4U, l_hardware_info.m_processor_count)
	);
	game::bc_engine_application_parameter l_engine_app_parameters
	(
		l_engine_component_parameters,
		std::move(l_render_app_parameters)
	);

	box::bx_application l_app;
	l_app.initialize(l_engine_app_parameters);
	l_app.set_fps(60);

	const auto l_return_code = l_app.run();
	
	l_app.destroy();

	return l_return_code;
}