// [03/03/2022 MRB]

#include "Game/GamePCH.h"

#include "Core/Math/bcCoordinate.h"
#include "Core/Math/bcMatrix3f.h"
#include "Game/System/Render/Particle/bcParticleManagerContainer.h"
#include "Game/System/Render/Particle/bcParticleManager.h"
#include "Game/bcUtility.h"
#include "Game/bcException.h"

namespace black_cat
{
	namespace game
	{
		_bc_particle_emitter_instance::_bc_particle_emitter_instance(const bc_particle_emitter_trait& p_trait)
			: bc_particle_emitter_trait(p_trait),
			m_age(0),
			m_energy(0),
			m_spawned_particles_count(0),
			m_particles_count_to_spawn(0)
		{
		}

		bc_particle_manager_container::bc_particle_manager_container(const bc_particle_manager& p_manager)
			: m_manager(&p_manager),
			m_emitters(s_emitters_pool_size),
			m_external_emitters(s_emitters_pool_size / 4)
		{
		}

		bc_particle_manager_container::bc_particle_manager_container(bc_particle_manager_container&& p_other) noexcept
			: m_manager(p_other.m_manager),
			m_emitters(std::move(p_other.m_emitters)),
			m_external_emitters(std::move(p_other.m_external_emitters))
		{
		}

		bc_particle_manager_container& bc_particle_manager_container::operator=(bc_particle_manager_container&& p_other) noexcept
		{
			m_manager = p_other.m_manager;
			m_emitters = std::move(p_other.m_emitters);
			m_external_emitters = std::move(p_other.m_external_emitters);

			return *this;
		}

		void bc_particle_manager_container::spawn_emitter(const bcCHAR* p_emitter_name,
			const core::bc_vector3f& p_pos,
			const core::bc_vector3f& p_dir,
			const core::bc_vector3f* p_color,
			bcFLOAT p_scale)
		{
			const auto l_rotation = bc_matrix3f_rotation_between_two_vector_checked(core::bc_vector3f::up(), p_dir);
			const auto l_definition = m_manager->get_emitter_definition(p_emitter_name);

			BC_ASSERT(!l_definition.empty());

			{
				core_platform::bc_hybrid_mutex_guard l_lock(m_emitters_lock, core_platform::bc_lock_operation::light);

				for (auto& l_emitter : l_definition)
				{
					m_emitters.emplace_back(l_emitter);
					auto& l_ite = m_emitters.back();

					if (p_scale != 1)
					{
						_apply_emitter_scale(l_ite, p_scale);
					}

					l_ite.m_position = l_rotation * l_ite.m_position + p_pos;
					l_ite.m_prev_position = l_ite.m_position;
					l_ite.m_direction = l_rotation * l_ite.m_direction;
					l_ite.m_emission_deviation_force = l_rotation * l_ite.m_emission_deviation_force;
					l_ite.m_lifetime += .0001f; // to avoid division by zero

					if (p_color)
					{
						const auto l_src_color_magnitude = l_ite.m_particles_color.magnitude();
						const auto l_color = core::bc_vector3f::normalize(*p_color * *p_color);
						l_ite.m_particles_color = core::bc_vector3f::normalize(l_color * l_ite.m_particles_color) * l_src_color_magnitude;
					}

					bc_randomize_direction(m_random, l_ite.m_direction, l_ite.m_direction_deviation, &l_ite.m_direction, &l_ite.m_direction + 1);
				}

				BC_ASSERT(m_emitters.size() <= s_emitters_pool_size);
			}
		}

		bc_particle_emitter_ptr bc_particle_manager_container::add_emitter(const bc_particle_builder& p_builder)
		{
			{
				core_platform::bc_hybrid_mutex_guard l_lock(m_emitters_lock, core_platform::bc_lock_operation::light);

				emitters_container::iterator l_first_emitter = std::end(m_emitters);
				for (auto& l_emitter : p_builder.m_emitters)
				{
					m_emitters.emplace_back(l_emitter);
					auto l_ite = m_emitters.rbegin();

					l_ite->m_lifetime = -1;
					l_ite->m_prev_position = l_ite->m_position;

					bc_randomize_direction(m_random, l_ite->m_direction, l_ite->m_direction_deviation, &l_ite->m_direction, &l_ite->m_direction + 1);

					if (l_first_emitter == std::end(m_emitters))
					{
						l_first_emitter = l_ite.base();
					}
				}

				BC_ASSERT(m_emitters.size() <= s_emitters_pool_size);

				bc_external_particle_emitter l_external_emitter;
				l_external_emitter.m_emitters.reserve(p_builder.m_emitters.size());
				while (l_first_emitter != std::end(m_emitters))
				{
					l_external_emitter.m_emitters.push_back(&*l_first_emitter);
					++l_first_emitter;
				}

				m_external_emitters.push_back(std::move(l_external_emitter));

				return bc_particle_emitter_ptr(&*m_external_emitters.rbegin(), _bc_particle_emitter_ptr_deleter(this));
			}
		}

		void bc_particle_manager_container::update(const core_platform::bc_clock::update_param& p_clock)
		{
			const auto l_elapsed = p_clock.m_fixed_elapsed_second;

			{
				core_platform::bc_hybrid_mutex_guard l_lock(m_emitters_lock, core_platform::bc_lock_operation::heavy);

				m_emitters.remove_if
				(
					[](const _bc_particle_emitter_instance& p_emitter)
					{
						return p_emitter.m_lifetime >= 0 && p_emitter.m_age > p_emitter.m_lifetime;
					}
				);

				const auto l_constant_curve_index = bc_particle_builder::_find_curve_index(bc_particle_builder::s_curve_constant);

				for (_bc_particle_emitter_instance& l_emitter : m_emitters)
				{
					l_emitter.m_age += l_elapsed;

					if (l_emitter.m_lifetime < 0) // it is external emitter
					{
						l_emitter.m_prev_position = l_emitter.m_position;
						l_emitter.m_energy = 1;
						l_emitter.m_particles_count_to_spawn = l_emitter.m_particles_total_count;
						l_emitter.m_spawned_particles_count += l_emitter.m_particles_count_to_spawn;

						continue;
					}

					bcFLOAT l_velocity_curve_value;
					bcFLOAT l_energy;
					bcFLOAT l_particle_count_multiplier;
					if (l_emitter.m_velocity_curve_index != l_constant_curve_index)
					{
						const auto l_velocity_curve_sample = m_manager->sample_curve(l_emitter.m_velocity_curve_index, l_emitter.m_age / l_emitter.m_velocity_curve_duration);
						l_velocity_curve_value = l_velocity_curve_sample;
						l_energy = l_velocity_curve_sample;
						l_particle_count_multiplier = 1 - l_energy;
					}
					else
					{
						const auto l_velocity_curve_sample = m_manager->sample_curve(l_emitter.m_velocity_curve_index, l_emitter.m_age / l_emitter.m_velocity_curve_duration);
						l_velocity_curve_value = l_velocity_curve_sample;
						l_energy = 1 - std::min(1.f, l_emitter.m_age / l_emitter.m_lifetime);
						l_particle_count_multiplier = 1 - l_energy;

						if (l_emitter.m_lifetime <= 0.0001f)
						{
							l_energy = 1;
							l_particle_count_multiplier = 1;
						}
					}

					const bcFLOAT l_acceleration = l_emitter.m_force * l_velocity_curve_value;
					const bcUINT32 l_particles_to_spawn = l_particle_count_multiplier * l_emitter.m_particles_total_count;

					l_emitter.m_prev_position = l_emitter.m_position;
					l_emitter.m_position += (l_emitter.m_direction * l_acceleration * l_elapsed) + (core::bc_vector3f::up() * -1 * 9.8f * l_emitter.m_mass * l_elapsed);
					l_emitter.m_force = std::max(0.f, l_emitter.m_force - l_emitter.m_mass * l_elapsed);
					l_emitter.m_energy = l_energy;
					l_emitter.m_particles_count_to_spawn = l_particles_to_spawn - l_emitter.m_spawned_particles_count;
					l_emitter.m_spawned_particles_count += l_emitter.m_particles_count_to_spawn;
				}
			}
		}

		core::bc_vector_movable<bc_particle_emitter_state> bc_particle_manager_container::get_emitter_states() const noexcept
		{
			{
				core_platform::bc_hybrid_mutex_guard l_lock(m_emitters_lock, core_platform::bc_lock_operation::heavy);

				core::bc_vector_movable<bc_particle_emitter_state> l_emitters;
				l_emitters.reserve(m_emitters.size());

				std::transform
				(
					std::begin(m_emitters),
					std::end(m_emitters),
					std::back_inserter(l_emitters),
					[](const _bc_particle_emitter_instance& p_emitter)
					{
						bc_particle_emitter_state l_emitter;
						l_emitter.m_prev_position = p_emitter.m_prev_position;
						l_emitter.m_position = p_emitter.m_position;
						l_emitter.m_emission_direction = p_emitter.m_direction * p_emitter.m_particles_velocity_reverse_direction;
						l_emitter.m_energy = p_emitter.m_energy;
						l_emitter.m_emission_deviation = p_emitter.m_emission_deviation;
						l_emitter.m_emission_deviation_force = p_emitter.m_emission_deviation_force;
						l_emitter.m_sprite_index = p_emitter.m_sprite_index;
						l_emitter.m_particles_color = p_emitter.m_particles_color;
						l_emitter.m_particles_color_intensity = p_emitter.m_particles_color_intensity;
						l_emitter.m_particles_count = p_emitter.m_particles_count_to_spawn;
						l_emitter.m_particles_lifetime = p_emitter.m_particles_lifetime;
						l_emitter.m_particles_force = p_emitter.m_particles_force;
						l_emitter.m_particles_mass = p_emitter.m_particles_mass;
						l_emitter.m_particles_start_size = p_emitter.m_particles_start_size;
						l_emitter.m_particles_end_size = p_emitter.m_particles_end_size;
						l_emitter.m_particles_rotation = core::bc_to_radian(p_emitter.m_particles_rotation);
						l_emitter.m_particles_velocity_curve_index = p_emitter.m_particles_velocity_curve_index;
						l_emitter.m_particles_velocity_curve_duration = p_emitter.m_particles_velocity_curve_duration;
						l_emitter.m_particles_size_curve_index = p_emitter.m_particles_size_curve_index;
						l_emitter.m_particles_fade_curve_index = p_emitter.m_particles_fade_curve_index;

						return l_emitter;
					}
				);

				return l_emitters;
			}
		}

		void bc_particle_manager_container::_destroy_emitter(bc_external_particle_emitter* p_emitter)
		{
			{
				core_platform::bc_hybrid_mutex_guard l_lock(m_emitters_lock, core_platform::bc_lock_operation::light);

				const auto l_external_emitter_entry = std::find_if
				(
					std::begin(m_external_emitters),
					std::end(m_external_emitters),
					[=](const bc_external_particle_emitter& p_external_emitter)
					{
						return &p_external_emitter == p_emitter;
					}
				);

				BC_ASSERT(l_external_emitter_entry != std::end(m_external_emitters));

				for (bc_particle_emitter_trait* l_external_emitter : l_external_emitter_entry->m_emitters)
				{
					const auto l_emitter_entry = std::find_if
					(
						std::begin(m_emitters),
						std::end(m_emitters),
						[=](const _bc_particle_emitter_instance& p_emitter)
						{
							return &p_emitter == l_external_emitter;
						}
					);

					BC_ASSERT(l_emitter_entry != std::end(m_emitters));

					m_emitters.erase(l_emitter_entry);
				}

				m_external_emitters.erase(l_external_emitter_entry);
			}
		}

		void bc_particle_manager_container::_apply_emitter_scale(bc_particle_emitter_trait& p_emitter, bcFLOAT p_scale) const noexcept
		{
			p_emitter.m_position *= p_scale;
			p_emitter.m_force *= p_scale;
			p_emitter.m_mass *= p_scale;
			p_emitter.m_particles_force *= p_scale;
			p_emitter.m_particles_mass *= p_scale;
			p_emitter.m_particles_start_size *= p_scale;
			p_emitter.m_particles_end_size *= p_scale;
			p_emitter.m_particles_total_count *= p_scale;
		}
	}
}
