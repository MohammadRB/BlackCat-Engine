float3 bc_to_encoded_normal(float3 p_normal)
{
    return (p_normal + 1) / 2.0;
}

float3 bc_to_decoded_normal(float3 p_normal)
{
    return (p_normal - 0.5) * 2;
}

float bc_convert_to_linear_depth(float p_depth, float p_near_plane, float p_far_plane)
{
    float l_depth = (p_near_plane * p_far_plane) / (p_far_plane - p_depth * (p_far_plane - p_near_plane)) / (p_far_plane - p_near_plane);
    return l_depth;
}

float4 bc_reconstruct_world_space(float2 p_ss_texcoord, float p_depth, float4x4 p_view_proj_inv)
{
    // compute screen-space position
    float4 l_position = 1.0f;
    l_position.x = p_ss_texcoord.x * 2.0f - 1.0f;
    l_position.y = -(p_ss_texcoord.y * 2.0f - 1.0f);
    l_position.z = p_depth;
    l_position.w = 1.0f;

    // transform to world space
    l_position = mul(l_position, p_view_proj_inv);
    l_position /= l_position.w;

    return l_position;
}