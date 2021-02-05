// [06/07/2016 MRB]

#include "Game/GamePCH.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/Component/bcRenderComponent.h"

namespace black_cat
{
	namespace game
	{
		bc_render_component::bc_render_component(bc_actor_index p_actor_index, bc_actor_component_index p_index)
			: bci_actor_component(p_actor_index, p_index)
		{
		}

		bc_render_component::bc_render_component(bc_render_component&& p_other) noexcept
			: bci_actor_component(std::move(p_other))
		{
		}

		bc_render_component::~bc_render_component()
		{	
		}

		bc_render_component& bc_render_component::operator=(bc_render_component&& p_other) noexcept
		{
			bci_actor_component::operator=(std::move(p_other));

			return *this;
		}
	}
}