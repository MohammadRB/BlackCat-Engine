// [12/16/2020 MRB]

#include "Game/GamePCH.h"
#include "Game/System/bcGameSystem.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/Particle/bcParticleManager.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcParticleEmitterComponent.h"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Game/bcConstant.h"

namespace black_cat
{
	namespace game
	{
		bc_particle_emitter_component::bc_particle_emitter_component(bc_actor_id p_actor_index, bc_actor_component_id p_index) noexcept
			: bci_actor_component(p_actor_index, p_index)
		{
		}

		bc_actor bc_particle_emitter_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		bc_external_particle_emitter* bc_particle_emitter_component::get_emitter() noexcept
		{
			return m_emitter.get();
		}

		void bc_particle_emitter_component::add_emitter(const bc_particle_builder& p_builder)
		{
			m_emitter = get_actor().get_component<bc_mediate_component>()->get_scene()->get_particle_manager().add_emitter(p_builder);
		}
		
		void bc_particle_emitter_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
		}

		void bc_particle_emitter_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			if(m_emitter)
			{
				const auto* l_world_transform_event = core::bci_message::as<bc_world_transform_actor_event>(p_context.m_event);
				if (l_world_transform_event)
				{
					m_emitter->set_positions(l_world_transform_event->get_transform().get_translation());
				}
			}
		}
	}	
}