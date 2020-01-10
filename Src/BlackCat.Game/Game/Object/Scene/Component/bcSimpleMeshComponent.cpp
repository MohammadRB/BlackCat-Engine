// [09/09/2019 MRB]

#include "Game/GamePCH.h"

#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcSimpleMeshComponent.h"

namespace black_cat
{
	namespace game
	{
		bc_simple_mesh_component::bc_simple_mesh_component(bc_actor_component_index p_index)
			: bc_mesh_component(p_index)
		{
		}

		bc_simple_mesh_component::bc_simple_mesh_component(bc_simple_mesh_component&& p_other) noexcept
			: bc_mesh_component(std::move(p_other))
		{
		}

		bc_simple_mesh_component::~bc_simple_mesh_component()
		{
		}

		bc_simple_mesh_component& bc_simple_mesh_component::operator=(bc_simple_mesh_component&& p_other) noexcept
		{
			bc_mesh_component::operator=(std::move(p_other));
			return *this;
		}

		bc_actor bc_simple_mesh_component::get_actor() const noexcept
		{
			return get_manager().component_get_actor(*this);
		}

		void bc_simple_mesh_component::update(const bc_actor& p_actor, const core_platform::bc_clock::update_param& p_clock_update_param)
		{
		}

		void bc_simple_mesh_component::handle_event(const bc_actor& p_actor, const bc_actor_event& p_event)
		{
		}
	}
}