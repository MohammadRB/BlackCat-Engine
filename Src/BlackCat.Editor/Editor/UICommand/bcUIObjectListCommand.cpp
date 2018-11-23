// [11/22/2018 MRB]

#include "Editor/EditorPCH.h"

#include "Game/Object/Scene/bcEntityManager.h"
#include "Editor/UICommand/bcUIObjectListCommand.h"

namespace black_cat
{
	namespace editor
	{
		bc_ui_object_list_command::bc_ui_object_list_command() = default;

		bc_ui_object_list_command::bc_ui_object_list_command(const bc_ui_object_list_command&) = default;

		bc_ui_object_list_command::~bc_ui_object_list_command() = default;

		bc_ui_object_list_command& bc_ui_object_list_command::operator=(const bc_ui_object_list_command&) = default;

		core::bc_string bc_ui_object_list_command::title() const
		{
			return "ObjectList";
		}

		bc_iui_command::state_ptr bc_ui_object_list_command::create_state(state_context& p_context) const
		{
			return nullptr;
		}

		bool bc_ui_object_list_command::update(update_context& p_context)
		{
			const auto l_entity_names = p_context.m_game_system.get_service<game::bc_entity_manager>()->get_entity_names();
			
			std::transform
			(
				std::cbegin(l_entity_names),
				std::cend(l_entity_names),
				std::back_inserter(m_entity_list),
				[](const bcCHAR* p_entity_name)
				{
					return QString(p_entity_name);
				}
			);

			return false;
		}

		void bc_ui_object_list_command::update_ui(update_ui_context& p_context)
		{
			p_context.m_form_object_insert.add_entity_names(m_entity_list);
		}
	}
}