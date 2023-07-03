// [22/11/2018 MRB]

#include "Editor/EditorPCH.h"

#include "Game/Object/Scene/bcEntityManager.h"
#include "Editor/UICommand/bcEntityNamesUICommand.h"
#include "Editor/UI/bcFormObjectInsert.h"

namespace black_cat
{
	namespace editor
	{
		bc_entity_names_ui_command::bc_entity_names_ui_command() = default;

		bc_entity_names_ui_command::bc_entity_names_ui_command(const bc_entity_names_ui_command&) = default;

		bc_entity_names_ui_command::~bc_entity_names_ui_command() = default;

		bc_entity_names_ui_command& bc_entity_names_ui_command::operator=(const bc_entity_names_ui_command&) = default;

		core::bc_string bc_entity_names_ui_command::title() const
		{
			return "EntityNames";
		}

		bci_ui_command::state_ptr bc_entity_names_ui_command::create_state(state_context& p_context) const
		{
			return nullptr;
		}

		bool bc_entity_names_ui_command::update(update_context& p_context)
		{
			const auto l_entity_names = core::bc_get_service<game::bc_entity_manager>()->get_entity_names();
			
			std::transform
			(
				std::cbegin(l_entity_names),
				std::cend(l_entity_names),
				std::back_inserter(m_entity_list),
				[](core::bc_string_view p_entity_name)
				{
					return QString(p_entity_name.data());
				}
			);

			return false;
		}

		void bc_entity_names_ui_command::update_ui(update_ui_context& p_context)
		{
			p_context.m_form_object_insert.add_entity_names(m_entity_list);
		}
	}
}