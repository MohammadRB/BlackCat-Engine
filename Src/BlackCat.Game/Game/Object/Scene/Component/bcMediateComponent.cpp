// [12/22/2018 MRB]

#include "Game/GamePCH.h"
#include "Game/Object/Scene/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"

namespace black_cat
{
	namespace game
	{
		bc_mediate_component::bc_mediate_component(bc_actor_component_index p_index)
			: bc_iactor_component(p_index)
		{
		}

		bc_actor bc_mediate_component::get_actor() const noexcept
		{
			return get_manager()->component_get_actor(*this);
		}

		void bc_mediate_component::initialize(bc_actor& p_actor, const core::bc_data_driven_parameter& p_parameters)
		{
		}

		void bc_mediate_component::update(const bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock_update_param)
		{
		}
	}
}