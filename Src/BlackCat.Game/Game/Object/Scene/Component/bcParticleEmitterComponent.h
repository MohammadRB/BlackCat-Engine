// [16/12/2020 MRB]

#pragma once

#include "Game/bcExport.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/System/Render/Particle/bcParticleEmitter.h"
#include "Game/System/Render/Particle/bcParticleBuilder.h"

namespace black_cat
{
	namespace game
	{
		class bc_scene;

		class BC_GAME_DLL bc_particle_emitter_component : public bci_actor_component
		{
			BC_COMPONENT("prtle", true, false)

		public:
			bc_particle_emitter_component(bc_actor_id p_actor_id, bc_actor_component_id p_id) noexcept;

			bc_particle_emitter_component(bc_particle_emitter_component&& p_other) noexcept = default;

			~bc_particle_emitter_component() override = default;

			bc_particle_emitter_component& operator=(bc_particle_emitter_component&& p_other) noexcept = default;
			
			bc_actor get_actor() const noexcept override;

			bc_external_particle_emitter* get_emitter() noexcept;
			
			void add_emitter(const bcCHAR* p_emitter, 
				const core::bc_vector3f& p_position, 
				const core::bc_vector3f& p_dir = core::bc_vector3f::up(),
				const core::bc_vector3f* p_color = nullptr);

			void add_emitter(const bc_particle_builder& p_builder, 
				const core::bc_vector3f& p_position, 
				const core::bc_vector3f& p_dir = core::bc_vector3f::up(),
				const core::bc_vector3f* p_color = nullptr);
			
			void initialize(const bc_actor_component_initialize_context& p_context) override;

			void handle_event(const bc_actor_component_event_context& p_context) override;
			
		private:
			bc_scene* m_scene;
			bc_particle_emitter_ptr m_emitter;
		};
	}
}