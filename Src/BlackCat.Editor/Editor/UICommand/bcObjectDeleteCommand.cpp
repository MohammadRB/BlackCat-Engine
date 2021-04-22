// [03/19/2021 MRB]

#include "Editor/EditorPCH.h"
#include "Editor/UICommand/bcObjectDeleteCommand.h"
#include "Editor/UI/bcFormObject.h"

namespace black_cat
{
	namespace editor
	{
		bc_ui_object_delete_command::bc_ui_object_delete_command(game::bc_actor p_actor) noexcept
			: m_actor(p_actor)
		{
		}

		bc_ui_object_delete_command::bc_ui_object_delete_command(const bc_ui_object_delete_command&) noexcept = default;

		bc_ui_object_delete_command::~bc_ui_object_delete_command() = default;

		bc_ui_object_delete_command& bc_ui_object_delete_command::operator=(const bc_ui_object_delete_command&) noexcept = default;

		core::bc_string bc_ui_object_delete_command::title() const
		{
			return "ObjectDelete";
		}

		bc_iui_command::state_ptr bc_ui_object_delete_command::create_state(state_context& p_context) const
		{
			return nullptr;
		}

		bool bc_ui_object_delete_command::update(update_context& p_context)
		{
			p_context.m_game_system.get_scene()->remove_actor(m_actor);
			return false;
		}

		void bc_ui_object_delete_command::update_ui(update_ui_context& p_context)
		{
			p_context.m_form_object.setSelectedActor(game::bc_actor(), core::bc_matrix4f::identity());
		}
	}
}