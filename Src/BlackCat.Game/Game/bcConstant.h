// [06/28/2019 MRB]

#pragma once

#include "Core/bcConstant.h"

namespace black_cat
{
	namespace constant
	{
		BC_PARAMETER(shader_function, "function");
		BC_PARAMETER(shader_macro, "macros");
		BC_PARAMETER(mesh, "mesh");
		BC_PARAMETER(sub_mesh, "sub_mesh");
		BC_PARAMETER(mesh_materials, "materials");
		BC_PARAMETER(mesh_skinned, "skinned");
		BC_PARAMETER(mesh_auto_scale, "auto_scale");
		BC_PARAMETER(mesh_lod_scale, "lod_scale");
		BC_PARAMETER(mesh_collider, "collider");
		BC_PARAMETER(mesh_collider_materials, "collider_materials");
		BC_PARAMETER(mesh_is_lod, "is_lod");
		BC_PARAMETER(high_detail_query_shape, "high_detail_query_shape");
		BC_PARAMETER(rigid_cmass, "cmass");
		BC_PARAMETER(rigid_mass, "mass");
		BC_PARAMETER(animations, "animations");
		BC_PARAMETER(child_actor, "childs");
		BC_PARAMETER(heightmap, "heightmap");
		BC_PARAMETER(texture_config, "config");
		BC_PARAMETER(emitter_name, "emitter_name");
		BC_PARAMETER(decal_name, "decal_name");
		BC_PARAMETER(weapon_class, "class");
		BC_PARAMETER(weapon_local_forward, "local_forward");
		BC_PARAMETER(weapon_main_hand_node, "main_hand_node");
		BC_PARAMETER(weapon_second_hand_node, "second_hand_node");
		BC_PARAMETER(weapon_main_hand_offset, "main_hand_offset");
		BC_PARAMETER(weapon_second_hand_offset, "second_hand_offset");
	}
}