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
    float2 l_coordinate = float2(p_texcoord.x * (1.0 / p_screen_width), p_texcoord.y * (1.0 / p_screen_height));
    l_coordinate.x = l_coordinate.x * 2.0f - 1.0f;
    l_coordinate.y = -(l_coordinate.y * 2.0f - 1.0f);

    return l_coordinate;
}

float2 bc_clip_space_to_texcoord(float4 p_input)
{
	float2 l_uv = float2(0.5f, -0.5f) * (p_input.xy / p_input.w) + 0.5f;
	return l_uv;
}

float bc_convert_to_linear_depth(float p_depth, float p_near_plane, float p_far_plane)
{
    float l_depth = (p_near_plane * p_far_plane) / (p_far_plane - p_depth * (p_far_plane - p_near_plane)) / (p_far_plane - p_near_plane);
    return l_depth;
}

float3 bc_reconstruct_world_space(float2 p_ss_texcoord, float p_depth, float4x4 p_view_proj_inv)
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