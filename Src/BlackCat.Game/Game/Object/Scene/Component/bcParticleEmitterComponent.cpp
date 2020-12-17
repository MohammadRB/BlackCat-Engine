// [12/16/2020 MRB]

#include "Game/GamePCH.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/Particle/bcParticleManager.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcParticleEmitterComponent.h"
#include "Game/Object/Scene/Component/Event/bcActorEventWorldTransform.h"

namespace black_cat
{
	namespace game
	{
		inline bc_particle_emitter_component::bc_particle_emitter_component(bc_actor_component_index p_index)
			: bc_iactor_component(p_index)
		{
		}

		inline bc_actor bc_particle_emitter_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		bc_external_particle_emitter* bc_particle_emitter_component::get_emitters() noexcept
		{
			return m_emitter.get();
		}

		inline void bc_particle_emitter_component::initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters)
		{
			const auto* l_emitter_name = p_parameters.get_value<core::bc_string>("emitter_name");
			if(l_emitter_name != nullptr)
			{
				m_emitter_name = *l_emitter_name; // TODO what to do with emitter name?
			}
		}

		inline void bc_particle_emitter_component::spawn_emitters(const bc_particle_builder& p_builder)
		{
			m_emitter = core::bc_get_service<bc_game_system>()->get_render_system().get_particle_manager().add_emitter(p_builder);
		}

		void bc_particle_emitter_component::handle_event(bc_actor& p_actor, const bc_actor_event& p_event)
		{
			auto* l_world_transform_event = core::bc_imessage::as<bc_actor_event_world_transform>(p_event);
			if (l_world_transform_event)
			{
				m_emitter->set_positions(l_world_transform_event->get_transform().get_translation());
			}
		}
	}	
}