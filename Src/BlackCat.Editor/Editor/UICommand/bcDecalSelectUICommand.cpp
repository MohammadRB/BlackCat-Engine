// [12/13/2021 MRB]

#include "Editor/EditorPCH.h"

#include "Core/Utility/bcLogger.h"
#include "PhysicsImp/Shape/bcShapeBox.h"
#include "PhysicsImp/Collision/bcShapeQuery.h"
#include "Game/System/Physics/bcPxWrap.h"
#include "Game/Object/Scene/ActorComponent/bcActor.h"
#include "Game/Object/Scene/Component/bcDecalComponent.h"
#include "Editor/UICommand/bcDecalSelectUICommand.h"
#include "Editor/UI/bcFormDecal.h"

namespace black_cat
{
	namespace editor
	{
		bc_decal_select_ui_command::bc_decal_select_ui_command(bcUINT16 p_screen_width,
			bcUINT16 p_screen_height,
			bcUINT16 p_point_left,
			bcUINT16 p_point_top)
			: m_screen_width(p_screen_width),
			m_screen_height(p_screen_height),
			m_point_left(p_point_left),
			m_point_top(p_point_top),
			m_selected_decal(nullptr)
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
			physics::bc_scene_ray_query_buffer l_query_buffer;

			const bool l_query_result = query_ray_in_scene
			(
				p_context,
				m_point_left,
				m_point_top,
				game::bc_actor_group::all,
				core::bc_enum::mask_or({ physics::bc_query_flags::statics, physics::bc_query_flags::dynamics }),
				l_query_buffer
			);

			if (!l_query_result)
			{
				return false;
			}
			
			const game::bc_ray_hit l_actor_hit = l_query_buffer.get_block();
			const auto l_actor = l_actor_hit.get_actor();
			const auto* l_decal_component = l_actor.get_component<game::bc_decal_component>();

			if(!l_decal_component)
			{
				return false;
			}

			const auto l_ray = get_pointer_ray(p_context, m_point_left, m_point_top);

			for(auto& l_decal : *l_decal_component)
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

				m_selected_decal = l_decal.get();
				break;
			}

			if(!m_selected_decal)
			{
				return false;
			}

			return false;
		}

		void bc_decal_select_ui_command::update_ui(update_ui_context& p_context)
		{
			p_context.m_form_decal.setSelectedDecal(m_selected_decal);
		}
	}
}