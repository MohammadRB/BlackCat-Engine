// [12/13/2021 MRB]

#include "Editor/EditorPCH.h"

#include "Core/Utility/bcLogger.h"
#include "PhysicsImp/Shape/bcShapeBox.h"
#include "PhysicsImp/Collision/bcShapeQuery.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/Object/Scene/Component/bcDecalComponent.h"
#include "Game/System/Physics/bcPxWrap.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/bcGameSystem.h"
#include "App/RenderPass/bcShapeDrawPass.h"
#include "Editor/UICommand/bcDecalSelectUICommand.h"
#include "Editor/UI/bcFormDecal.h"

namespace black_cat
{
	namespace editor
	{
		bc_decal_select_ui_command::bc_decal_select_ui_command(bcUINT16 p_screen_width,
			bcUINT16 p_screen_height,
			bcUINT16 p_point_left,
			bcUINT16 p_point_top,
			bool p_is_decal_hover)
			: m_screen_width(p_screen_width),
			m_screen_height(p_screen_height),
			m_point_left(p_point_left),
			m_point_top(p_point_top),
			m_is_decal_hover(p_is_decal_hover)
		{
		}

		bc_decal_select_ui_command::bc_decal_select_ui_command(const bc_decal_select_ui_command&) = default;

		bc_decal_select_ui_command::~bc_decal_select_ui_command() = default;

		bc_decal_select_ui_command& bc_decal_select_ui_command::operator=(const bc_decal_select_ui_command&) = default;

		core::bc_string bc_decal_select_ui_command::title() const
		{
			return "DecalSelect";
		}

		bci_ui_command::state_ptr bc_decal_select_ui_command::create_state(state_context& p_context) const
		{
			return nullptr;
		}

		bool bc_decal_select_ui_command::update(update_context& p_context)
		{
			if (!p_context.m_game_system.get_scene())
			{
				return false;
			}
			
			const auto l_actor = query_ray_in_scene
			(
				p_context,
				m_point_left,
				m_point_top
			);
			if (!l_actor.is_valid())
			{
				return false;
			}
			
			const auto* l_decal_component = l_actor.get_component<game::bc_decal_component>();
			if(!l_decal_component)
			{
				return false;
			}

			const auto l_ray = get_pointer_ray(p_context, m_point_left, m_point_top);

			for(const auto& l_decal : *l_decal_component)
			{
				physics::bc_shape_box l_decal_box
				(
					l_decal->get_decal()->get_width() / 2,
					l_decal->get_decal()->get_height() / 2,
					l_decal->get_decal()->get_depth() / 2
				);

				physics::bc_ray_hit l_decal_hit;
				const auto l_num_hits = physics::bc_shape_query::ray_cast
				(
					l_ray,
					l_decal_box,
					physics::bc_transform(l_decal->get_world_transform()),
					physics::bc_hit_flag::distance,
					&l_decal_hit,
					1
				);

				if(!l_num_hits)
				{
					continue;
				}

				m_selected_decal = l_decal;
				break;
			}

			auto* l_shape_draw_pass = p_context.m_game_system.get_render_system().get_render_pass<bc_shape_draw_pass>();
			if (l_shape_draw_pass)
			{
				if (m_is_decal_hover)
				{
					l_shape_draw_pass->set_hovered_decal(m_selected_decal.get());
				}
				else
				{
					l_shape_draw_pass->set_selected_decal(m_selected_decal.get());
				}
			}

			return false;
		}

		void bc_decal_select_ui_command::update_ui(update_ui_context& p_context)
		{
			if (!m_is_decal_hover)
			{
				p_context.m_form_decal.setSelectedDecal(m_selected_decal.get());
			}
		}
	}
}