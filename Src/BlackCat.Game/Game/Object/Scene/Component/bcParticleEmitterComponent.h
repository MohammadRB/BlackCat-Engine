// [12/16/2020 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Game/bcExport.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/System/Render/Particle/bcParticleEmitter.h"
#include "Game/System/Render/Particle/bcParticleBuilder.h"

namespace black_cat
{
	namespace game
	{
		class BC_GAME_DLL bc_particle_emitter_component : public bci_actor_component
		{
			BC_COMPONENT("prtle", true, false)

		public:
			bc_particle_emitter_component(bc_actor_id p_actor_index, bc_actor_component_id p_index) noexcept;

			bc_particle_emitter_component(bc_particle_emitter_component&& p_other) noexcept = default;

			~bc_particle_emitter_component() override = default;

			bc_particle_emitter_component& operator=(bc_particle_emitter_component&& p_other) noexcept = default;
			
			bc_actor get_actor() const noexcept override;

			bc_external_particle_emitter* get_emitter() noexcept;

			void add_emitter(const bc_particle_builder& p_builder);
			
			void initialize(const bc_actor_component_initialize_context& p_context) override;

			void handle_event(const bc_actor_component_event_context& p_context) override;
			
		private:
			bc_particle_emitter_ptr m_emitter;
		};
	}
}