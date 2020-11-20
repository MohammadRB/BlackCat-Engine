// [11/19/2020 MRB]

struct emitter
{
	float3 m_position;
	float3 m_emission_direction;
	uint m_emission_deviation;
	uint m_texture_index;
	uint m_sprite_index;
	uint m_particles_count;
	float m_particles_lifetime;
	float m_particles_force;
	float m_particles_mass;
};

struct particle
{
	float3 m_position;
	float m_lifetime;
	float3 m_direction;
	float m_age;
	float m_force;
	float m_mass;
	uint m_texture_index;
	uint m_sprite_index;
};

struct alive_particle
{
	uint m_index;
	float m_distance;
};