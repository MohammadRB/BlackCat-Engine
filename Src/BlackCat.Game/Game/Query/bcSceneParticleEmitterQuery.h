// [11/16/2020 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Messaging/Query/bcQuery.h"
#include "Game/Query/bcQueryContext.h"
#include "Game/System/Render/Particle/bcParticleEmitter.h"
#include "Game/Object/Scene/bcScene.h"

namespace black_cat
{
	namespace game
	{
		class bc_scene_particle_emitter_query : public core::bc_query<bc_scene_query_context>
		{
			BC_QUERY(ptc_emt)
			
		public:
			bc_scene_particle_emitter_query()
				: bc_query(message_name())
			{
			}

			bc_scene_particle_emitter_query(bc_scene_particle_emitter_query&&) = default;

			~bc_scene_particle_emitter_query() override = default;

			bc_scene_particle_emitter_query& operator=(bc_scene_particle_emitter_query&&) = default;

			core::bc_vector_movable<bc_particle_emitter_state> get_emitters() noexcept
			{
				return std::move(m_emitters);
			}

		protected:
			void execute(const bc_scene_query_context& p_context) noexcept override
			{
				if (!p_context.m_scene)
				{
					return;
				}

				m_emitters = p_context.m_scene->get_particle_manager().get_emitter_states();
			}
			
		private:
			core::bc_vector_movable<bc_particle_emitter_state> m_emitters;
		};
	}
}