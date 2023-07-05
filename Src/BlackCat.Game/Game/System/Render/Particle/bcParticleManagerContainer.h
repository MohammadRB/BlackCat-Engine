// [03/03/2022 MRB]

#pragma once

#include "CorePlatformImp/Concurrency/bcMutex.h"
#include "CorePlatformImp/Utility/bcClock.h"
#include "Core/Container/bcListPool.h"
#include "Core/Container/bcSpan.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Utility/bcRandom.h"
#include "Game/System/Render/Particle/bcParticleEmitter.h"
#include "Game/System/Render/Particle/bcParticleBuilder.h"
#include "Game/bcExport.h"

namespace black_cat::game
{
	class bc_particle_manager;

	struct _bc_particle_emitter_instance : bc_particle_emitter_trait
	{
		_bc_particle_emitter_instance(const bc_particle_emitter_trait& p_trait) noexcept;

		void update_particles_count(const platform::bc_clock::update_param& p_clock, bcFLOAT p_spawn_energy) noexcept;

		core::bc_vector3f m_prev_position;
		bcFLOAT m_age;
		bcFLOAT m_energy;
		bcUINT16 m_particles_count_to_spawn;

		// For per second count model
		bcUINT16 m_milliseconds_to_spawn;
		bcUINT16 m_milliseconds_since_last_spawn;
		// for total count model
		bcUINT16 m_spawned_particles_count;
	};

	class BC_GAME_DLL bc_particle_manager_container
	{
	public:
		using emitters_container = core::bc_list_pool<_bc_particle_emitter_instance>;
		using external_emitters_container = core::bc_list_pool<bc_external_particle_emitter>;

	public:
		explicit bc_particle_manager_container(const bc_particle_manager& p_manager);

		bc_particle_manager_container(bc_particle_manager_container&&) noexcept;

		~bc_particle_manager_container() = default;

		bc_particle_manager_container& operator=(bc_particle_manager_container&&) noexcept;

		/**
			 * \brief spawn and forget particle emitters
			 * \param p_emitter_name 
			 * \param p_pos 
			 * \param p_dir 
			 * \param p_color 
			 * \param p_scale 
			 */
		void spawn_emitter(const bcCHAR* p_emitter_name,
		                   const core::bc_vector3f& p_pos,
		                   const core::bc_vector3f& p_dir,
		                   const core::bc_vector3f* p_color = nullptr,
		                   bcFLOAT p_scale = 1);

		/**
			 * \brief spawn particle emitters but mark them as external controlled emitters with strong reference to them
			 * \param p_emitter_name 
			 * \param p_pos 
			 * \param p_dir 
			 * \param p_color 
			 * \param p_scale 
			 * \return 
			 */
		bc_particle_emitter_ptr add_emitter(const bcCHAR* p_emitter_name,
		                                    const core::bc_vector3f& p_pos,
		                                    const core::bc_vector3f& p_dir,
		                                    const core::bc_vector3f* p_color = nullptr,
		                                    bcFLOAT p_scale = 1);

		bc_particle_emitter_ptr add_emitter(const bc_particle_builder& p_builder,
		                                    const core::bc_vector3f& p_pos,
		                                    const core::bc_vector3f& p_dir = core::bc_vector3f::up(),
		                                    const core::bc_vector3f* p_color = nullptr,
		                                    bcFLOAT p_scale = 1);

		void update(const platform::bc_clock::update_param& p_clock);

		core::bc_vector_movable<bc_particle_emitter_state> get_emitter_states() const noexcept;

		void _destroy_emitter(bc_external_particle_emitter* p_emitter);

	private:
		bc_particle_emitter_ptr _add_emitters(core::bc_const_span<bc_particle_emitter_trait> p_emitters,
		                                      const core::bc_vector3f& p_pos,
		                                      const core::bc_vector3f& p_dir,
		                                      const core::bc_vector3f* p_color = nullptr,
		                                      bcFLOAT p_scale = 1);

		void _apply_emitter_scale(bc_particle_emitter_trait& p_emitter, bcFLOAT p_scale) const noexcept;

		constexpr static bcSIZE s_emitters_pool_size = 300;

		const bc_particle_manager* m_manager;
		core::bc_random m_random;

		mutable platform::bc_hybrid_mutex m_emitters_lock;
		emitters_container m_emitters;
		external_emitters_container m_external_emitters;
	};
}
