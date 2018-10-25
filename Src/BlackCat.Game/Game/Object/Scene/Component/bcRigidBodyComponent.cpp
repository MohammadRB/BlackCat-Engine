// [02/01/2017 MRB]

#include "Game/GamePCH.h"
#include "Game/System/bcGameSystem.h"
#include "Game/Object/Mesh/bcSubMesh.h"
#include "Game/Object/Scene/Component/bcMeshComponent.h"
#include "Game/Object/Scene/Component/bcRigidBodyComponent.h"

namespace black_cat
{
	namespace game
	{
		void bc_rigid_body_component::initialize_from_mesh(bc_physics_system& p_physics_system, 
			bc_actor& p_actor, 
			physics::bc_rigid_body& p_px_actor, 
			bc_mesh_component& p_mesh_component)
		{
			auto& l_physics = p_physics_system.get_physics();
			const bc_sub_mesh& l_mesh_part = p_mesh_component.get_sub_mesh();
			core::bc_vector_frame<const bc_mesh_part_collider::entry*> l_shapes;

			_get_mesh_colliders(l_mesh_part, l_mesh_part.get_node(), l_shapes);

			auto l_px_material = l_physics.create_material(1, 1, 0.1);

			for (const bc_mesh_part_collider_entry* l_shape : l_shapes)
			{
				auto l_px_shape = p_px_actor.create_shape(*l_shape->m_px_shape.get(), l_px_material.get());
				l_px_shape.set_local_pose(l_shape->m_transformation);
			}

			p_physics_system.connect_px_actor_to_game_actor(p_px_actor, p_actor);
		}

		void bc_rigid_body_component::_get_mesh_colliders(const bc_sub_mesh& p_mesh,
			const bc_mesh_node* p_mesh_node,
			core::bc_vector_frame<const bc_mesh_part_collider::entry*>& p_result)
		{
			for (bcUINT32 l_mesh_index = 0, l_end = p_mesh_node->get_mesh_count(); l_mesh_index < l_end; ++l_mesh_index)
			{
				const bc_mesh_part_collider* l_mesh_colliders = p_mesh.get_node_mesh_colliders(p_mesh_node, l_mesh_index);
				
				if(!l_mesh_colliders)
				{
					continue;
				}

				for(auto l_begin = l_mesh_colliders->cbegin(), l_end = l_mesh_colliders->cend(); l_begin != l_end; ++l_begin)
				{
					p_result.push_back(&*l_begin);
				}
			}

			auto& l_node_childs = p_mesh.get_node_childs(p_mesh_node);
			for (bcUINT32 l_child = 0, l_end = l_node_childs.size(); l_child < l_end; ++l_child)
			{
				_get_mesh_colliders(p_mesh, l_node_childs[l_child], p_result);
			}
		}
	}
}