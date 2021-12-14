// [12/10/2021 MRB]

#include "Editor/EditorPCH.h"

#include "Game/Object/Scene/Component/Event/bcRemoveDecalActorEvent.h"
#include "Editor/UICommand/bcDecalRemoveUICommand.h"
#include "Editor/UI/bcFormDecal.h"

namespace black_cat
{
	namespace editor
	{
		bc_decal_remove_ui_command::bc_decal_remove_ui_command(game::bc_decal_instance& p_decal)
			: m_decal(&p_decal)
		{
		}

		bc_decal_remove_ui_command::bc_decal_remove_ui_command(const bc_decal_remove_ui_command&) noexcept = default;

		bc_decal_remove_ui_command::~bc_decal_remove_ui_command() = default;

		bc_decal_remove_ui_command& bc_decal_remove_ui_command::operator=(bc_decal_remove_ui_command&) noexcept = default;

		core::bc_string bc_decal_remove_ui_command::title() const
		{
			return "DecalRemove";
		}

		bool bc_decal_remove_ui_command::update(update_context& p_context)
		{
			m_decal->get_actor().add_event(game::bc_remove_decal_actor_event(*m_decal));
			
			return false;
		}

		void bc_decal_remove_ui_command::update_ui(update_ui_context& p_context)
		{
			p_context.m_form_decal.setSelectedDecal(nullptr);
		}
	}
}
