static const float2 g_ss_quad[] =
{
	float2(-1, 1), float2(1, 1), float2(-1, -1), float2(1, 1), float2(1, -1), float2(-1, -1)
};
static const float2 g_ss_quad_tex[] =
{
	float2(0, 0), float2(1, 0), float2(0, 1), float2(1, 0), float2(1, 1), float2(0, 1)
};

float3 bc_to_encoded_normal(float3 p_normal)
{
	return (p_normal + 1) / 2.0;
}

float3 bc_to_decoded_normal(float3 p_normal)
{
	return (p_normal - 0.5) * 2;
}

float2 bc_to_screen_space_texcoord(int2 p_texcoord, uint p_screen_width, uint p_screen_height)
{
	float2 l_coordinate = float2(p_texcoord.x * rcp((float) p_screen_width), p_texcoord.y * rcp((float) p_screen_height));
	l_coordinate.x = l_coordinate.x * 2.0f - 1.0f;
	l_coordinate.y = -(l_coordinate.y * 2.0f - 1.0f);

	return l_coordinate;
}

float2 bc_to_screen_space_texcoord(float2 p_texcoord)
{
	float2 l_texcoord = p_texcoord;
	l_texcoord.x = l_texcoord.x * 2.0f - 1.0f;
	l_texcoord.y = -(l_texcoord.y * 2.0f - 1.0f);

	return l_texcoord;
}

float2 bc_clip_space_to_texcoord(float4 p_input)
{
	float2 l_uv = float2(0.5f, -0.5f) * (p_input.xy / p_input.w) + 0.5f;
	return l_uv;
}

float2 bc_to_sprite_texcoord(float2 p_texcoord, uint p_sprite_index, uint p_horizontal_count, uint p_vertical_count)
{
	const uint2 l_sprite_index = uint2(p_sprite_index % p_vertical_count, p_sprite_index / p_horizontal_count);
	const float2 l_start_texcoord = l_sprite_index * float2(1.0f / p_horizontal_count, 1.0f / p_vertical_count);
	const float2 l_local_texcoord = p_texcoord * float2(1.0f / p_horizontal_count, 1.0f / p_vertical_count);

	return l_start_texcoord + l_local_texcoord;
}

float bc_convert_to_linear_depth(float p_depth, float p_near_plane, float p_far_plane)
{
	const float l_depth = (p_near_plane * p_far_plane) / (p_far_plane - p_depth * (p_far_plane - p_near_plane)) / (p_far_plane - p_near_plane);
	return l_depth;
}

float bc_convert_to_scaled_linear_depth(float p_depth, float p_near_plane, float p_far_plane)
{
	const float l_depth = (p_near_plane * p_far_plane) / (p_far_plane - p_depth * (p_far_plane - p_near_plane));
	return l_depth;
}

float3 bc_reconstruct_world_position(float2 p_ss_texcoord, float p_depth, float4x4 p_view_proj_inv)
{
    // compute screen-space position
	float4 l_position = 1.0f;
	l_position.x = p_ss_texcoord.x;
	l_position.y = p_ss_texcoord.y;
	l_position.z = p_depth;
	l_position.w = 1.0f;

    // transform to world space
	l_position = mul(l_position, p_view_proj_inv);
	l_position /= l_position.w;

	return l_position.xyz;
}

float3 bc_reconstruct_world_position(float p_depth, float3 p_world_frustum_vector, float3 p_camera_position, float p_near_plane, float p_far_plane)
{
	const float l_scaled_linear_depth = bc_convert_to_scaled_linear_depth(p_depth, p_near_plane, p_far_plane);
	const float3 l_world_pos = p_camera_position + p_world_frustum_vector * l_scaled_linear_depth;

	return l_world_pos;
}

float3 bc_reconstruct_world_normal(float3 p_world_pos)
{
	const float3 l_ddx_pixel_world = ddx(p_world_pos);
	const float3 l_ddy_pixel_world = ddy(p_world_pos);
	const float3 l_pixel_normal = normalize(cross(l_ddx_pixel_world, l_ddy_pixel_world));
	return l_pixel_normal;
}

float bc_noise(float x, float y)
{
	const float pi = 3.14f;
	return cos(x * pi) * cos(x * pi) * cos(x * 3 * pi) * cos(x * 5 * pi) * 0.5 + sin(x * 25 * pi) * y;
}

uint wang_hash(uint seed)
{
	seed = (seed ^ 61) ^ (seed >> 16);
	seed *= 9;
	seed = seed ^ (seed >> 4);
	seed *= 0x27d4eb2d;
	seed = seed ^ (seed >> 15);
	return seed;
}

float bc_random(uint p_seed)
{
	uint l_rnd = wang_hash(p_seed);
	return l_rnd * (1.0f / uint(4294967295));
}

float bc_random(uint p_seed, float p_low, float p_high)
{
	float l_rnd = bc_random(p_seed);
	return p_low * (1.f - l_rnd) + p_high * l_rnd;
}