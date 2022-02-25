// [02/24/2022 MRB]

#include "Game/GamePCH.h"

#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "Game/Object/Scene/Component/bcCheckPointComponent.h"

namespace black_cat
{
	namespace game
	{
		void bc_actor_load_instance(core::bc_vector_frame<bci_actor_component*>& p_buffer, const bc_actor_component_load_context& p_context)
		{
			p_buffer.clear();
			p_context.m_actor.get_components(std::back_inserter(p_buffer));

			for (auto* l_component : p_buffer)
			{
				l_component->load_instance(p_context);
			}
		}

		void bc_actor_write_instance(core::bc_vector_frame<bci_actor_component*>& p_buffer, const bc_actor_component_write_context& p_context)
		{
			p_buffer.clear();
			p_context.m_actor.get_components(std::back_inserter(p_buffer));

			for (auto* l_component : p_buffer)
			{
				l_component->write_instance(p_context);
			}
		}

		void bc_actor_load_network_instance(core::bc_vector_frame<bci_actor_component*>& p_buffer, const bc_actor_component_network_load_context& p_context)
		{
			p_buffer.clear();
			p_context.m_actor.get_components(std::back_inserter(p_buffer));

			for (auto* l_component : p_buffer)
			{
				l_component->load_network_instance(p_context);
			}
		}

		void bc_actor_write_network_instance(core::bc_vector_frame<bci_actor_component*>& p_buffer, const bc_actor_component_network_write_context& p_context)
		{
			p_buffer.clear();
			p_context.m_actor.get_components(std::back_inserter(p_buffer));

			for (auto* l_component : p_buffer)
			{
				l_component->write_network_instance(p_context);
			}
		}

		void bc_actor_mark_for_double_update(bc_actor& p_actor)
		{
			p_actor.mark_for_double_update();
		}

		void bc_actor_mark_for_checkpoint(bc_actor& p_actor)
		{
			p_actor.get_create_component<bc_checkpoint_component>();
		}

		void bc_actor_unmark_for_checkpoint(bc_actor& p_actor)
		{
			if (p_actor.has_component<bc_checkpoint_component>())
			{
				p_actor.remove_component<bc_checkpoint_component>();
			}
		}
	}
}
