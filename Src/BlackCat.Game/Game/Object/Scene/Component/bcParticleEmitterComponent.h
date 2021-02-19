// [12/16/2020 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Container/bcString.h"
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
			bc_particle_emitter_component(bc_actor_index p_actor_index, bc_actor_component_index p_index);

			bc_particle_emitter_component(bc_particle_emitter_component&& p_other) noexcept = default;

			~bc_particle_emitter_component() = default;

			bc_particle_emitter_component& operator=(bc_particle_emitter_component&& p_other) noexcept = default;
			
			bc_actor get_actor() const noexcept override;

			bc_external_particle_emitter* get_emitters() noexcept;

			void spawn_emitter(const core::bc_vector3f& p_pos, const core::bc_vector3f& p_dir, const core::bc_vector3f* p_color = nullptr);

			void spawn_emitter(const bcCHAR* p_name, const core::bc_vector3f& p_pos, const core::bc_vector3f& p_dir, const core::bc_vector3f* p_color = nullptr);

			void add_emitter(const bc_particle_builder& p_builder);
			
			void initialize(const bc_actor_component_initialize_context& p_context) override;

			void handle_event(const bc_actor_component_event_context& p_context) override;
			
		private:
			core::bc_string m_emitter_name;
			bc_particle_emitter_ptr m_emitter;
		};
	}
}