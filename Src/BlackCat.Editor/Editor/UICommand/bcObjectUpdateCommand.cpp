// [04/22/2021 MRB]

#include "Editor/EditorPCH.h"

#include "Game/Object/Scene/Component/Event/bcWorldTransformActorEvent.h"
#include "Editor/UICommand/bcObjectUpdateCommand.h"
#include "Editor/UI/bcFormObject.h"

namespace black_cat
{
	namespace editor
	{
		bc_ui_object_update_command::bc_ui_object_update_command(game::bc_actor p_actor, const core::bc_matrix4f& p_transform) noexcept
			: m_actor(p_actor),
			m_transform(p_transform)
		{
		}

		bc_ui_object_update_command::bc_ui_object_update_command(const bc_ui_object_update_command&) noexcept = default;

		bc_ui_object_update_command::~bc_ui_object_update_command() = default;

		bc_ui_object_update_command& bc_ui_object_update_command::operator=(const bc_ui_object_update_command&) noexcept = default;

		core::bc_string bc_ui_object_update_command::title() const
		{
			return "ObjectUpdate";
		}

		bc_iui_command::state_ptr bc_ui_object_update_command::create_state(state_context& p_context) const
		{
			return nullptr;
		}

		bool bc_ui_object_update_command::update(update_context& p_context)
		{
			m_actor.add_event(game::bc_world_transform_actor_event(m_transform));
			return false;
		}
	}
}