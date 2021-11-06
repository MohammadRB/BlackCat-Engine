// [11/15/2020 MRB]

#include "Game/GamePCH.h"

#include "Core/Math/bcCoordinate.h"
#include "Core/Math/bcMatrix3f.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Core/bcUtility.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Render/Particle/bcParticleManager.h"
#include "Game/System/Input/bcFileSystem.h"
#include "Game/bcUtility.h"
#include "Game/bcException.h"

namespace black_cat
{
	namespace game
	{
		bc_particle_manager::curves_container bc_particle_manager::m_curves{};
		core::bc_nullable<bc_particle_manager::particle_definition_container> bc_particle_manager::m_emitter_definitions;
		
		_bc_particle_emitter_instance::_bc_particle_emitter_instance(const bc_particle_emitter_trait& p_trait)
			: bc_particle_emitter_trait(p_trait)
		{
			m_age = 0;
			m_energy = 0;
			m_spawned_particles_count = 0;
			m_particles_count_to_spawn = 0;
		}

		bc_particle_manager::bc_particle_manager()
			: m_emitters(s_emitters_pool_size),
			m_external_emitters(s_emitters_pool_size / 4)
		{
		}

		bc_particle_manager::bc_particle_manager(bc_particle_manager&& p_other) noexcept
			: m_emitters(std::move(p_other.m_emitters)),
			m_external_emitters(std::move(p_other.m_external_emitters))
		{
		}

		bc_particle_manager::~bc_particle_manager() = default;

		bc_particle_manager& bc_particle_manager::operator=(bc_particle_manager&& p_other) noexcept
		{
			m_emitters = std::move(p_other.m_emitters);
			m_external_emitters = std::move(p_other.m_external_emitters);
			
			return *this;
		}

		const bc_particle_manager::curves_container& bc_particle_manager::get_curves() noexcept
		{
			return m_curves;
		}

		void bc_particle_manager::spawn_emitter(const bcCHAR* p_emitter_name, 
			const core::bc_vector3f& p_pos, 
			const core::bc_vector3f& p_dir, 
			const core::bc_vector3f* p_color,
			bcFLOAT p_scale)
		{
			const auto l_rotation = bc_matrix3f_rotation_between_two_vector_checked(core::bc_vector3f::up(), p_dir);
			const auto l_definition_ite = m_emitter_definitions->find(string_hash_t()(p_emitter_name));
			BC_ASSERT(l_definition_ite != std::end(*m_emitter_definitions));

			{
				core_platform::bc_hybrid_mutex_guard l_lock(m_emitters_lock, core_platform::bc_lock_operation::light);
				
				for (auto& l_emitter : l_definition_ite->second)
				{
					m_emitters.emplace_back(l_emitter);
					auto& l_ite = m_emitters.back();

					if(p_scale != 1)
					{
						_apply_emitter_scale(l_ite, p_scale);
					}

					l_ite.m_position = l_rotation * l_ite.m_position + p_pos;
					l_ite.m_prev_position = l_ite.m_position;
					l_ite.m_direction = l_rotation * l_ite.m_direction;
					l_ite.m_emission_deviation_force = l_rotation * l_ite.m_emission_deviation_force;
					l_ite.m_lifetime += .0001f; // to avoid division by zero

					if(p_color)
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

		bc_particle_emitter_ptr bc_particle_manager::add_emitter(const bc_particle_builder& p_builder)
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
					
					if(l_first_emitter == std::end(m_emitters))
					{
						l_first_emitter = l_ite.base();
					}
				}

				BC_ASSERT(m_emitters.size() <= s_emitters_pool_size);

				bc_external_particle_emitter l_external_emitter;
				l_external_emitter.m_emitters.reserve(p_builder.m_emitters.size());
				while(l_first_emitter != std::end(m_emitters))
				{
					l_external_emitter.m_emitters.push_back(static_cast<bc_particle_emitter_trait*>(&*l_first_emitter));
					++l_first_emitter;
				}

				m_external_emitters.push_back(std::move(l_external_emitter));

				return bc_particle_emitter_ptr(&*m_external_emitters.rbegin(), _bc_particle_emitter_ptr_deleter(this));
			}
		}

		void bc_particle_manager::update(const core_platform::bc_clock::update_param& p_clock)
		{			
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
					l_emitter.m_age += p_clock.m_elapsed_second;

					if(l_emitter.m_lifetime < 0) // it is external emitter
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
					if(l_emitter.m_velocity_curve_index != l_constant_curve_index)
					{
						const auto l_velocity_curve_sample = _sample_curve(l_emitter.m_velocity_curve_index, l_emitter.m_age / l_emitter.m_velocity_curve_duration);
						l_velocity_curve_value = l_velocity_curve_sample;
						l_energy = l_velocity_curve_sample;
						l_particle_count_multiplier = 1 - l_energy;
					}
					else
					{
						const auto l_velocity_curve_sample = _sample_curve(l_emitter.m_velocity_curve_index, l_emitter.m_age / l_emitter.m_velocity_curve_duration);
						l_velocity_curve_value = l_velocity_curve_sample;
						l_energy = 1 - std::min(1.f, l_emitter.m_age / l_emitter.m_lifetime);
						l_particle_count_multiplier = 1 - l_energy;

						if(l_emitter.m_lifetime <= 0.0001f)
						{
							l_energy = 1;
							l_particle_count_multiplier = 1;
						}
					}
					
					const bcFLOAT l_acceleration = l_emitter.m_force * l_velocity_curve_value;
					const bcUINT32 l_particles_to_spawn = l_particle_count_multiplier * l_emitter.m_particles_total_count;
					
					l_emitter.m_prev_position = l_emitter.m_position;
					l_emitter.m_position += (l_emitter.m_direction * l_acceleration * p_clock.m_elapsed_second) +
						(core::bc_vector3f::up() * -1 * 9.8f * l_emitter.m_mass * p_clock.m_elapsed_second);
					l_emitter.m_force = std::max(0.f, l_emitter.m_force - l_emitter.m_mass * p_clock.m_elapsed_second);
					l_emitter.m_energy = l_energy;
					l_emitter.m_particles_count_to_spawn = l_particles_to_spawn - l_emitter.m_spawned_particles_count;
					l_emitter.m_spawned_particles_count += l_emitter.m_particles_count_to_spawn;
				}
			}
		}

		core::bc_vector_movable<bc_particle_emitter_state> bc_particle_manager::get_emitter_states() const noexcept
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

				return std::move(l_emitters);
			}
		}

		void bc_particle_manager::init_emitter_states()
		{
			for (bcSIZE l_curve_i = 0; l_curve_i < bc_particle_builder::s_curves.size(); ++l_curve_i)
			{
				const auto* l_particle_curve = bc_particle_builder::s_curves[l_curve_i];
				core::bc_curve2f l_curve(std::cbegin(*l_particle_curve), std::cend(*l_particle_curve));
				// Sample at higher rate than actually needed to avoid repeated values in output array
				auto l_curve_samples = l_curve.sample_many(s_curve_sample_count * 2);
				curve_sample_container l_curve_samples_container;

				for (bcSIZE l_curve_sample_i = 0; l_curve_sample_i < s_curve_sample_count; ++l_curve_sample_i)
				{
					const auto l_time = static_cast<bcFLOAT>(l_curve_sample_i) / (s_curve_sample_count - 1);
					const auto l_read_sample = std::lower_bound
					(
						std::begin(l_curve_samples),
						std::end(l_curve_samples),
						core::bc_vector2f(l_time),
						[&](const core::bc_vector2f& p_sample1, const core::bc_vector2f& p_sample2)
						{
							return p_sample1.x < p_sample2.x;
						}
					);

					l_curve_samples_container[l_curve_sample_i] = l_read_sample->y;
				}

				m_curves[l_curve_i] = l_curve_samples_container;
			}
		}

		void bc_particle_manager::register_emitter_definition(const bcCHAR* p_name, const bc_particle_builder& p_builder)
		{
			if (!m_emitter_definitions.has_value())
			{
				m_emitter_definitions.reset(particle_definition_container());
			}

			core::bc_vector_program<bc_particle_emitter_trait> l_emitters(std::begin(p_builder.m_emitters), std::end(p_builder.m_emitters));

			auto l_hash = string_hash_t()(p_name);
			auto l_value = std::make_pair(l_hash, std::move(l_emitters));
			auto l_entry = m_emitter_definitions->find(l_hash);

			if (l_entry == std::end(*m_emitter_definitions))
			{
				l_entry = m_emitter_definitions->insert(std::move(l_value)).first;
			}
			else
			{
				l_entry->second = std::move(l_value.second);
			}
		}
		
		void bc_particle_manager::clear_emitter_states()
		{
			m_emitter_definitions->clear();
			m_emitter_definitions.reset();
		}

		void bc_particle_manager::_destroy_emitter(bc_external_particle_emitter* p_emitter)
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

		void bc_particle_manager::_apply_emitter_scale(bc_particle_emitter_trait& p_emitter, bcFLOAT p_scale) const noexcept
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

		bcFLOAT bc_particle_manager::_sample_curve(bcSIZE p_curve_index, bcFLOAT p_normalized_time) const noexcept
		{
			p_normalized_time = std::min(1.f, p_normalized_time);
			const curve_sample_container& l_curve_samples = m_curves[p_curve_index];
			const bcFLOAT l_time = p_normalized_time * (s_curve_sample_count - 1);
			const bcSIZE l_lower_band = std::floor(l_time);
			const bcSIZE l_upper_band = std::ceil(l_time);

			const bcFLOAT l_lower_band_time = static_cast<bcFLOAT>(l_lower_band) / (s_curve_sample_count - 1);
			const bcFLOAT l_lower_band_sample = l_curve_samples[l_lower_band];
			const bcFLOAT l_upper_band_time = static_cast<bcFLOAT>(l_upper_band) / (s_curve_sample_count - 1);
			const bcFLOAT l_upper_band_sample = l_curve_samples[l_upper_band];

			const bcFLOAT l_normalized_time_between_samples = (p_normalized_time - l_lower_band_time) / (l_upper_band_time - l_lower_band_time + .001f);

			return l_normalized_time_between_samples * (l_upper_band_sample - l_lower_band_sample) + l_lower_band_sample;
		}
	}
}