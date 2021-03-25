// [11/08/2018 MRB]

#include "Editor/EditorPCH.h"

#include "Core/Utility/bcEnumOperand.h"
#include "PhysicsImp/Collision/bcShapeQuery.h"
#include "Game/Object/Scene/Component/bcSkinnedMeshComponent.h"
#include "Game/Object/Animation/Job/bcMeshColliderSkinningAnimationJob.h"
#include "Editor/Application/bcUICommand.h"

namespace black_cat
{
	namespace editor
	{
		bc_iui_command::state_ptr bc_iui_command::create_state(state_context& p_context) const
		{
			return nullptr;
		}

		void bc_iui_command::update_ui(update_ui_context& p_context)
		{
		}

		physics::bc_ray bc_iui_command::get_pointer_ray(const update_context& p_context,
			bcUINT16 p_point_left,
			bcUINT16 p_point_top) const
		{
			const auto& l_camera = *p_context.m_game_system.get_input_system().get_camera();
			const auto l_pointing_ray = l_camera.project_clip_point_to_3d_ray(p_point_left, p_point_top);

			return physics::bc_ray(l_camera.get_position(), l_pointing_ray, l_camera.get_far_clip());
		}

		bool bc_iui_command::query_ray_in_scene(const update_context& p_context,
			bcUINT16 p_point_left,
			bcUINT16 p_point_top,
			game::bc_actor_group p_query_group,
			physics::bc_query_flags p_flags,
			physics::bc_scene_ray_query_buffer& p_result) const
		{
			const auto l_ray = get_pointer_ray(p_context, p_point_left, p_point_top);
			auto l_query_filter = physics::bc_scene_query_post_filter_callback
			(
				[&p_context, &l_ray, this](const physics::bc_scene_query_post_filter_data& p_filter_data)
				{
					return skinned_mesh_hit_check(p_context, l_ray, p_filter_data);
				}
			);

			auto& l_px_scene = p_context.m_game_system.get_scene()->get_px_scene();

			{
				physics::bc_scene_shared_lock l_lock(&l_px_scene);

				const bool l_px_hit_result = l_px_scene.raycast
				(
					l_ray,
					p_result,
					physics::bc_hit_flag::hit_info,
					p_flags,
					static_cast<physics::bc_query_group>(p_query_group),
					&l_query_filter
				);
				return l_px_hit_result;
			}
		}

		physics::bc_query_hit_type bc_iui_command::skinned_mesh_hit_check(const update_context& p_context,
			const physics::bc_ray& p_ray,
			const physics::bc_scene_query_post_filter_data& p_filter_data) const
		{
			const auto l_shape_query_flag = p_filter_data.m_shape.get_query_flags();
			const auto l_shape_query_type = core::bc_enum::has(l_shape_query_flag, physics::bc_shape_query_flag::touching) ?
				physics::bc_query_hit_type::touch :
				physics::bc_query_hit_type::block;
			
			if (p_filter_data.m_shape.get_query_group() != static_cast<physics::bc_query_group>(game::bc_actor_group::skinned_mesh))
			{
				return l_shape_query_type;
			}
			
			game::bc_actor l_actor = p_context.m_game_system.get_physics_system().get_game_actor(p_filter_data.m_actor);
			auto* l_mediate_component = l_actor.get_component<game::bc_mediate_component>();
			auto* l_skinned_mesh_component = l_actor.get_component<game::bc_skinned_mesh_component>();

			BC_ASSERT(l_skinned_mesh_component);

			game::bc_mesh_collider_skinning_animation_job l_skinning_job
			(
				*l_skinned_mesh_component->get_skeleton(),
				l_skinned_mesh_component->get_mesh(),
				l_skinned_mesh_component->get_model_transforms()
			);
			l_skinning_job.run(p_context.m_clock);

			const physics::bc_triangle_mesh_desc l_triangle_mesh_desc
			(
				physics::bc_bounded_strided_typed_data< core::bc_vector3f >
				(
					l_skinning_job.get_vertices(),
					sizeof(core::bc_vector3f),
					l_skinning_job.get_vertices_count()
				),
				physics::bc_bounded_strided_data
				(
					l_skinning_job.get_16bit_indices() ? static_cast<const void*>(l_skinning_job.get_16bit_indices()) : static_cast<const void*>(l_skinning_job.get_32bit_indices()),
					l_skinning_job.get_16bit_indices() ? sizeof(bcUINT16) : sizeof(bcUINT32),
					l_skinning_job.get_faces_count() * 3
				),
				l_skinning_job.get_16bit_indices() ? physics::bc_triangle_mesh_flag::use_16bit_index : core::bc_enum::none<physics::bc_triangle_mesh_flag>()
			);
			auto l_triangle_mesh = p_context.m_game_system.get_physics_system().get_physics().create_runtime_triangle_mesh(l_triangle_mesh_desc);

			physics::bc_ray_hit l_hit;
			const auto l_hit_count = physics::bc_shape_query::ray_cast
			(
				p_ray,
				physics::bc_shape_triangle_mesh(l_triangle_mesh.get()),
				physics::bc_transform(l_mediate_component->get_world_transform()),
				physics::bc_hit_flag::position,
				&l_hit,
				1
			);

			if(l_hit_count == 0)
			{
				return physics::bc_query_hit_type::none;
			}
			
			return l_shape_query_type;
		}
	}
}