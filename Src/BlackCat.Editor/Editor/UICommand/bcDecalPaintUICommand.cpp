// [12/10/2021 MRB]

#include "Editor/EditorPCH.h"

#include "Game/Object/Scene/Component/bcDecalResolverComponent.h"
#include "Editor/UICommand/bcDecalPaintUICommand.h"

namespace black_cat
{
	namespace editor
	{
		bc_decal_paint_ui_command::bc_decal_paint_ui_command(QString p_decal_name,
			bcUINT16 p_screen_width,
			bcUINT16 p_screen_height,
			bcUINT16 p_point_left,
			bcUINT16 p_point_top)
			: m_decal_name(std::move(p_decal_name)),
			m_screen_width(p_screen_width),
			m_screen_height(p_screen_height),
			m_point_left(p_point_left),
			m_point_top(p_point_top)
		{
		}

		bc_decal_paint_ui_command::bc_decal_paint_ui_command(const bc_decal_paint_ui_command&) noexcept = default;

		bc_decal_paint_ui_command::~bc_decal_paint_ui_command() = default;

		bc_decal_paint_ui_command& bc_decal_paint_ui_command::operator=(bc_decal_paint_ui_command&) noexcept = default;

		core::bc_string bc_decal_paint_ui_command::title() const
		{
			return "DecalPaint";
		}

		bool bc_decal_paint_ui_command::update(update_context& p_context)
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
			if (!l_query_result || !l_query_buffer.has_block())
			{
				return false;
			}

			const game::bc_ray_hit l_ray_hit = l_query_buffer.get_block();
			auto* l_decal_component = l_ray_hit.get_actor().get_component<game::bc_decal_resolver_component>();
			if(!l_decal_component)
			{
				return false;
			}

			const auto l_decal_name = m_decal_name.toStdString();
			l_decal_component->add_decal(l_decal_name.c_str(), l_ray_hit.get_position(), l_ray_hit.get_normal());
			
			return false;
		}
	}
}