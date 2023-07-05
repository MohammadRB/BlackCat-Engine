// [15/12/2021 MRB]

#include "Editor/EditorPCH.h"

#include "Game/bcUtility.h"
#include "Editor/UICommand/bcDecalUpdateUICommand.h"

namespace black_cat::editor
{
	bc_decal_update_ui_command::bc_decal_update_ui_command(game::bc_decal_instance& p_decal, const core::bc_vector3f& p_position, const core::bc_matrix3f& p_rotation)
		: m_decal(&p_decal),
		  m_position(p_position),
		  m_rotation(p_rotation)
	{
	}

	bc_decal_update_ui_command::bc_decal_update_ui_command(const bc_decal_update_ui_command&) noexcept = default;

	bc_decal_update_ui_command::~bc_decal_update_ui_command() = default;

	bc_decal_update_ui_command& bc_decal_update_ui_command::operator=(const bc_decal_update_ui_command&) noexcept = default;

	core::bc_string bc_decal_update_ui_command::title() const
	{
		return "DecalUpdate";
	}

	bool bc_decal_update_ui_command::update(update_context& p_context)
	{
		m_decal->set_local_position(m_position);
		m_decal->set_local_rotation(m_rotation);
		return false;
	}
}
