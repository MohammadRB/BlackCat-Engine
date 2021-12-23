// [12/19/2020 MRB]

#include "App/BlackCatPCH.h"

#include "Core/bcUtility.h"
#include "Game/Object/Scene/bcEntityManager.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/Component/bcLightComponent.h"
#include "Game/Object/Scene/Component/bcParticleEmitterComponent.h"
#include "App/SampleImp/ActorController/bcFireActorController.h"

namespace black_cat
{
	void bc_fire_actor_controller::initialize(const game::bc_actor_component_initialize_context& p_context)
	{
		auto* l_light_component = p_context.m_actor.get_component<game::bc_light_component>();
		auto* l_emitter_component = p_context.m_actor.get_create_component<game::bc_particle_emitter_component>();

		if (!l_light_component || !l_light_component->get_light()->as_point_light())
		{
			throw bc_invalid_operation_exception("fire actor must have point light components");
		}

		const auto l_emitter = game::bc_particle_builder()
			.emitter(core::bc_vector3f(0), core::bc_vector3f::up(), 100, 0, 0)
			.with_emission_deviation(60)
			.with_texture(4)
			.with_particles_color({ 0.1f, 0.1f, 0.1f }, 1)
			.with_particle_size(2.0f, 8.0f)
			.with_particles_rotation(20)
			.with_particle_velocity_curve(game::bc_particle_builder::s_curve_fast_step2, 5)
			.emit_particles(0, 10, 20, -0.1f)
			.duplicate_last(core::bc_vector3f(0), core::bc_vector3f::up())
			.with_texture(0)
			.emit_particles(0, 11, 19, -0.1f)
			.duplicate_last(core::bc_vector3f(0), core::bc_vector3f::up())
			.with_texture(10)
			.with_particles_color({ 0.9f, 0.005f, 0.001f }, 3.0)
			.with_particle_size(0.1f, 0.3f)
			.emit_particles(0, 3, 25, 0.0f);
		l_emitter_component->add_emitter(l_emitter);

		auto* l_point_light = l_light_component->get_light()->as_point_light();
		m_light_intensity = l_point_light->get_intensity();
		m_light_flare_intensity = l_point_light->get_flare() ? l_point_light->get_flare()->get_intensity() : 0;
	}

	void bc_fire_actor_controller::update(const game::bc_actor_component_update_content& p_context)
	{
		auto* l_light_component = p_context.m_actor.get_component<game::bc_light_component>();
		auto* l_emitter_component = p_context.m_actor.get_component<game::bc_particle_emitter_component>();

		const bcFLOAT l_noise = (bc_noise(p_context.m_clock.m_total_elapsed_second / 7.5f, 1) - 0.5f) * m_light_intensity * 0.2f;
		auto* l_point_light = l_light_component->get_light()->as_point_light();
		l_point_light->set_intensity(m_light_intensity + l_noise);
		l_point_light->set_flare_intensity(m_light_flare_intensity + l_noise);

		const bcUINT32 l_total_particles_in_current_second = (p_context.m_clock.m_total_elapsed_second - std::floor(p_context.m_clock.m_total_elapsed_second)) * m_num_particles_per_second;
		if (l_total_particles_in_current_second < m_num_spawned_particles_in_current_second)
		{
			m_num_spawned_particles_in_current_second = 0;
		}

		const bcUINT32 l_num_particles_in_current_second = l_total_particles_in_current_second - m_num_spawned_particles_in_current_second;
		l_emitter_component->get_emitter()->set_particle_counts(l_num_particles_in_current_second);

		m_num_spawned_particles_in_current_second += l_num_particles_in_current_second;
	}
}