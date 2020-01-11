// [01/19/2017 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/ActorComponent/bcActorComponent.h"
#include "bcActorComponent.h"

namespace black_cat
{
	namespace game
	{
		void bc_iactor_component::load_instance(bc_actor& p_actor, const core::bc_json_key_value& p_parameters)
		{
		}

		void bc_iactor_component::write_instance(bc_actor& p_actor, core::bc_json_key_value& p_parameters)
		{
		}

		void bc_iactor_component::handle_event(const bc_actor& p_actor, const bc_actor_event& p_event)
		{
		}

		void bc_iactor_component::update(const bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock_update_param)
		{
		}

		bc_actor_component_manager& bc_iactor_component::get_manager() noexcept
		{
			static bc_actor_component_manager* s_manager = core::bc_get_service<bc_actor_component_manager>();
			return *s_manager;
		}
	}
}