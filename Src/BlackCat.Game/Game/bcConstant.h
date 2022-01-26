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
		BC_PARAMETER(position, "position");
		BC_PARAMETER(direction, "direction");
		BC_PARAMETER(rotation, "rotation");
		BC_PARAMETER(entity_name, "entity_name");
		BC_PARAMETER(rigid_cmass, "cmass");
		BC_PARAMETER(rigid_mass, "mass");
		BC_PARAMETER(rigid_force_amount, "force_amount");
		BC_PARAMETER(rigid_force_radius, "force_radius");
		BC_PARAMETER(animations, "animations");
		BC_PARAMETER(animation_additive, "additive");
		BC_PARAMETER(child_actor, "childs");
		BC_PARAMETER(heightmap, "heightmap");
		BC_PARAMETER(texture_config, "config");
		BC_PARAMETER(emitter_name, "emitter_name");
		BC_PARAMETER(decal_parameters, "decals");
		BC_PARAMETER(decal_name, "decal_name");
		BC_PARAMETER(weapon_class, "class");
		BC_PARAMETER(weapon_up_ls, "up_ls");
		BC_PARAMETER(weapon_forward_ls, "forward_ls");
		BC_PARAMETER(weapon_main_hand_offset_ls, "main_hand_offset_ls");
		BC_PARAMETER(weapon_second_hand_offset_ls, "second_hand_offset_ls");
		BC_PARAMETER(weapon_fire_offset_ls, "fire_offset_ls");
		BC_PARAMETER(weapon_fire_particle, "fire_particle");
		BC_PARAMETER(weapon_fire_light_color, "fire_light_color");
		BC_PARAMETER(weapon_fire_light_radius, "fire_light_radius");
		BC_PARAMETER(weapon_fire_light_intensity, "fire_light_intensity");
		BC_PARAMETER(weapon_fire_light_flare_surface, "fire_light_flare_surface");
		BC_PARAMETER(weapon_fire_light_flare_intensity, "fire_light_flare_intensity");
		BC_PARAMETER(weapon_fire_light_flare_mask_material, "fire_light_flare_mask_material");
		BC_PARAMETER(weapon_rate_of_fire_seconds, "rate_of_fire");
		BC_PARAMETER(weapon_bullet_speed, "bullet_speed");
		BC_PARAMETER(weapon_bullet_mass, "bullet_mass");
		BC_PARAMETER(light_type, "type");
		BC_PARAMETER(light_position, "position");
		BC_PARAMETER(light_direction, "direction");
		BC_PARAMETER(light_color, "color");
		BC_PARAMETER(light_ambient_color, "ambient_color");
		BC_PARAMETER(light_radius, "radius");
		BC_PARAMETER(light_length, "length");
		BC_PARAMETER(light_angle, "angle");
		BC_PARAMETER(light_particle_cast, "particle_cast");
		BC_PARAMETER(light_particle_intensity, "particle_intensity");
		BC_PARAMETER(light_flare_surface, "flare_surface");
		BC_PARAMETER(light_flare_intensity, "flare_intensity");
		BC_PARAMETER(light_flare_mask_material, "flare_mask_material");
		BC_PARAMETER(light_flare_mask_u0, "flare_mask_u0");
		BC_PARAMETER(light_flare_mask_v0, "flare_mask_v0");
		BC_PARAMETER(light_flare_mask_u1, "flare_mask_u1");
		BC_PARAMETER(light_flare_mask_v1, "flare_mask_v1");
		BC_PARAMETER(network_data_dir, "data_dir");
		BC_PARAMETER(network_entity_name, "entity_name");
		BC_PARAMETER(ragdoll_joints, "joints");
		BC_PARAMETER(icon_name, "name");
		BC_PARAMETER(icon_size, "size");
		BC_PARAMETER(icon_type, "type");
		BC_PARAMETER(player_id, "player_id");
	}
}