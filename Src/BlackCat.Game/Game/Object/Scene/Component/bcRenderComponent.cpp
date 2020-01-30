// [06/07/2016 MRB]

#include "Game/GamePCH.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/Component/bcRenderComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"

namespace black_cat
{
	namespace game
	{
		bc_render_component::bc_render_component(bc_actor_component_index p_index)
			: bc_iactor_component(p_index)
		{
		}

		bc_render_component::bc_render_component(bc_render_component&& p_other) noexcept
			: bc_iactor_component(std::move(p_other))
		{
		}

		bc_render_component::~bc_render_component()
		{	
		}

		bc_render_component& bc_render_component::operator=(bc_render_component&& p_other) noexcept
		{
			bc_iactor_component::operator=(std::move(p_other));

			return *this;
		}
	}
}