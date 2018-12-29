// [11/22/2018 MRB]

#include "Editor/EditorPCH.h"

#include "Game/Object/Scene/Component/bcRigidBodyComponent.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/bcEntityManager.h"
#include "Game/System/Physics/bcPxWrap.h"
#include "Editor/UICommand/bcUIObjectInsertCommand.h"

namespace black_cat
{
	namespace editor
	{
		bc_ui_object_insert_command::bc_ui_object_insert_command(QString p_entity_name,
			bcUINT16 p_screen_width,
			bcUINT16 p_screen_height,
			bcUINT16 p_point_left,
			bcUINT16 p_point_top)
			: m_entity_name(std::move(p_entity_name)),
			m_screen_width(p_screen_width),
			m_screen_height(p_screen_height),
			m_point_left(p_point_left),
			m_point_top(p_point_top)
		{
		}

		bc_ui_object_insert_command::bc_ui_object_insert_command(const bc_ui_object_insert_command&) = default;

		bc_ui_object_insert_command::~bc_ui_object_insert_command() = default;

		bc_ui_object_insert_command& bc_ui_object_insert_command::operator=(const bc_ui_object_insert_command&) = default;

		core::bc_string bc_ui_object_insert_command::title() const
		{
			return "ObjectInsert";
		}

		bc_iui_command::state_ptr bc_ui_object_insert_command::create_state(state_context& p_context) const
		{
			return nullptr;
		}

		bool bc_ui_object_insert_command::update(update_context& p_context)
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

			core::bc_vector3f l_position;
			if (l_query_result)
			{
				game::bc_ray_hit l_hit = l_query_buffer.get_block();
				l_position = l_hit.get_position();
			}
			else
			{
				auto l_ray = get_pointer_ray
				(
					p_context,
					m_screen_width,
					m_screen_height,
					m_point_left,
					m_point_top
				);

				l_position = l_ray.m_origin + (l_ray.m_dir * 70);
			}

			std::string l_entity_name = m_entity_name.toStdString();
			game::bc_actor l_actor = core::bc_get_service<game::bc_entity_manager>()->create_entity(l_entity_name.c_str());
			
			auto* l_rigid_body_component = l_actor.get_component<game::bc_rigid_body_component>();
			if(l_rigid_body_component != nullptr)
			{
				l_rigid_body_component->get_body().set_global_pose(physics::bc_transform(l_position));
			}

			auto* l_mesh_component = l_actor.get_component<game::bc_mesh_component>();
			if(l_mesh_component != nullptr)
			{
				core::bc_matrix4f l_mat;
				l_mat.translate(l_position.x, l_position.y, l_position.z);

				l_mesh_component->set_world_transform(core::bc_matrix4f(l_mat));
			}

			p_context.m_game_system.get_scene()->add_actor(l_actor);

			return false;
		}
	}
}