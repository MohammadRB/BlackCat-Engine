// [05/31/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Container/bcString.h"

namespace black_cat
{
	namespace core
	{
		// Define expression parameter resolver 
#define BC_PARAMETER_RESOLVER(p_name, p_value)	constexpr const bcCHAR* g_param_rsv_##p_name = p_value
		// Define expression parameter
#define BC_PARAMETER(p_name, p_value)			constexpr const bcCHAR* g_param_##p_name = p_value
		// Define service
#define BC_SERVICE(p_name, p_value)				constexpr const bcCHAR* g_srv_##p_name = p_value
		// Define content
#define BC_CONTENT(p_name, p_value)				constexpr const bcCHAR* g_cnt_##p_name = p_value
		// Define component
#define BC_COMPONENT(p_name, p_value)			constexpr const bcCHAR* g_cmp_##p_name = p_value \
												//constexpr auto g_cmp_hash_##p_name = bc_compile_time_string_hash(p_value)

#define BC_EVENT(p_name, p_value)				constexpr const bcCHAR* g_evt_##p_name = p_value
		
		BC_PARAMETER(device, "device");
		BC_PARAMETER(shader_function, "function");
		BC_PARAMETER(mesh, "mesh");
		BC_PARAMETER(mesh_part, "mesh_part");
		BC_PARAMETER(child_actor, "childs");

		BC_PARAMETER_RESOLVER(device, "get_device");
		BC_PARAMETER_RESOLVER(mesh, "get_mesh");

		BC_SERVICE(thread_manager, "thread_manager");
		BC_SERVICE(content_manager, "content_manager");
		BC_SERVICE(content_stream_manager, "content_stream_manager");
		BC_SERVICE(game_system, "game_system");
		BC_SERVICE(expression_parameter_manager, "expression_parameter_manager");
		BC_SERVICE(event_manager, "event_manager");
		BC_SERVICE(logger, "logger");
		BC_SERVICE(actor_component_manager, "actor_component_manager");
		BC_SERVICE(entity_manager, "entity_manager");

		BC_CONTENT(mesh, "mesh_content");
		BC_CONTENT(vertex_shader, "vertex_shader_content");
		BC_CONTENT(domain_shader, "domain_shader_content");
		BC_CONTENT(hull_shader, "hull_shader_content");
		BC_CONTENT(geometry_shader, "geometry_shader_content");
		BC_CONTENT(pixel_shader, "pixel_shader_content");
		BC_CONTENT(texture2d, "texture2d_content");

		BC_COMPONENT(hierarchy, "hierarchy_component");
		BC_COMPONENT(mesh, "mesh_component");
		BC_COMPONENT(render, "render_component");

		BC_EVENT(error, "error_event");
		BC_EVENT(warning, "warning_event");
		BC_EVENT(app_exit, "exit_event");
		BC_EVENT(window_resize, "window_resize_event");
		BC_EVENT(app_active, "app_active_event");
		BC_EVENT(key, "key_event");
		BC_EVENT(point, "point_event");
		BC_EVENT(device_reset, "device_reset_event");
	}
}