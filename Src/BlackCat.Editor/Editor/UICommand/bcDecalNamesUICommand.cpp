// [10/12/2021 MRB]

#include "Editor/EditorPCH.h"

#include "Game/Object/Scene/bcScene.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/bcGameSystem.h"
#include "Editor/UI/bcFormDecalInsert.h"
#include "Editor/UICommand/bcDecalNamesUICommand.h"

namespace black_cat::editor
{
	bc_decal_names_ui_command::bc_decal_names_ui_command() = default;

	bc_decal_names_ui_command::bc_decal_names_ui_command(const bc_decal_names_ui_command&) = default;

	bc_decal_names_ui_command::~bc_decal_names_ui_command() = default;

	bc_decal_names_ui_command& bc_decal_names_ui_command::operator=(const bc_decal_names_ui_command&) = default;

	core::bc_string bc_decal_names_ui_command::title() const
	{
		return "DecalNames";
	}

	bci_ui_command::state_ptr bc_decal_names_ui_command::create_state(state_context& p_context) const
	{
		return nullptr;
	}

	bool bc_decal_names_ui_command::update(update_context& p_context)
	{
		const auto l_decal_names = p_context.m_game_system.get_render_system().get_decal_manager().get_decal_names();

		std::transform
		(
			std::cbegin(l_decal_names),
			std::cend(l_decal_names),
			std::back_inserter(m_entity_list),
			[](core::bc_string_view p_entity_name)
			{
				return QString(p_entity_name.data());
			}
		);

		return false;
	}

	void bc_decal_names_ui_command::update_ui(update_ui_context& p_context)
	{
		p_context.m_form_decal_insert.add_decal_names(m_entity_list);
	}
}
