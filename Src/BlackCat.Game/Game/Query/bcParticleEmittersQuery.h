// [11/16/2020 MRB]

#pragma once

#include "Core/bcConstant.h"
#include "Core/Container/bcVector.h"
#include "Core/Messaging/Query/bcQuery.h"
#include "Game/System/Render/Particle/bcParticleEmitter.h"

namespace black_cat
{
	namespace game
	{
		class bc_particle_emitters_query_context : public core::bc_query_context
		{
		public:
			explicit bc_particle_emitters_query_context(core::bc_vector_movable<bc_particle_emitter> p_emitters)
				: m_emitters(std::move(p_emitters))
			{
			}

			core::bc_vector_movable<bc_particle_emitter> m_emitters;
		};
		
		class bc_particle_emitter_query : public core::bc_query<bc_particle_emitters_query_context>
		{
			BC_QUERY(ptc_emt)
			
		public:
			explicit bc_particle_emitter_query()
				: bc_query(message_name())
			{
			}

			bc_particle_emitter_query(bc_particle_emitter_query&&) = default;

			~bc_particle_emitter_query() = default;

			bc_particle_emitter_query& operator=(bc_particle_emitter_query&&) = default;

			core::bc_vector_movable<bc_particle_emitter> get_emitters() noexcept
			{
				return std::move(m_emitters);
			}
			
			void execute(const bc_particle_emitters_query_context& p_context) noexcept override
			{
				m_emitters = p_context.m_emitters;
			}
			
		private:
			core::bc_vector_movable<bc_particle_emitter> m_emitters;
		};
	}
}