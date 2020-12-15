// [11/15/2020 MRB]

#include "Game/GamePCH.h"

#include "Core/Math/bcCoordinate.h"
#include "Core/Math/bcMatrix3f.h"
#include "Core/Messaging/Query/bcQueryManager.h"
#include "Core/Content/bcContentStreamManager.h"
#include "Core/Utility/bcLogger.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Query/bcParticleEmittersQuery.h"
#include "Game/System/Render/Particle/bcParticleManager.h"
#include "Game/System/Input/bcFileSystem.h"

namespace black_cat
{
	namespace game
	{
		_bc_particle_emitter_instance::_bc_particle_emitter_instance(const bc_particle_emitter_trait& p_trait)
			: bc_particle_emitter_trait(p_trait)
		{
			m_age = 0;
			m_energy = 0;
			m_spawned_particles_count = 0;
			m_particles_count_to_spawn = 0;
		}

		bc_particle_manager::bc_particle_manager(graphic::bc_device& p_device, core::bc_content_stream_manager& p_content_stream)
			: m_emitters(s_emitter_count, core::bc_alloc_type::unknown_movable)
		{
			for(bcSIZE l_curve_i = 0; l_curve_i < bc_particle_builder::s_curves.size(); ++l_curve_i)
			{
				const auto* l_particle_curve = bc_particle_builder::s_curves[l_curve_i];
				core::bc_curve2f l_curve(std::cbegin(*l_particle_curve), std::cend(*l_particle_curve));
				// Sample at higher rate than actually needed to avoid repeated values in output array
				auto l_curve_samples = l_curve.sample_many(curve_sample_count * 2);
				curve_sample_container l_curve_samples_container;

				for(bcSIZE l_curve_sample_i = 0; l_curve_sample_i < curve_sample_count; ++l_curve_sample_i)
				{
					const auto l_time = static_cast<bcFLOAT>(l_curve_sample_i) / (curve_sample_count - 1);
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

			m_emitters_provider_handle = core::bc_get_service< core::bc_query_manager >()
					->register_query_provider< bc_particle_emitters_query_context >
					(
						core::bc_query_provider_handle::delegate_t
						(*this, &bc_particle_manager::_emitters_query_context_provider)
					);

			const auto l_sprites_path = core::bc_get_service<bc_game_system>()->get_file_system().get_content_texture_path(bcL("Sprites.dds"));
			m_sprites = p_content_stream.get_content_manager().load< graphic::bc_texture2d_content >
			(
				core::bc_alloc_type::program,
				l_sprites_path.c_str(),
				core::bc_content_loader_parameter()
			);

			auto l_sprites_texture = m_sprites->get_resource();
			auto l_sprites_view_config = graphic::bc_graphic_resource_builder()
				.as_resource_view()
				.as_texture_view(m_sprites->get_resource().get_format())
				.as_tex2d_shader_view(0, -1)
				.on_texture2d();
			m_sprites_view = p_device.create_resource_view(l_sprites_texture, l_sprites_view_config);
		}

		bc_particle_manager::bc_particle_manager(bc_particle_manager&& p_other) noexcept
			: m_curves(p_other.m_curves),
			m_emitter_definitions(std::move(p_other.m_emitter_definitions)),
			m_emitters(std::move(p_other.m_emitters)),
			m_emitters_provider_handle(std::move(p_other.m_emitters_provider_handle)),
			m_sprites(std::move(p_other.m_sprites)),
			m_sprites_view(std::move(p_other.m_sprites_view))
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
			m_sprites = std::move(p_other.m_sprites);
			m_sprites_view = std::move(p_other.m_sprites_view);
			
			m_emitters_provider_handle.reassign(core::bc_query_provider_handle::delegate_t(*this, &bc_particle_manager::_emitters_query_context_provider));

			return *this;
		}

		graphic::bc_resource_view bc_particle_manager::get_sprites_view() const noexcept
		{
			return m_sprites_view.get();
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
					l_ite->m_prev_position = l_ite->m_position;
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
					.with_deviation(60)
					.with_texture(4)
					.with_particles_color({ 0.1f, 0.1f, 0.1f }, 1.0)
					.with_particle_size(3.0f, 8.0f)
					.with_particles_rotation(20)
					.with_particle_velocity_curve(bc_particle_builder::s_curve_fast_step2, 5)
					.emit_particles(2800, 10, 20, 0.1f)
					.duplicate_last(core::bc_vector3f(0), core::bc_vector3f::up())
					.with_texture(0)
					.emit_particles(2850, 10, 20, 0.1f);
				register_emitter_definition("test_emitter", l_test_emitter);

				spawn_emitter
				(
					"test_emitter",
					core::bc_vector3f(29, 48, -734),
					core::bc_vector3f::up()
				);
			}
			g_last_elapsed += p_clock.m_elapsed_second;
			//if(g_last_elapsed >= 5)
			//{
			//	g_last_elapsed -= 5;
			//	
			//	/*const auto l_builder = bc_particle_builder()
			//		.emitter(core::bc_vector3f(0), core::bc_vector3f::up())
			//		.with_deviation(180)
			//		.with_particle_size(0.9f, 1.5f)
			//		.with_particle_velocity_curve(bc_particle_builder::s_curve_fast_step5, 1.5)
			//		.emit_particles(1000, 3, 80, 0.3f);*/

			//	bc_particle_builder l_builder;
			//	l_builder.emitter(core::bc_vector3f(0), core::bc_vector3f::up())
			//		.with_deviation(180)
			//		.with_texture(2)
			//		.with_particles_color({ 0.07f, 0.07f, 0.07f }, 0.1)
			//		.with_particle_size(6, 9)
			//		.with_particles_rotation(45)
			//		.with_particle_size_curve(bc_particle_builder::s_curve_fast_step3)
			//		.with_particle_velocity_curve(bc_particle_builder::s_curve_fast_step4, 0.4f)
			//		.emit_particles(300, 3, 300, 0.1f);
			//		//.duplicate_last(core::bc_vector3f(0), core::bc_vector3f::up())
			//		//.with_texture(4)
			//		//.with_particles_color({1.0f, 0.4f, 0.001f}, 1000)
			//		//.with_particle_velocity_curve(bc_particle_builder::s_curve_fast_step5, 0.25f)
			//		//.emit_particles(100, 0.5f, 50, 0.05f);

			//	core::bc_array<std::tuple<core::bc_vector3f, core::bc_vector3f>, 7> l_emitter_traits =
			//	{
			//		std::make_tuple(core::bc_vector3f(0, 0, 0), core::bc_vector3f(0, 1, 0)),
			//		std::make_tuple(core::bc_vector3f(-2, 0, -2), core::bc_vector3f(-0.2, 1, -0.5)),
			//		std::make_tuple(core::bc_vector3f(-2, 0, 2), core::bc_vector3f(-0.3, 1, 0.5)),
			//		std::make_tuple(core::bc_vector3f(2, 0, 2), core::bc_vector3f(0.4, 1, 0.4)),
			//		std::make_tuple(core::bc_vector3f(2, 0, -2), core::bc_vector3f(0.5, 1, -0.6)),
			//		std::make_tuple(core::bc_vector3f(-2, 0, 0), core::bc_vector3f(-0.4, 1, 0.0)),
			//		std::make_tuple(core::bc_vector3f(2, 0, -2), core::bc_vector3f(0.4, 1, -0.1))
			//	};
			//	/*for(auto& l_trait : l_emitter_traits)
			//	{
			//		l_builder.emitter(std::get<0>(l_trait), std::get<1>(l_trait), 0.2f, 800, 0.4f)
			//			.with_deviation(90)
			//			.with_velocity_curve(bc_particle_builder::s_curve_fast_step5)
			//			.with_texture(1)
			//			.with_particles_color({ 0.8f, 0.8f, 0.8f }, 1.5)
			//			.with_particle_size(1.0f, 3.0f)
			//			.with_particle_size_curve(bc_particle_builder::s_curve_fast_step4)
			//			.with_particle_reverse_direction()
			//			.with_particle_velocity_curve(bc_particle_builder::s_curve_fast_step1, 1)
			//			.emit_particles(100, 3, 1.0, 0.4f);
			//	}*/
			//	
			//	register_emitter_definition("test_emitter", l_builder);
			//	spawn_emitter
			//	(
			//		"test_emitter",
			//		core::bc_vector3f(29, 48, -732),
			//		core::bc_vector3f::up()
			//	);
			//}
			
			{
				core_platform::bc_hybrid_mutex_guard l_lock(m_emitters_lock, core_platform::bc_lock_operation::heavy);
				
				m_emitters.remove_if
				(
					[](const _bc_particle_emitter_instance& p_emitter)
					{
						return p_emitter.m_age > p_emitter.m_lifetime;
					}
				);

				const auto l_constant_curve_index = bc_particle_builder::_find_curve_index(bc_particle_builder::s_curve_constant);
				
				for (_bc_particle_emitter_instance& l_emitter : m_emitters)
				{
					l_emitter.m_age += p_clock.m_elapsed_second;

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

						if(l_emitter.m_lifetime <= 0.001f)
						{
							l_energy = 1;
							l_particle_count_multiplier = 1;
						}
					}
					
					const bcFLOAT l_acceleration = l_emitter.m_force / l_emitter.m_mass * l_velocity_curve_value;
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
						l_emitter.m_emission_direction = p_emitter.m_direction * p_emitter.m_particles_velocity_reverse_direction;
						l_emitter.m_energy = p_emitter.m_energy;
						l_emitter.m_emission_deviation = p_emitter.m_deviation_angle;
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