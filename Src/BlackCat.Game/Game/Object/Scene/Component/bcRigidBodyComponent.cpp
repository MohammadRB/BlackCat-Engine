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
		void bc_rigid_body_component::create_px_shapes_from_mesh(bc_physics_system& p_physics_system, 
			physics::bc_rigid_body& p_px_actor, 
			const bc_sub_mesh& p_mesh)
		{
			auto& l_physics = p_physics_system.get_physics();
			auto l_px_material = l_physics.create_material(1, 1, 0.1f);

			for(const auto& l_mesh_collider : p_mesh.get_mesh_collider())
			{
				for(bc_mesh_part_collider_entry& l_collider_entry : l_mesh_collider.second)
				{
					auto l_px_shape = p_px_actor.create_shape(*l_collider_entry.m_px_shape, l_px_material.get(), l_collider_entry.m_shape_flags);
					
					BC_ASSERT(l_px_shape.is_valid());

					l_px_shape.set_local_pose(l_collider_entry.m_initial_transform);
					l_px_shape.set_data(&l_collider_entry);
				}
			}
		}

		void bc_rigid_body_component::update_px_shape_transforms(physics::bc_rigid_body& p_px_actor,
			const bc_sub_mesh_px_transform& p_model_space_transforms)
		{
			core::bc_vector_frame< physics::bc_shape > l_shapes(p_px_actor.get_shape_count());
			p_px_actor.get_shapes(l_shapes.data(), l_shapes.size());

			for(physics::bc_shape& l_shape : l_shapes)
			{
				const auto* l_collider_entry = static_cast<bc_mesh_part_collider_entry*>(l_shape.get_data());
				const auto& l_transform = p_model_space_transforms[l_collider_entry->m_attached_node_transform_index];

				l_shape.set_local_pose(l_transform);
			}
		}
	}
}