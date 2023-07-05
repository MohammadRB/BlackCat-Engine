// [19/11/2020 MRB]

#define MAX_FLOAT 3.402823466e+38F

struct emitter
{
	float3 m_prev_position;
	float3 m_position;
	float3 m_emission_direction;
	float m_energy;
	float m_emission_direction_deviation;
	float m_emission_position_deviation;
	float3 m_emission_position_deviation_plane;
	uint m_sprite_index;
	float3 m_particles_color;
	float m_particles_color_intensity;
	uint m_particles_count;
	float m_particles_lifetime;
	float m_particles_force;
	float m_particles_mass;
	float m_particles_start_size;
	float m_particles_end_size;
	float m_particles_rotation;
	uint m_particles_velocity_curve_index;
	float m_particles_velocity_curve_duration;
	uint m_particles_size_curve_index;
	uint m_particles_fade_curve_index;
};

struct particle
{
	float3 m_position;
	float3 m_direction;
	float3 m_color;
	float m_color_intensity;
	float m_lifetime;
	float m_age;
	float m_force;
	float m_mass;
	float m_start_size;
	float m_end_size;
	float m_size;
	float m_fade;
	float m_rotation;
	uint m_sprite_index;
	uint m_velocity_curve_index;
	float m_velocity_curve_duration;
	uint m_size_curve_index;
	uint m_fade_curve_index;
};

struct alive_particle
{
	uint m_index;
	float m_distance;
};