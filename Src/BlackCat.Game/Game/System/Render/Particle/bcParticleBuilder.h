// [11/12/2020 MRB]

#pragma once

#include "Core/Math/bcVector2f.h"
#include "Core/Container/bcArray.h"
#include "Core/Container/bcVector.h"
#include "Game/System/Render/Particle/bcParticleEmitter.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		class bc_particle_manager;
		class bc_particle_builder;
		using bc_particle_curve = core::bc_array< core::bc_vector2f, 3 >;

		class BC_GAME_DLL bc_particle_builder1
		{
		public:
			bc_particle_builder1(bc_particle_builder& p_builder, bc_particle_emitter_trait& p_emitter) noexcept;

			bc_particle_builder1& with_texture(bcUINT32 p_sprite_index) noexcept;

			bc_particle_builder1& with_velocity_curve(const bc_particle_curve& p_curve) noexcept;
			
			bc_particle_builder1& with_direction_deviation(bcUINT32 p_angle) noexcept;
			
			bc_particle_builder1& with_emission_deviation(bcUINT32 p_angle, const core::bc_vector3f& p_deviation_force = core::bc_vector3f(0)) noexcept;

			bc_particle_builder1& with_particles_color(const core::bc_vector3f& p_color, bcFLOAT p_intensity = 1);
			
			bc_particle_builder1& with_particles_reverse_direction() noexcept;

			bc_particle_builder1& with_particle_size(bcFLOAT p_start_size, bcFLOAT p_end_size) noexcept;

			bc_particle_builder1& with_particles_rotation(bcFLOAT p_degree_per_second) noexcept;

			bc_particle_builder1& with_particle_velocity_curve(const bc_particle_curve& p_curve, bcFLOAT p_curve_duration_second) noexcept;
			
			bc_particle_builder1& with_particle_size_curve(const bc_particle_curve& p_curve) noexcept;
			
			bc_particle_builder1& with_particle_fade_curve(const bc_particle_curve& p_curve) noexcept;

			bc_particle_builder& emit_particles(bcUINT32 p_particles_total_count,
				bcFLOAT p_particles_lifetime,
				bcFLOAT p_particles_force,
				bcFLOAT p_particles_mass) noexcept;

		private:
			bc_particle_builder& m_builder;
			bc_particle_emitter_trait& m_emitter;
		};

		class BC_GAME_DLL bc_particle_builder
		{
			friend class bc_particle_builder1;
			friend class bc_particle_manager;
			friend class bc_particle_manager_container;
			
		public:
			const static bc_particle_curve s_curve_constant;
			const static bc_particle_curve s_curve_slow_step5;
			const static bc_particle_curve s_curve_slow_step4;
			const static bc_particle_curve s_curve_slow_step3;
			const static bc_particle_curve s_curve_slow_step2;
			const static bc_particle_curve s_curve_slow_step1;
			const static bc_particle_curve s_curve_linear;
			const static bc_particle_curve s_curve_fast_step1;
			const static bc_particle_curve s_curve_fast_step2;
			const static bc_particle_curve s_curve_fast_step3;
			const static bc_particle_curve s_curve_fast_step4;
			const static bc_particle_curve s_curve_fast_step5;
			const static core::bc_array<const bc_particle_curve*, 12> s_curves;
			
		public:
			bc_particle_builder();

			bc_particle_builder1 emitter(const core::bc_vector3f& p_position, const core::bc_vector3f& p_direction) noexcept;
			
			bc_particle_builder1 emitter(const core::bc_vector3f& p_position, 
				const core::bc_vector3f& p_direction,
				bcFLOAT p_lifetime_seconds,
				bcFLOAT p_force,
				bcFLOAT p_mass) noexcept;

			bc_particle_builder1 duplicate_last(const core::bc_vector3f& p_position, const core::bc_vector3f& p_direction) noexcept;

			bc_particle_builder1 duplicate_last(const core::bc_vector3f& p_position,
				const core::bc_vector3f& p_direction,
				bcFLOAT p_lifetime_seconds,
				bcFLOAT p_force,
				bcFLOAT p_mass) noexcept;

		private:
			static decltype(s_curves)::size_type _find_curve_index(const bc_particle_curve& p_curve) noexcept;
			
			core::bc_vector_frame<bc_particle_emitter_trait> m_emitters;
		};
	}
}