// [11/08/2018 MRB]

#include "Editor/EditorPCH.h"

#include "Core/Utility/bcLogger.h"
#include "Game/System/Physics/bcPxWrap.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "App/RenderPass/bcShapeDrawPass.h"
#include "Editor/UICommand/bcObjectSelectUICommand.h"
#include "Editor/UI/bcFormObject.h"

namespace black_cat
{
	namespace editor
	{
		bc_object_select_ui_command::bc_object_select_ui_command(bcUINT16 p_screen_width,
			bcUINT16 p_screen_height,
			bcUINT16 p_point_left,
			bcUINT16 p_point_top,
			bool p_is_object_hover)
			: m_screen_width(p_screen_width),
			m_screen_height(p_screen_height),
			m_point_left(p_point_left),
			m_point_top(p_point_top),
			m_is_object_hover(p_is_object_hover)
		{
		}

		bc_object_select_ui_command::bc_object_select_ui_command(const bc_object_select_ui_command&) = default;

		bc_object_select_ui_command::~bc_object_select_ui_command() = default;

		bc_object_select_ui_command& bc_object_select_ui_command::operator=(const bc_object_select_ui_command&) = default;

		core::bc_string bc_object_select_ui_command::title() const
		{
			return "ObjectSelect";
		}

		bci_ui_command::state_ptr bc_object_select_ui_command::create_state(state_context& p_context) const
		{
			return nullptr;
		}

		bool bc_object_select_ui_command::update(update_context& p_context)
		{
			m_actor = query_ray_in_scene(p_context, m_point_left, m_point_top);

			if (m_actor.is_valid())
			{
				m_actor_transformation = m_actor.get_component<game::bc_mediate_component>()->get_world_transform();
			}

			auto* l_shape_draw_pass = p_context.m_game_system.get_render_system().get_render_pass<bc_shape_draw_pass>();
			if (l_shape_draw_pass)
			{
				if(m_is_object_hover)
				{
					l_shape_draw_pass->set_hovered_actor(m_actor);
				}
				else
				{
					l_shape_draw_pass->set_selected_actor(m_actor);
				}
			}

			return false;

			/*physics::bc_scene_ray_query_buffer l_query_buffer;
			const bool l_query_result = query_ray_in_px_scene
			(
				p_context,
				m_point_left, 
				m_point_top,
				game::bc_actor_group::all,
				core::bc_enum::mask_or({ physics::bc_query_flags::statics, physics::bc_query_flags::dynamics }),
				l_query_buffer
			);
			
			if(l_query_result)
			{
				const game::bc_ray_hit l_hit = l_query_buffer.get_block();
				m_actor = l_hit.get_actor();
				m_actor_transformation = m_actor.get_component<game::bc_mediate_component>()->get_world_transform();
			}
			else
			{
				m_actor = game::bc_actor();
			}

			auto* l_shape_draw_pass = p_context.m_game_system.get_render_system().get_render_pass<bc_shape_draw_pass>();
			if(l_shape_draw_pass)
			{
				l_shape_draw_pass->set_selected_actor(m_actor);
			}

			return false;*/
		}

		void bc_object_select_ui_command::update_ui(update_ui_context& p_context)
		{
			if(!m_is_object_hover)
			{
				p_context.m_form_object.setSelectedActor(m_actor, m_actor_transformation);
			}
		}
	}
}