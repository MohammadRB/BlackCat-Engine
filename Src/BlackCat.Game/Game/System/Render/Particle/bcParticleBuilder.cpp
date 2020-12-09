// [11/14/2020 MRB]

#include "Game/GamePCH.h"

#include "Game/System/Render/Particle/bcParticleBuilder.h"

namespace black_cat
{
	namespace game
	{
		const bc_particle_curve bc_particle_builder::s_curve_constant =		{ core::bc_vector2f{0.0f, 1.0f}, core::bc_vector2f{0.50f, 1.00f}, core::bc_vector2f{1.0f, 1.0f} };
		const bc_particle_curve bc_particle_builder::s_curve_slow_step5 =	{ core::bc_vector2f{0.0f, 0.0f}, core::bc_vector2f{0.90f, 0.05f}, core::bc_vector2f{1.0f, 1.0f} };
		const bc_particle_curve bc_particle_builder::s_curve_slow_step4 =	{ core::bc_vector2f{0.0f, 0.0f}, core::bc_vector2f{0.82f, 0.10f}, core::bc_vector2f{1.0f, 1.0f} };
		const bc_particle_curve bc_particle_builder::s_curve_slow_step3 =	{ core::bc_vector2f{0.0f, 0.0f}, core::bc_vector2f{0.74f, 0.15f}, core::bc_vector2f{1.0f, 1.0f} };
		const bc_particle_curve bc_particle_builder::s_curve_slow_step2 =	{ core::bc_vector2f{0.0f, 0.0f}, core::bc_vector2f{0.66f, 0.20f}, core::bc_vector2f{1.0f, 1.0f} };
		const bc_particle_curve bc_particle_builder::s_curve_slow_step1 =	{ core::bc_vector2f{0.0f, 0.0f}, core::bc_vector2f{0.58f, 0.25f}, core::bc_vector2f{1.0f, 1.0f} };
		const bc_particle_curve bc_particle_builder::s_curve_linear =		{ core::bc_vector2f{0.0f, 1.0f}, core::bc_vector2f{0.50f, 0.50f}, core::bc_vector2f{1.0f, 0.0f} };
		const bc_particle_curve bc_particle_builder::s_curve_fast_step1 =	{ core::bc_vector2f{0.0f, 1.0f}, core::bc_vector2f{0.42f, 0.25f}, core::bc_vector2f{1.0f, 0.0f} };
		const bc_particle_curve bc_particle_builder::s_curve_fast_step2 =	{ core::bc_vector2f{0.0f, 1.0f}, core::bc_vector2f{0.34f, 0.20f}, core::bc_vector2f{1.0f, 0.0f} };
		const bc_particle_curve bc_particle_builder::s_curve_fast_step3 =	{ core::bc_vector2f{0.0f, 1.0f}, core::bc_vector2f{0.26f, 0.15f}, core::bc_vector2f{1.0f, 0.0f} };
		const bc_particle_curve bc_particle_builder::s_curve_fast_step4 =	{ core::bc_vector2f{0.0f, 1.0f}, core::bc_vector2f{0.18f, 0.10f}, core::bc_vector2f{1.0f, 0.0f} };
		const bc_particle_curve bc_particle_builder::s_curve_fast_step5 =	{ core::bc_vector2f{0.0f, 1.0f}, core::bc_vector2f{0.10f, 0.05f}, core::bc_vector2f{1.0f, 0.0f} };
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

		bc_particle_builder1& bc_particle_builder1::with_texture(bcUINT32 p_texture_index, bcUINT32 p_sprite_index) noexcept
		{
			m_emitter.m_texture_index = p_texture_index;
			m_emitter.m_sprite_index = p_sprite_index;
			return *this;
		}

		bc_particle_builder1& bc_particle_builder1::with_velocity_curve(const bc_particle_curve& p_curve) noexcept
		{
			m_emitter.m_velocity_curve_index = bc_particle_builder::_find_curve_index(p_curve);
			m_emitter.m_velocity_curve_duration = m_emitter.m_lifetime;
			return *this;
		}

		bc_particle_builder1& bc_particle_builder1::with_deviation(bcUINT32 p_angle) noexcept
		{
			m_emitter.m_deviation_angle = p_angle;
			return *this;
		}

		bc_particle_builder1& bc_particle_builder1::with_particle_reverse_direction() noexcept
		{
			m_emitter.m_particles_velocity_reverse_direction = -1;
			return *this;
		}

		bc_particle_builder1& bc_particle_builder1::with_particle_size(bcFLOAT p_size) noexcept
		{
			m_emitter.m_particles_size = p_size;
			return *this;
		}

		bc_particle_builder1& bc_particle_builder1::with_particle_velocity_curve(const bc_particle_curve& p_curve, bcFLOAT p_curve_duration) noexcept
		{
			m_emitter.m_particles_curve_index = bc_particle_builder::_find_curve_index(p_curve);
			m_emitter.m_particles_velocity_curve_duration = p_curve_duration;
			return *this;
		}

		bc_particle_builder& bc_particle_builder1::emit_particles(bcUINT32 p_particles_total_count, 
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
			bcFLOAT p_lifetime,
			bcFLOAT p_force,
			bcFLOAT p_mass) noexcept
		{
			bc_particle_emitter_trait l_emitter;
			l_emitter.m_position = p_position;
			l_emitter.m_lifetime = p_lifetime;
			l_emitter.m_direction = core::bc_vector3f::normalize(p_direction);
			l_emitter.m_force = p_force;
			l_emitter.m_mass = p_mass;
			l_emitter.m_deviation_angle = 45;
			l_emitter.m_texture_index = 0;
			l_emitter.m_sprite_index = 0;
			l_emitter.m_velocity_curve_index = _find_curve_index(s_curve_constant);
			l_emitter.m_velocity_curve_duration = p_lifetime;
			l_emitter.m_particles_total_count = 0;
			l_emitter.m_particles_lifetime = 0;
			l_emitter.m_particles_force = 0;
			l_emitter.m_particles_mass = 0;
			l_emitter.m_particles_size = 1;
			l_emitter.m_particles_curve_index = _find_curve_index(s_curve_constant);
			l_emitter.m_particles_velocity_curve_duration = 0;
			l_emitter.m_particles_velocity_reverse_direction = 1;

			m_emitters.push_back(l_emitter);

			return bc_particle_builder1(*this, *m_emitters.rbegin());
		}

		decltype(bc_particle_builder::s_curves)::size_type bc_particle_builder::_find_curve_index(const bc_particle_curve& p_curve) noexcept
		{
			const auto l_index = std::find(std::begin(s_curves), std::end(s_curves), &p_curve) - std::begin(s_curves);
			bcAssert(l_index < s_curves.size());

			return l_index;
		}
	}
}