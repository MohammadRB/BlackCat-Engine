// [11/11/2020 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Math/bcCurve.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Container/bcArray.h"
#include "Core/Container/bcVector.h"
#include "Core/Container/bcListPool.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Utility/bcRandom.h"
#include "Core/Utility/bcNullable.h"
#include "Game/System/Render/Particle/bcParticleBuilder.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace core
	{
		class bc_content_stream_manager;
	}
	
	namespace game
	{
		struct _bc_particle_emitter_instance : bc_particle_emitter_trait
		{
			_bc_particle_emitter_instance(const bc_particle_emitter_trait& p_trait);

			core::bc_vector3f m_prev_position;
			bcFLOAT m_age;
			bcFLOAT m_energy;
			bcUINT32 m_spawned_particles_count;
			bcUINT32 m_particles_count_to_spawn;
		};
		
		/**
		 * \brief
		 * \b ThreadSafe
		 */
		class BC_GAME_DLL bc_particle_manager
		{
		private:
			using string_hash_t = std::hash<const bcCHAR*>;
			using particle_definition_container = core::bc_unordered_map_program<string_hash_t::result_type, core::bc_vector_program<bc_particle_emitter_trait>>;
			using emitters_container = core::bc_list_pool<_bc_particle_emitter_instance>;
			using external_emitters_container = core::bc_list_pool<bc_external_particle_emitter>;
			
		public:
			static constexpr bcSIZE s_curve_sample_count = 10;
			static constexpr bcSIZE s_curve_count = bc_particle_builder::s_curves.size();
			using curve_sample_container = core::bc_array<bcFLOAT, s_curve_sample_count>;
			using curves_container = core::bc_array<curve_sample_container, s_curve_count>;
			
		public:
			bc_particle_manager();

			bc_particle_manager(bc_particle_manager&&) noexcept;

			~bc_particle_manager();

			bc_particle_manager& operator=(bc_particle_manager&&) noexcept;

			static const curves_container& get_curves() noexcept;

			void spawn_emitter(const bcCHAR* p_emitter_name, 
				const core::bc_vector3f& p_pos, 
				const core::bc_vector3f& p_dir, 
				const core::bc_vector3f* p_color = nullptr,
				bcFLOAT p_scale = 1);

			bc_particle_emitter_ptr add_emitter(const bc_particle_builder& p_builder);
			
			void update(const core_platform::bc_clock::update_param& p_clock);

			core::bc_vector_movable<bc_particle_emitter_state> get_emitter_states() const noexcept;

			static void init_emitter_states();
			
			static void register_emitter_definition(const bcCHAR* p_name, const bc_particle_builder& p_builder);
			
			static void clear_emitter_states();
			
			void _destroy_emitter(bc_external_particle_emitter* p_emitter);

		private:
			void _apply_emitter_scale(bc_particle_emitter_trait& p_emitter, bcFLOAT p_scale) const noexcept;
			
			bcFLOAT _sample_curve(bcSIZE p_curve_index, bcFLOAT p_normalized_time) const noexcept;
			
			constexpr static bcSIZE s_emitters_pool_size = 300;

			static curves_container m_curves;
			// Use nullable to postpone initialization to the time when memory manager is initialized
			static core::bc_nullable<particle_definition_container> m_emitter_definitions;
			
			core::bc_random m_random;
			mutable core_platform::bc_hybrid_mutex m_emitters_lock;
			emitters_container m_emitters;
			external_emitters_container m_external_emitters;
		};
	}
}