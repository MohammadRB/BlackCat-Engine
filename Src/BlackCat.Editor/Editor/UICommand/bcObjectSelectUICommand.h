// [08/11/2018 MRB]

#pragma once

#include "Core/Math/bcMatrix4f.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Editor/Application/bcUICommand.h"

namespace black_cat::editor
{
	class bc_object_select_ui_command : public bci_ui_command
	{
	public:
		bc_object_select_ui_command(bcUINT16 p_screen_width,
		                            bcUINT16 p_screen_height,
		                            bcUINT16 p_point_left,
		                            bcUINT16 p_point_top,
		                            bool p_is_object_hover);

		bc_object_select_ui_command(const bc_object_select_ui_command&);

		~bc_object_select_ui_command() override;

		bc_object_select_ui_command& operator=(const bc_object_select_ui_command&);

		core::bc_string title() const override;

		state_ptr create_state(state_context& p_context) const override;

		bool update(update_context& p_context) override;

		void update_ui(update_ui_context& p_context) override;

	private:
		bcUINT16 m_screen_width;
		bcUINT16 m_screen_height;
		bcUINT16 m_point_left;
		bcUINT16 m_point_top;
		bool m_is_object_hover;

		game::bc_actor m_actor;
		core::bc_matrix4f m_actor_transformation;
	};
}
