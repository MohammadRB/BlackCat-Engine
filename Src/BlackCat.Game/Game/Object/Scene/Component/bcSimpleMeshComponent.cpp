// [09/09/2019 MRB]

#include "Game/GamePCH.h"

#include "Game/Object/Scene/ActorComponent/bcActorComponentManager.h"
#include "Game/Object/Scene/Component/bcSimpleMeshComponent.h"
#include "Game/Object/Scene/Component/Event/bcActorEventWorldTransform.h"

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

		void bc_simple_mesh_component::handle_event(bc_actor_component_event_context& p_context)
		{
			auto* l_world_transform_event = core::bci_message::as<bc_actor_event_world_transform>(p_context.m_event);
			if(l_world_transform_event)
			{
				bc_mesh_component::set_world_transform(p_context.m_actor, l_world_transform_event->get_transform());
			}
		}
	}
}