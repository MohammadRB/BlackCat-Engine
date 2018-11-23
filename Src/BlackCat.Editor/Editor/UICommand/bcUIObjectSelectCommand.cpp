// [11/08/2018 MRB]

#include "Editor/EditorPCH.h"

#include "Core/Utility/bcLogger.h"
#include "Game/System/Physics/bcPxWrap.h"
#include "Game/Object/Scene/bcActor.h"
#include "Game/Object/Scene/Component/bcNameComponent.h"
#include "Editor/UICommand/bcUIObjectSelectCommand.h"

namespace black_cat
{
	namespace editor
	{
		bc_ui_object_select_command::bc_ui_object_select_command(bcUINT16 p_screen_width,
			bcUINT16 p_screen_height,
			bcUINT16 p_point_left,
			bcUINT16 p_point_top)
			: m_screen_width(p_screen_width),
			m_screen_height(p_screen_height),
			m_point_left(p_point_left),
			m_point_top(p_point_top)
		{
		}

		bc_ui_object_select_command::bc_ui_object_select_command(const bc_ui_object_select_command&) = default;

		bc_ui_object_select_command::~bc_ui_object_select_command() = default;

		bc_ui_object_select_command& bc_ui_object_select_command::operator=(const bc_ui_object_select_command&) = default;

		core::bc_string bc_ui_object_select_command::title() const
		{
			return "ObjectSelect";
		}

		bc_iui_command::state_ptr bc_ui_object_select_command::create_state(state_context& p_context) const
		{
			return nullptr;
		}

		bool bc_ui_object_select_command::update(update_context& p_context)
		{
			physics::bc_scene_ray_query_buffer l_query_buffer(1);
			const bool l_query_result = query_ray_in_scene
			(
				p_context,
				m_screen_width, 
				m_screen_height, 
				m_point_left, 
				m_point_top,
				game::bc_query_group::all,
				core::bc_enum::or({ physics::bc_query_flags::statics, physics::bc_query_flags::dynamics }),
				l_query_buffer
			);
			if(!l_query_result)
			{
				m_selected_actor_index = game::bc_actor::invalid_index;
				m_selected_actor_entity_name = nullptr;
				return false;
			}

			game::bc_ray_hit l_hit = l_query_buffer.get_block();
			game::bc_actor l_actor = l_hit.get_actor();
			game::bc_name_component* l_name = l_actor.get_component<game::bc_name_component>();

			m_selected_actor_index = l_actor.get_index();
			m_selected_actor_entity_name = l_name->get_entity_name();

			return false;
		}

		void bc_ui_object_select_command::update_ui(update_ui_context& p_context)
		{
			if(m_selected_actor_index != game::bc_actor::invalid_index)
			{
				p_context.m_form_object.setEntityName(m_selected_actor_entity_name);
			}
			else
			{
				p_context.m_form_object.setEntityName(nullptr);
			}
		}
	}
}