// [12/16/2020 MRB]

#pragma once

#include "Core/Container/bcString.h"
#include "Game/bcExport.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/System/Render/Particle/bcParticleEmitter.h"
#include "Game/System/Render/Particle/bcParticleBuilder.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_particle_emitter_component : public bc_iactor_component
		{
			BC_COMPONENT("prtle", true, false)

		public:
			explicit bc_particle_emitter_component(bc_actor_component_index p_index);

			bc_particle_emitter_component(bc_particle_emitter_component&& p_other) noexcept = default;

			~bc_particle_emitter_component() = default;

			bc_particle_emitter_component& operator=(bc_particle_emitter_component&& p_other) noexcept = default;
			
			bc_actor get_actor() const noexcept override;

			bc_external_particle_emitter* get_emitters() noexcept;
			
			void initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters) override;

			void spawn_emitters(const bc_particle_builder& p_builder);

			void handle_event(bc_actor& p_actor, const bc_actor_event& p_event) override;
			
		private:
			core::bc_string m_emitter_name;
			bc_particle_emitter_ptr m_emitter;
		};
	}
}