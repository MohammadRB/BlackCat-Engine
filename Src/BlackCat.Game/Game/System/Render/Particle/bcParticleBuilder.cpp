// [14/11/2020 MRB]

#include "Game/GamePCH.h"

#include "Game/System/Render/Particle/bcParticleBuilder.h"

namespace black_cat
{
	namespace game
	{
		const bc_particle_curve bc_particle_builder::s_curve_constant =		{ core::bc_vector2f{0.0f, 1.0f}, core::bc_vector2f{0.50f, 1.00f}, core::bc_vector2f{1.0f, 1.0f} };
		const bc_particle_curve bc_particle_builder::s_curve_slow_step5 =	{ core::bc_vector2f{0.0f, 0.0f}, core::bc_vector2f{0.88f, 0.05f}, core::bc_vector2f{1.0f, 1.0f} };
		const bc_particle_curve bc_particle_builder::s_curve_slow_step4 =	{ core::bc_vector2f{0.0f, 0.0f}, core::bc_vector2f{0.76f, 0.10f}, core::bc_vector2f{1.0f, 1.0f} };
		const bc_particle_curve bc_particle_builder::s_curve_slow_step3 =	{ core::bc_vector2f{0.0f, 0.0f}, core::bc_vector2f{0.64f, 0.15f}, core::bc_vector2f{1.0f, 1.0f} };
		const bc_particle_curve bc_particle_builder::s_curve_slow_step2 =	{ core::bc_vector2f{0.0f, 0.0f}, core::bc_vector2f{0.52f, 0.20f}, core::bc_vector2f{1.0f, 1.0f} };
		const bc_particle_curve bc_particle_builder::s_curve_slow_step1 =	{ core::bc_vector2f{0.0f, 0.0f}, core::bc_vector2f{0.40f, 0.25f}, core::bc_vector2f{1.0f, 1.0f} };
		const bc_particle_curve bc_particle_builder::s_curve_linear =		{ core::bc_vector2f{0.0f, 1.0f}, core::bc_vector2f{0.50f, 0.50f}, core::bc_vector2f{1.0f, 0.0f} };
		const bc_particle_curve bc_particle_builder::s_curve_fast_step1 =	{ core::bc_vector2f{0.0f, 1.0f}, core::bc_vector2f{0.60f, 0.25f}, core::bc_vector2f{1.0f, 0.0f} };
		const bc_particle_curve bc_particle_builder::s_curve_fast_step2 =	{ core::bc_vector2f{0.0f, 1.0f}, core::bc_vector2f{0.48f, 0.20f}, core::bc_vector2f{1.0f, 0.0f} };
		const bc_particle_curve bc_particle_builder::s_curve_fast_step3 =	{ core::bc_vector2f{0.0f, 1.0f}, core::bc_vector2f{0.36f, 0.15f}, core::bc_vector2f{1.0f, 0.0f} };
		const bc_particle_curve bc_particle_builder::s_curve_fast_step4 =	{ core::bc_vector2f{0.0f, 1.0f}, core::bc_vector2f{0.24f, 0.10f}, core::bc_vector2f{1.0f, 0.0f} };
		const bc_particle_curve bc_particle_builder::s_curve_fast_step5 =	{ core::bc_vector2f{0.0f, 1.0f}, core::bc_vector2f{0.12f, 0.05f}, core::bc_vector2f{1.0f, 0.0f} };
		const core::bc_array<const bc_particle_curve*, 12> bc_particle_builder::s_curves = 
		{
			&s_curve_constant,
			&s_curve_slow_step5, &s_curve_slow_step4, &s_curve_slow_step3, &s_curve_slow_step2, &s_curve_slow_step1,
			&s_curve_linear,
			&s_curve_fast_step1 ,&s_curve_fast_step2, &s_curve_fast_step3, &s_curve_fast_step4, &s_curve_fast_step5
		};

		bc_particle_builder1::bc_particle_builder1(bc_particle_builder& p_builder, bc_particle_emitter_trait& p_emitter) noexcept
			: m_builder(p_builder),
			m_emitter(p_emitter)
		{
		}

		bc_particle_builder1& bc_particle_builder1::with_texture(bcUINT16 p_sprite_index) noexcept
		{
			m_emitter.m_sprite_index = p_sprite_index;
			return *this;
		}

		bc_particle_builder1& bc_particle_builder1::with_velocity_curve(const bc_particle_curve& p_curve) noexcept
		{
			m_emitter.m_velocity_curve_index = bc_particle_builder::_find_curve_index(p_curve);
			m_emitter.m_velocity_curve_duration = m_emitter.m_lifetime_seconds;
			return *this;
		}

		bc_particle_builder1& bc_particle_builder1::with_direction_deviation(bcUINT16 p_angle) noexcept
		{
			m_emitter.m_direction_deviation = p_angle;
			return *this;
		}

		bc_particle_builder1& bc_particle_builder1::with_emission_position_deviation(bcFLOAT p_radius, const core::bc_vector3f& p_deviation_plane) noexcept
		{
			m_emitter.m_emission_position_deviation = p_radius;
			m_emitter.m_emission_position_deviation_plane = p_deviation_plane;
			return *this;
		}

		bc_particle_builder1& bc_particle_builder1::with_emission_direction_deviation(bcUINT16 p_angle) noexcept
		{
			m_emitter.m_emission_direction_deviation = p_angle;
			return *this;
		}
		
		bc_particle_builder1& bc_particle_builder1::with_particles_color(const core::bc_vector3f& p_color, bcFLOAT p_intensity)
		{
			m_emitter.m_particles_color = p_color;
			m_emitter.m_particles_color_intensity = p_intensity;
			return *this;
		}

		bc_particle_builder1& bc_particle_builder1::with_particles_reverse_direction() noexcept
		{
			m_emitter.m_particles_velocity_reverse_direction = -1;
			return *this;
		}

		bc_particle_builder1& bc_particle_builder1::with_particle_size(bcFLOAT p_start_size, bcFLOAT p_end_size) noexcept
		{
			m_emitter.m_particles_start_size = p_start_size;
			m_emitter.m_particles_end_size = p_end_size;
			return *this;
		}

		bc_particle_builder1& bc_particle_builder1::with_particles_rotation(bcFLOAT p_degree_per_second) noexcept
		{
			m_emitter.m_particles_rotation_per_second = p_degree_per_second;
			return *this;
		}

		bc_particle_builder1& bc_particle_builder1::with_particle_velocity_curve(const bc_particle_curve& p_curve, bcFLOAT p_curve_duration_second) noexcept
		{
			m_emitter.m_particles_velocity_curve_index = bc_particle_builder::_find_curve_index(p_curve);
			m_emitter.m_particles_velocity_curve_duration = p_curve_duration_second;
			return *this;
		}

		bc_particle_builder1& bc_particle_builder1::with_particle_size_curve(const bc_particle_curve& p_curve) noexcept
		{
			m_emitter.m_particles_size_curve_index = bc_particle_builder::_find_curve_index(p_curve);
			return *this;
		}

		bc_particle_builder1& bc_particle_builder1::with_particle_fade_curve(const bc_particle_curve& p_curve) noexcept
		{
			m_emitter.m_particles_fade_curve_index = bc_particle_builder::_find_curve_index(p_curve);
			return *this;
		}

		bc_particle_builder& bc_particle_builder1::emit_particles_with_per_second_count(bcUINT16 p_particles_per_second,
			bcFLOAT p_particles_lifetime,
			bcFLOAT p_particles_force,
			bcFLOAT p_particles_mass) noexcept
		{
			m_emitter.m_particles_count_per_second = p_particles_per_second;
			m_emitter.m_particles_lifetime = p_particles_lifetime;
			m_emitter.m_particles_force = p_particles_force;
			m_emitter.m_particles_mass = p_particles_mass;

			return m_builder;
		}

		bc_particle_builder& bc_particle_builder1::emit_particles_with_total_count(bcUINT16 p_particles_total_count,
			bcFLOAT p_particles_lifetime, 
			bcFLOAT p_particles_force, 
			bcFLOAT p_particles_mass) noexcept
		{
			m_emitter.m_particles_total_count = p_particles_total_count;
			m_emitter.m_particles_lifetime = p_particles_lifetime;
			m_emitter.m_particles_force = p_particles_force;
			m_emitter.m_particles_mass = p_particles_mass;

			return m_builder;
		}

		bc_particle_builder::bc_particle_builder() = default;

		bc_particle_builder1 bc_particle_builder::emitter(const core::bc_vector3f& p_position, const core::bc_vector3f& p_direction) noexcept
		{
			return emitter(p_position, p_direction, 0, 0, 0);
		}

		bc_particle_builder1 bc_particle_builder::emitter(const core::bc_vector3f& p_position,
			const core::bc_vector3f& p_direction,
			bcFLOAT p_lifetime_seconds,
			bcFLOAT p_force,
			bcFLOAT p_mass) noexcept
		{
			bc_particle_emitter_trait l_emitter;
			l_emitter.m_position = p_position;
			l_emitter.m_lifetime_seconds = std::max(0.0f, p_lifetime_seconds);
			l_emitter.m_direction = core::bc_vector3f::normalize(p_direction);
			l_emitter.m_force = p_force;
			l_emitter.m_mass = p_mass;
			l_emitter.m_direction_deviation = 0;
			l_emitter.m_emission_position_deviation = 0;
			l_emitter.m_emission_direction_deviation = 45;
			l_emitter.m_emission_position_deviation_plane = core::bc_vector3f(0);
			l_emitter.m_sprite_index = 0;
			l_emitter.m_velocity_curve_index = _find_curve_index(s_curve_constant);
			l_emitter.m_velocity_curve_duration = p_lifetime_seconds;
			l_emitter.m_particles_color = core::bc_vector3f(0.8f);
			l_emitter.m_particles_color_intensity = 1;
			l_emitter.m_particles_count_per_second = 0;
			l_emitter.m_particles_total_count = 0;
			l_emitter.m_particles_lifetime = 0;
			l_emitter.m_particles_force = 0;
			l_emitter.m_particles_mass = 0;
			l_emitter.m_particles_start_size = 0.5;
			l_emitter.m_particles_end_size = 1;
			l_emitter.m_particles_rotation_per_second = 0;
			l_emitter.m_particles_velocity_reverse_direction = 1;
			l_emitter.m_particles_velocity_curve_index = _find_curve_index(s_curve_constant);
			l_emitter.m_particles_velocity_curve_duration = 0;
			l_emitter.m_particles_size_curve_index = _find_curve_index(s_curve_fast_step1);
			l_emitter.m_particles_fade_curve_index = _find_curve_index(s_curve_fast_step1);

			m_emitters.push_back(l_emitter);

			return bc_particle_builder1(*this, *m_emitters.rbegin());
		}

		bc_particle_builder1 bc_particle_builder::duplicate_last(const core::bc_vector3f& p_position, const core::bc_vector3f& p_direction) noexcept
		{
			if(m_emitters.empty())
			{
				return 	duplicate_last(p_position, p_direction, 0, 0, 0);
			}

			return duplicate_last(p_position, p_direction, m_emitters.back().m_lifetime_seconds, m_emitters.back().m_force, m_emitters.back().m_mass);
		}

		bc_particle_builder1 bc_particle_builder::duplicate_last(const core::bc_vector3f& p_position,
			const core::bc_vector3f& p_direction,
			bcFLOAT p_lifetime_seconds,
			bcFLOAT p_force,
			bcFLOAT p_mass) noexcept
		{
			if (m_emitters.empty())
			{
				return emitter(p_position, p_direction, p_lifetime_seconds, p_force, p_mass);
			}

			auto& l_emitter = m_emitters.back();
			l_emitter.m_position = p_position;
			l_emitter.m_direction = core::bc_vector3f::normalize(p_direction);
			l_emitter.m_lifetime_seconds = p_lifetime_seconds;
			l_emitter.m_force = p_force;
			l_emitter.m_mass = p_mass;

			m_emitters.push_back(l_emitter);

			return bc_particle_builder1(*this, *m_emitters.rbegin());
		}

		decltype(bc_particle_builder::s_curves)::size_type bc_particle_builder::_find_curve_index(const bc_particle_curve& p_curve) noexcept
		{
			const auto l_index = std::find(std::begin(s_curves), std::end(s_curves), &p_curve) - std::begin(s_curves);
			BC_ASSERT(l_index < s_curves.size());

			return l_index;
		}
	}
}