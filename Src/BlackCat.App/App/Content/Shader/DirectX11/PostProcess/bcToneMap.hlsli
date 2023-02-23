// [02/18/2023 MRB]
// HDR Rendering based on MJP Sample

static const float g_middle_grey = 0.15f;
static const float g_white_level = 16.0f;
static const float g_luminance_saturation = 1.0f;
static const float3 g_lum_convert = float3(0.299f, 0.587f, 0.114f);

static const float g_shoulder_strength = 0.22f;
static const float g_linear_strength = 0.3f;
static const float g_linear_angle = 0.1f;
static const float g_toe_strength = 0.2f;
static const float g_toe_numerator = 0.01f;
static const float g_toe_denominator = 0.3f;
static const float g_linear_white = 11.2f;

float calculate_luminance(float3 p_color)
{
	return max(dot(p_color, g_lum_convert), 0.0001f);
}

float3 reinhard_tone_map_1(float3 p_color, float p_avg_luminance)
{
	const float l_pixel_luminance = calculate_luminance(p_color);
	const float l_scaled_luminance = (l_pixel_luminance * g_middle_grey) / p_avg_luminance;
	const float l_compressed_luminance = (l_scaled_luminance * (1 + (l_scaled_luminance / (g_white_level * g_white_level)))) / (1 + l_scaled_luminance);

	return l_compressed_luminance * p_color;
}

float3 reinhard_tone_map(float3 p_color, float p_avg_luminance)
{
	const float l_pixel_luminance = calculate_luminance(p_color);
	const float l_tone_mapped_luminance = l_pixel_luminance * (1.0f + l_pixel_luminance / (g_white_level * g_white_level)) / (1.0f + l_pixel_luminance);
	return l_tone_mapped_luminance * pow(p_color / l_pixel_luminance, g_luminance_saturation);
}

// Function used by the Uncharted 2 tone mapping curve
float3 u2_func(float3 x)
{
    float a = g_shoulder_strength;
    float b = g_linear_strength;
    float c = g_linear_angle;
    float d = g_toe_strength;
    float e = g_toe_numerator;
    float f = g_toe_denominator;
    return ((x * (a * x + c * b) + d * e) / (x * (a * x + b) + d * f)) - e / f;
}

// Applies the Uncharted 2 filmic tone mapping curve
float3 filmic_u2_tone_map(float3 color)
{
	const float3 l_numerator = u2_func(color);
	const float3 l_denominator = u2_func(g_linear_white);

    return l_numerator / l_denominator;
}

// Determines the color based on exposure settings
float3 calc_exposed_color(float3 p_color, float p_avg_luminance, float p_threshold, out float p_exposure)
{
    p_exposure = 0;

    // Use geometric mean
    p_avg_luminance = max(p_avg_luminance, 0.001f);

    //const float l_key_value = 1.03f - (2.0f / (2 + log10(p_avg_luminance + 1)));
    const float l_key_value = 0.90;
    const float l_linear_exposure = (l_key_value / p_avg_luminance);
    p_exposure = log2(max(l_linear_exposure, 0.0001f));

    p_exposure -= p_threshold;
    return exp2(p_exposure) * p_color;
}

// Applies exposure and tone mapping to the specific color, and applies the threshold to the exposure value. 
float3 tone_map(float3 p_color, float p_avg_luminance, float p_threshold, out float p_exposure)
{
    p_exposure = 0;
    //float3 l_color = calc_exposed_color(p_color, p_avg_luminance, p_threshold, p_exposure);
    //l_color = reinhard_tone_map(l_color, p_avg_luminance);
    //l_color = filmic_u2_tone_map(l_color);
    float3 l_color = reinhard_tone_map_1(p_color, p_avg_luminance);

    return l_color;
}