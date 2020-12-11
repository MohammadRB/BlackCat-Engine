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
#include "Core/Messaging/Query/bcQueryProviderHandle.h"
#include "Core/Messaging/Query/bcQueryContext.h"
#include "Game/System/Render/Particle/bcParticleBuilder.h"
#include "Game/bcExport.h"

namespace black_cat
{
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
			using particle_definition_container = core::bc_unordered_map_program<const bcCHAR*, core::bc_vector_program<bc_particle_emitter_trait>>;
			using emitters_container = core::bc_list_pool<_bc_particle_emitter_instance>;

		public:
			static constexpr bcSIZE curve_sample_count = 10;
			static constexpr bcSIZE curve_count = bc_particle_builder::s_curves.size();
			using curve_sample_container = core::bc_array<bcFLOAT, curve_sample_count>;
			using curves_container = core::bc_array<curve_sample_container, curve_count>;
			
		public:
			bc_particle_manager();

			bc_particle_manager(bc_particle_manager&&) noexcept;

			~bc_particle_manager();

			bc_particle_manager& operator=(bc_particle_manager&&) noexcept;

			const curves_container& get_curves() const noexcept;
			
			void register_emitter_definition(const bcCHAR* p_name, const bc_particle_builder& p_builder);

			void spawn_emitter(const bcCHAR* p_emitter_name, const core::bc_vector3f& p_pos, const core::bc_vector3f& p_dir);
			
			void update(const core_platform::bc_clock::update_param& p_clock);
			
		private:
			core::bc_query_context_ptr _emitters_query_context_provider() const;

			bcFLOAT _sample_curve(bcSIZE p_curve_index, bcFLOAT p_normalized_time) const;
			
			constexpr static bcSIZE s_emitter_count = 300;

			curves_container m_curves{};
			particle_definition_container m_emitter_definitions;

			mutable core_platform::bc_hybrid_mutex m_emitters_lock;
			emitters_container m_emitters;

			core::bc_query_provider_handle m_emitters_provider_handle;
		};
	}
}