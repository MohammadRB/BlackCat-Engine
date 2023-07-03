// [13/06/2021 MRB]

#include "CorePlatformImp/Utility/bcHardwareInfo.h"
#include "Game/Object/Scene/SceneGraph/bcOctalTreeSceneGraphNode.h"
#include "BoX.Server/Application/bxServerApplication.h"

using namespace black_cat;

int WINAPI WinMain(HINSTANCE p_instance, HINSTANCE p_prev_instance, CHAR* p_cmd_line, int p_cmd_show)
{
	platform::bc_basic_hardware_info l_hardware_info{};
	platform::bc_hardware_info::get_basic_info(l_hardware_info);

	platform::bc_application_parameter l_app_parameters
	(
		p_instance,
		bcL("BattleOfXbots.Server"),
		p_cmd_line
	);
	game::bc_render_application_parameter l_render_app_parameters
	(
		l_app_parameters,
		[]() 
		{
			return nullptr;
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
		16,
		128,
		static_cast<bcSIZE>(core::bc_mem_size::mb) * 32,
		static_cast<bcSIZE>(core::bc_mem_size::mb) * 32,
		static_cast<bcSIZE>(core::bc_mem_size::mb) * 128,
		std::max(4_uz, l_hardware_info.m_processor_count),
		std::max(4_uz, l_hardware_info.m_processor_count)
	);
	game::bc_engine_application_parameter l_engine_app_parameters
	(
		l_engine_component_parameters,
		std::move(l_render_app_parameters)
	);
	
	box::bx_server_application l_app;
	l_app.initialize(l_engine_app_parameters);
	l_app.set_fps(60);

	const auto l_return_code = l_app.run();

	l_app.destroy();

	return l_return_code;
}