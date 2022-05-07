// [02/24/2022 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/bcScene.h"
#include "Game/Object/Scene/Component/bcCallbackComponent.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManagerContainer.h"

namespace black_cat
{
	namespace game
	{
		bc_callback_component::bc_callback_component(bc_actor_id p_actor_id, bc_actor_component_id p_id)
			: bci_actor_component(p_actor_id, p_id),
			m_call_mode(call_mode::once),
			m_delay_ms(0),
			m_current_delay(0)
		{
		}

		bc_actor bc_callback_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_callback_component::update(const bc_actor_component_update_content& p_context)
		{
			if(!m_callback.is_valid())
			{
				return;
			}

			m_current_delay += p_context.m_clock.m_elapsed;
			if(m_current_delay >= m_delay_ms)
			{
				m_callback(p_context.m_actor);

				if(m_call_mode == call_mode::once)
				{
					m_callback.reset();
				}
			}
		}
		
		void bc_callback_component::set_callback(call_mode p_call_mode, bcFLOAT p_delay_ms, callback p_callback) noexcept
		{
			m_call_mode = p_call_mode;
			m_delay_ms = p_delay_ms;
			m_callback = std::move(p_callback);
		}

		void bc_callback_component::set_as_auto_remove(bcFLOAT p_delay_ms)
		{
			set_callback
			(
				call_mode::once,
				p_delay_ms,
				[=](const game::bc_actor& p_actor)
				{
					auto* l_mediate_component = p_actor.get_component<bc_mediate_component>();
					auto* l_scene = l_mediate_component->get_scene();
					l_scene->remove_actor(p_actor);
				}
			);
		}
	}
}
