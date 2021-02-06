// [02/01/2017 MRB]

#include "Game/GamePCH.h"

#include "Core/Container/bcVector.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Mesh/bcMeshCollider.h"
#include "Game/Object/Mesh/bcSubMesh.h"
#include "Game/Object/Scene/Component/bcRigidBodyComponent.h"

namespace black_cat
{
	namespace game
	{
		void bc_rigid_body_component::update_px_shape_transforms(physics::bc_rigid_body& p_px_actor,
			const bc_sub_mesh_px_transform& p_model_space_transforms)
		{
			core::bc_vector_frame< physics::bc_shape > l_shapes(p_px_actor.get_shape_count());
			p_px_actor.get_shapes(l_shapes.data(), l_shapes.size());

			for(physics::bc_shape& l_shape : l_shapes)
			{
				const auto* l_collider_entry = static_cast<bc_mesh_part_collider_entry*>(l_shape.get_data());
				if(l_collider_entry == nullptr)
				{
					// TODO 
					// This shape is not controlled by transforms
					continue;
				}
				
				const auto& l_transform = p_model_space_transforms[l_collider_entry->m_attached_node_transform_index];

				l_shape.set_local_pose(l_transform);
			}
		}

		void bc_rigid_body_component::debug_draw(physics::bc_rigid_body& p_px_actor, const bc_actor_component_debug_draw_context& p_context)
		{
			core::bc_vector_frame< physics::bc_shape > l_shapes(p_px_actor.get_shape_count());
			p_px_actor.get_shapes(l_shapes.data(), l_shapes.size());

			for (physics::bc_shape& l_shape : l_shapes)
			{
				switch (l_shape.get_type())
				{
				case physics::bc_shape_type::sphere:
				{
					physics::bc_shape_sphere l_sphere(0);
					l_shape.as_sphere(l_sphere);
					p_context.m_shape_drawer.draw_wired_sphere(l_sphere, p_px_actor.get_global_pose().transform(l_shape.get_local_pose()));
					break;
				}
				case physics::bc_shape_type::plane:
					break;
				case physics::bc_shape_type::capsule:
				{
					physics::bc_shape_capsule l_capsule(0,0);
					l_shape.as_capsule(l_capsule);
					p_context.m_shape_drawer.draw_wired_capsule(l_capsule, p_px_actor.get_global_pose().transform(l_shape.get_local_pose()));
					break;
				}
				case physics::bc_shape_type::box:
				{
					physics::bc_shape_box l_box(0, 0, 0);
					l_shape.as_box(l_box);
					p_context.m_shape_drawer.draw_wired_box(l_box, p_px_actor.get_global_pose().transform(l_shape.get_local_pose()));
					break;
				}
				case physics::bc_shape_type::convex_mesh:
					break;
				case physics::bc_shape_type::triangle_mesh:
					break;
				case physics::bc_shape_type::height_field:
					break;
				default: ;
				}
			}
		}
	}
}