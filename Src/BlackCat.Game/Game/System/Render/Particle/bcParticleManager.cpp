// [11/15/2020 MRB]

#include "Game/GamePCH.h"
#include "Core/Math/bcMatrix3f.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "Game/Query/bcParticleEmittersQuery.h"
#include "Game/System/Render/Particle/bcParticleManager.h"

namespace black_cat
{
	namespace game
	{
		_bc_particle_emitter_instance::_bc_particle_emitter_instance(const bc_particle_emitter_trait& p_trait)
			: bc_particle_emitter_trait(p_trait)
		{
			m_age = 0;
			m_spawned_particles_count = 0;
			m_particles_count_to_spawn = 0;
		}

		bc_particle_manager::bc_particle_manager()
			: m_emitters(s_emitter_count, core::bc_alloc_type::unknown_movable)
		{
			m_emitters_provider_handle = core::bc_get_service< core::bc_query_manager >()->register_query_provider< bc_particle_emitters_query_context >
			(
				core::bc_query_provider_handle::delegate_t(*this, &bc_particle_manager::_emitters_query_context_provider)
			);

			for(bcSIZE i = 0; i < bc_particle_builder::s_curves.size(); ++i)
			{
				const auto* l_particle_curve = bc_particle_builder::s_curves[i];
				core::bc_curve1f l_curve(std::cbegin(*l_particle_curve), std::cend(*l_particle_curve));
				auto l_curve_samples = l_curve.sample_many(curve_sample_count);
				curve_sample_container l_curve_samples_container;

				std::copy(std::begin(l_curve_samples), std::end(l_curve_samples), std::begin(l_curve_samples_container));

				m_curves[i] = l_curve_samples_container;
			}
		}

		bc_particle_manager::bc_particle_manager(bc_particle_manager&& p_other) noexcept
			: m_curves(p_other.m_curves),
			m_emitter_definitions(std::move(p_other.m_emitter_definitions)),
			m_emitters(std::move(p_other.m_emitters)),
			m_emitters_provider_handle(std::move(p_other.m_emitters_provider_handle))
		{
			m_emitters_provider_handle.reassign(core::bc_query_provider_handle::delegate_t(*this, &bc_particle_manager::_emitters_query_context_provider));
		}

		bc_particle_manager::~bc_particle_manager() = default;

		bc_particle_manager& bc_particle_manager::operator=(bc_particle_manager&& p_other) noexcept
		{
			m_curves = p_other.m_curves;
			m_emitter_definitions = std::move(p_other.m_emitter_definitions);
			m_emitters = std::move(p_other.m_emitters);
			m_emitters_provider_handle = std::move(p_other.m_emitters_provider_handle);
			
			m_emitters_provider_handle.reassign(core::bc_query_provider_handle::delegate_t(*this, &bc_particle_manager::_emitters_query_context_provider));

			return *this;
		}

		const bc_particle_manager::curves_container& bc_particle_manager::get_curves() const noexcept
		{
			return m_curves;
		}

		void bc_particle_manager::register_emitter_definition(const bcCHAR* p_name, const bc_particle_builder& p_builder)
		{
			core::bc_vector_program<bc_particle_emitter_trait> l_emitters(std::begin(p_builder.m_emitters), std::end(p_builder.m_emitters));

			auto l_value = std::make_pair(p_name, std::move(l_emitters));
			auto l_entry = m_emitter_definitions.find(p_name);

			if(l_entry == std::end(m_emitter_definitions))
			{
				l_entry = m_emitter_definitions.insert(std::move(l_value)).first;
			}
			else
			{
				l_entry->second = std::move(l_value.second);
			}
		}

		void bc_particle_manager::spawn_emitter(const bcCHAR* p_emitter_name, const core::bc_vector3f& p_pos, const core::bc_vector3f& p_dir)
		{
			core::bc_matrix3f l_rotation;
			l_rotation.rotation_between_two_vector_lh(core::bc_vector3f(0, 1, 0), p_dir);
			const auto l_definition_ite = m_emitter_definitions.find(p_emitter_name);
			
			bcAssert(l_definition_ite != std::end(m_emitter_definitions));

			{
				core_platform::bc_hybrid_mutex_guard l_lock(m_emitters_lock, core_platform::bc_lock_operation::light);
				
				for (auto& l_emitter : l_definition_ite->second)
				{
					m_emitters.emplace_back(l_emitter);
					auto l_ite = m_emitters.rbegin();
					
					l_ite->m_position += p_pos;
					l_ite->m_direction = l_rotation * l_ite->m_direction;
					l_ite->m_lifetime += .001f; // to avoid division by zero
					l_ite->m_mass += .001f; // to avoid division by zero
				}

				bcAssert(m_emitters.size() <= s_emitter_count);
			}
		}

		float g_last_elapsed = 0;
		void bc_particle_manager::update(const core_platform::bc_clock::update_param& p_clock)
		{
			if(g_last_elapsed == 0)
			{
				const auto l_test_emitter = bc_particle_builder()
					.emitter(core::bc_vector3f(0), core::bc_vector3f::up(), 100, 0, 0)
					.with_deviation(90)
					.with_particle_size(1.3)
					.with_particle_curve(bc_particle_builder::s_curve_fast_step5)
					.emit_particles(4000, 20, 1, 0.3f);
				register_emitter_definition("test_emitter", l_test_emitter);

				spawn_emitter
				(
					"test_emitter",
					core::bc_vector3f(29, 48, -732),
					core::bc_vector3f::up()
				);
			}
			g_last_elapsed += p_clock.m_elapsed_second;
			/*if(g_last_elapsed >= 5)
			{
				g_last_elapsed -= 5;
				
				const auto l_test_emitter = bc_particle_builder()
					.emitter(core::bc_vector3f(0), core::bc_vector3f::up())
					.with_deviation(360)
					.emit_particles(2000, 4, 2, 0.1f);
				register_emitter_definition("test_emitter", l_test_emitter);

				spawn_emitter
				(
					"test_emitter",
					core::bc_vector3f(29, 48, -732),
					core::bc_vector3f::up()
				);
			}*/
			
			{
				core_platform::bc_hybrid_mutex_guard l_lock(m_emitters_lock, core_platform::bc_lock_operation::heavy);
				
				m_emitters.remove_if
				(
					[](const _bc_particle_emitter_instance& p_emitter)
					{
						return p_emitter.m_age > p_emitter.m_lifetime;
					}
				);

				for (_bc_particle_emitter_instance& l_emitter : m_emitters)
				{
					l_emitter.m_age += p_clock.m_elapsed_second;

					const bcFLOAT l_acceleration = l_emitter.m_force / l_emitter.m_mass;
					const bcFLOAT l_normalized_age = _sample_curve(l_emitter.m_curve_index, l_emitter.m_age / l_emitter.m_lifetime);
					const bcFLOAT l_normalized_age_reversed = 1 - l_normalized_age;
					const bcFLOAT l_velocity_to_apply = l_normalized_age_reversed * l_acceleration;
					const bcUINT32 l_particles_to_spawn = l_normalized_age * l_emitter.m_particles_total_count;

					l_emitter.m_prev_position = l_emitter.m_position;
					l_emitter.m_position += l_emitter.m_direction * l_velocity_to_apply * p_clock.m_elapsed_second;
					l_emitter.m_particles_count_to_spawn = l_particles_to_spawn - l_emitter.m_spawned_particles_count;
					l_emitter.m_spawned_particles_count += l_emitter.m_particles_count_to_spawn;
				}
			}
		}

		core::bc_query_context_ptr bc_particle_manager::_emitters_query_context_provider() const
		{
			{
				core_platform::bc_hybrid_mutex_guard l_lock(m_emitters_lock, core_platform::bc_lock_operation::heavy);

				core::bc_vector_movable<bc_particle_emitter> l_emitters;
				l_emitters.reserve(m_emitters.size());

				std::transform
				(
					std::begin(m_emitters),
					std::end(m_emitters),
					std::back_inserter(l_emitters),
					[](const _bc_particle_emitter_instance& p_emitter)
					{
						bc_particle_emitter l_emitter;
						l_emitter.m_prev_position = p_emitter.m_prev_position;
						l_emitter.m_position = p_emitter.m_position;
						l_emitter.m_emission_direction = p_emitter.m_direction * p_emitter.m_particles_reverse_direction;
						l_emitter.m_emission_deviation = p_emitter.m_deviation_angle;
						l_emitter.m_texture_index = p_emitter.m_texture_index;
						l_emitter.m_sprite_index = p_emitter.m_sprite_index;
						l_emitter.m_particles_count = p_emitter.m_particles_count_to_spawn;
						l_emitter.m_particles_curve_index = p_emitter.m_particles_curve_index;
						l_emitter.m_particles_lifetime = p_emitter.m_particles_lifetime;
						l_emitter.m_particles_force = p_emitter.m_particles_energy;
						l_emitter.m_particles_mass = p_emitter.m_particles_mass;
						l_emitter.m_particles_size = p_emitter.m_particles_size;

						return l_emitter;
					}
				);

				return core::bc_make_query_context(bc_particle_emitters_query_context(l_emitters));
			}
		}

		bcFLOAT bc_particle_manager::_sample_curve(bcSIZE p_curve_index, bcFLOAT p_normalized_time) const
		{
			p_normalized_time = std::min(1.f, p_normalized_time);
			const curve_sample_container& l_curve_samples = m_curves[p_curve_index];
			const bcFLOAT l_time = p_normalized_time * (curve_sample_count - 1);
			const bcSIZE l_lower_band = std::floor(l_time);
			const bcSIZE l_upper_band = std::ceil(l_time);

			const bcFLOAT l_lower_band_time = static_cast<bcFLOAT>(l_lower_band) / (curve_sample_count - 1);
			const bcFLOAT l_lower_band_sample = l_curve_samples[l_lower_band];
			const bcFLOAT l_upper_band_time = static_cast<bcFLOAT>(l_upper_band) / (curve_sample_count - 1);
			const bcFLOAT l_upper_band_sample = l_curve_samples[l_upper_band];

			const bcFLOAT l_normalized_time_between_samples = (p_normalized_time - l_lower_band_time) / (l_upper_band_time - l_lower_band_time + .001f);

			return l_normalized_time_between_samples * (l_upper_band_sample - l_lower_band_sample) + l_lower_band_sample;
		}
	}
}
