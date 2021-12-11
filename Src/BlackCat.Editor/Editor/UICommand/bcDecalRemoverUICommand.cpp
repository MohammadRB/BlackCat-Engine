// [12/10/2021 MRB]

#include "Editor/EditorPCH.h"
#include "Editor/UICommand/bcDecalRemoverUICommand.h"

namespace black_cat
{
	namespace editor
	{
		bc_decal_remover_ui_command::bc_decal_remover_ui_command(bcUINT16 p_screen_width,
			bcUINT16 p_screen_height,
			bcUINT16 p_point_left,
			bcUINT16 p_point_top)
			: m_screen_width(p_screen_width),
			m_screen_height(p_screen_height),
			m_point_left(p_point_left),
			m_point_top(p_point_top)
		{
		}

		bc_decal_remover_ui_command::bc_decal_remover_ui_command(const bc_decal_remover_ui_command&) noexcept = default;

		bc_decal_remover_ui_command::~bc_decal_remover_ui_command() = default;

		bc_decal_remover_ui_command& bc_decal_remover_ui_command::operator=(bc_decal_remover_ui_command&) noexcept = default;

		core::bc_string bc_decal_remover_ui_command::title() const
		{
			return "DecalRemover";
		}

		bool bc_decal_remover_ui_command::update(update_context& p_context)
		{
			return false;
		}
	}
}