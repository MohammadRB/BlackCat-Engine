// [12/16/2020 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Render/Particle/bcParticleEmitter.h"
#include "Game/System/Render/Particle/bcParticleManager.h"

namespace black_cat
{
	namespace game
	{
		_bc_particle_emitter_ptr_deleter::_bc_particle_emitter_ptr_deleter()
			: m_manager(nullptr)
		{
		}

		_bc_particle_emitter_ptr_deleter::_bc_particle_emitter_ptr_deleter(bc_particle_manager* p_manager)
			: m_manager(p_manager)
		{
		}

		void _bc_particle_emitter_ptr_deleter::operator()(bc_external_particle_emitter* p_emitter)
		{
			m_manager->_destroy_emitter(p_emitter);
		}
	}	
}