// [22/04/2021 MRB]

#include "Editor/EditorPCH.h"

#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Editor/UICommand/bcObjectUpdateUICommand.h"
#include "Editor/UI/bcFormObject.h"

namespace black_cat
{
	namespace editor
	{
		bc_object_update_ui_command::bc_object_update_ui_command(game::bc_actor p_actor, const core::bc_matrix4f& p_transform) noexcept
			: m_actor(p_actor),
			m_transform(p_transform)
		{
		}

		bc_object_update_ui_command::bc_object_update_ui_command(const bc_object_update_ui_command&) noexcept = default;

		bc_object_update_ui_command::~bc_object_update_ui_command() = default;

		bc_object_update_ui_command& bc_object_update_ui_command::operator=(const bc_object_update_ui_command&) noexcept = default;

		core::bc_string bc_object_update_ui_command::title() const
		{
			return "ObjectUpdate";
		}

		bci_ui_command::state_ptr bc_object_update_ui_command::create_state(state_context& p_context) const
		{
			return nullptr;
		}

		bool bc_object_update_ui_command::update(update_context& p_context)
		{
			m_actor.add_event(game::bc_world_transform_actor_event(m_transform));
			return false;
		}
	}
}