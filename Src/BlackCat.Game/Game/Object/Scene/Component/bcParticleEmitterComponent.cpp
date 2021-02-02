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
			: bci_actor_component(p_index)
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

		void bc_particle_emitter_component::spawn_emitter()
		{
			if (!m_emitter_name.empty())
			{
				const auto l_position = get_actor().get_component<bc_mediate_component>()->get_position();
				core::bc_get_service<bc_game_system>()->get_render_system()
					.get_particle_manager()
					.spawn_emitter(m_emitter_name.c_str(), l_position, core::bc_vector3f::up());
			}
		}

		void bc_particle_emitter_component::spawn_emitter(const bcCHAR* p_name)
		{
			const auto l_position = get_actor().get_component<bc_mediate_component>()->get_position();
			core::bc_get_service<bc_game_system>()->get_render_system()
				.get_particle_manager()
				.spawn_emitter(p_name, l_position, core::bc_vector3f::up());
		}

		void bc_particle_emitter_component::add_emitter(const bc_particle_builder& p_builder)
		{
			m_emitter = core::bc_get_service<bc_game_system>()->get_render_system().get_particle_manager().add_emitter(p_builder);
		}
		
		void bc_particle_emitter_component::initialize(const bc_actor_component_initialize_context& p_context)
		{
			const auto* l_emitter_name = p_context.m_parameters.get_value<core::bc_string>("emitter_name");
			if(l_emitter_name != nullptr)
			{
				m_emitter_name = *l_emitter_name;
			}
		}

		void bc_particle_emitter_component::handle_event(const bc_actor_component_event_context& p_context)
		{
			if(m_emitter)
			{
				const auto* l_world_transform_event = core::bci_message::as<bc_actor_event_world_transform>(p_context.m_event);
				if (l_world_transform_event)
				{
					m_emitter->set_positions(l_world_transform_event->get_transform().get_translation());
				}
			}
		}
	}	
}