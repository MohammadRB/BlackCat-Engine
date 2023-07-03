// [19/03/2021 MRB]

#include "Editor/EditorPCH.h"

#include "Game/Object/Scene/bcScene.h"
#include "Game/System/bcGameSystem.h"
#include "Editor/UICommand/bcObjectDeleteUICommand.h"
#include "Editor/UI/bcFormObject.h"

namespace black_cat
{
	namespace editor
	{
		bc_object_delete_ui_command::bc_object_delete_ui_command(game::bc_actor p_actor) noexcept
			: m_actor(p_actor)
		{
		}

		bc_object_delete_ui_command::bc_object_delete_ui_command(const bc_object_delete_ui_command&) noexcept = default;

		bc_object_delete_ui_command::~bc_object_delete_ui_command() = default;

		bc_object_delete_ui_command& bc_object_delete_ui_command::operator=(const bc_object_delete_ui_command&) noexcept = default;

		core::bc_string bc_object_delete_ui_command::title() const
		{
			return "ObjectDelete";
		}

		bci_ui_command::state_ptr bc_object_delete_ui_command::create_state(state_context& p_context) const
		{
			return nullptr;
		}

		bool bc_object_delete_ui_command::update(update_context& p_context)
		{
			if (!p_context.m_game_system.get_scene())
			{
				return false;
			}

			p_context.m_game_system.get_scene()->remove_actor(m_actor);
			return false;
		}

		void bc_object_delete_ui_command::update_ui(update_ui_context& p_context)
		{
			p_context.m_form_object.setSelectedActor(game::bc_actor(), core::bc_matrix4f::identity());
		}
	}
}