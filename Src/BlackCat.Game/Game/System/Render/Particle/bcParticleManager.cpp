// [11/15/2020 MRB]

#include "Game/GamePCH.h"
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
			: m_emitters(core::bc_memory_pool_allocator<bc_particle_emitter_trait>(m_emitters_pool))
		{
			m_emitters_pool.initialize(m_num_emitter_count, sizeof(_bc_particle_emitter_instance), core::bc_alloc_type::unknown_movable);
			m_emitters_provider_handle.reassign(core::bc_query_provider_handle::delegate_t(*this, &bc_particle_manager::_emitters_query_context_provider));
		}

		bc_particle_manager::bc_particle_manager(bc_particle_manager&& p_other) noexcept
			: m_emitter_definitions(std::move(p_other.m_emitter_definitions)),
			m_emitters_pool(std::move(p_other.m_emitters_pool)),
			m_emitters(std::move(p_other.m_emitters)),
			m_emitters_provider_handle(std::move(p_other.m_emitters_provider_handle))
		{
			m_emitters_provider_handle.reassign(core::bc_query_provider_handle::delegate_t(*this, &bc_particle_manager::_emitters_query_context_provider));
		}

		bc_particle_manager::~bc_particle_manager() = default;

		bc_particle_manager& bc_particle_manager::operator=(bc_particle_manager&& p_other) noexcept
		{
			m_emitter_definitions = std::move(p_other.m_emitter_definitions);
			m_emitters_pool = std::move(p_other.m_emitters_pool);
			m_emitters = std::move(p_other.m_emitters);
			m_emitters_provider_handle = std::move(p_other.m_emitters_provider_handle);
			
			m_emitters_provider_handle.reassign(core::bc_query_provider_handle::delegate_t(*this, &bc_particle_manager::_emitters_query_context_provider));

			return *this;
		}
		
		void bc_particle_manager::register_emitter_definition(const bcCHAR* p_name, const bc_particle_builder& p_builder)
		{
			core::bc_vector_program<bc_particle_emitter_trait> l_emitters(std::begin(p_builder.m_emitters), std::end(p_builder.m_emitters));

			auto l_value = std::make_pair(p_name, std::move(l_emitters));
			m_emitter_definitions.insert(std::move(l_value));
		}

		void bc_particle_manager::emit(const bcCHAR* p_emitter_name, const core::bc_matrix4f& p_transformation)
		{
			const auto l_definition_ite = m_emitter_definitions.find(p_emitter_name);
			
			bcAssert(l_definition_ite != std::end(m_emitter_definitions));

			{
				core_platform::bc_hybrid_mutex_guard l_lock(m_emitters_lock, core_platform::bc_lock_operation::light);
				
				for (auto& l_emitter : l_definition_ite->second)
				{
					m_emitters.emplace_back(l_emitter);
					
					auto l_ite = m_emitters.rbegin();
					l_ite->m_position = p_transformation.get_translation();
					l_ite->m_direction = p_transformation.get_rotation() * l_ite->m_direction;
					l_ite->m_lifetime += .001; // to avoid 0 lifetime and division by zero
				}

				bcAssert(m_emitters.size() <= m_num_emitter_count);
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
						return p_emitter.m_age > p_emitter.m_lifetime;
					}
				);

				for (_bc_particle_emitter_instance& l_emitter : m_emitters)
				{
					l_emitter.m_age += p_clock.m_elapsed;

					const bcFLOAT l_acceleration = l_emitter.m_force / l_emitter.m_mass;
					const bcFLOAT l_normalized_age = 1 - std::powf(std::max(1.f, l_emitter.m_age / l_emitter.m_lifetime), 2);
					const bcFLOAT l_velocity_to_apply = l_normalized_age * l_acceleration;
					const bcUINT32 l_particles_to_spawn = l_normalized_age * l_emitter.m_particles_total_count;

					l_emitter.m_position += l_emitter.m_direction * l_velocity_to_apply;
					l_emitter.m_force *= l_normalized_age;
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
						l_emitter.m_position = p_emitter.m_position;
						l_emitter.m_emission_direction = p_emitter.m_direction;
						l_emitter.m_emission_deviation = p_emitter.m_deviation_angle;
						l_emitter.m_texture_index = p_emitter.m_texture_index;
						l_emitter.m_sprite_index = p_emitter.m_sprite_index;
						l_emitter.m_particles_count = p_emitter.m_particles_count_to_spawn;
						l_emitter.m_particles_lifetime = p_emitter.m_particles_lifetime;
						l_emitter.m_particles_force = p_emitter.m_particles_energy;
						l_emitter.m_particles_mass = p_emitter.m_particles_mass;

						return l_emitter;
					}
				);

				return core::bc_make_unique<bc_particle_emitters_query_context>(l_emitters);
			}
		}
	}
}