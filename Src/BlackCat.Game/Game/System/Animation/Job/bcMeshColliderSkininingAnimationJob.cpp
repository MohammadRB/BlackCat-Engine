// [02/04/2021 MRB]

#include "Game/GamePCH.h"
#include "Game/System/Animation/Job/bcMeshColliderSkinningAnimationJob.h"

namespace black_cat
{
	namespace game
	{
		bc_mesh_collider_skinning_animation_job::bc_mesh_collider_skinning_animation_job(bc_animation_skeleton& p_skeleton,
			const bc_sub_mesh& p_sub_mesh,
			const bc_sub_mesh_mat4_transform& p_model_transforms)
			: bci_animation_job(p_skeleton),
			m_sub_mesh(&p_sub_mesh),
			m_model_transforms(&p_model_transforms)
		{
		}

		bc_mesh_collider_skinning_animation_job::bc_mesh_collider_skinning_animation_job(bc_mesh_collider_skinning_animation_job&& p_other) noexcept = default;

		bc_mesh_collider_skinning_animation_job::~bc_mesh_collider_skinning_animation_job() = default;

		bc_mesh_collider_skinning_animation_job& bc_mesh_collider_skinning_animation_job::operator=(bc_mesh_collider_skinning_animation_job&& p_other) noexcept = default;

		bool bc_mesh_collider_skinning_animation_job::run(const platform::bc_clock::update_param& p_clock)
		{
			if (!m_enabled)
			{
				return true;
			}
			
			if(m_skinning_model_transforms.empty())
			{
				m_skinning_model_transforms = bc_sub_mesh_mat4_transform(*m_sub_mesh->get_root_node());
			}
			
			const auto l_bone_names = get_skeleton().get_joint_names();
			auto l_bone_name_ite = l_bone_names.begin();

			for (auto l_bone_ite = 0U; l_bone_ite < l_bone_names.size(); ++l_bone_ite, ++l_bone_name_ite)
			{
				const auto* l_bone_name = *l_bone_name_ite;
				const auto* l_mesh_node = m_sub_mesh->find_node(l_bone_name);
				const auto& l_mesh_node_inverse_transform = m_sub_mesh->get_node_inverse_bind_pose_transform(*l_mesh_node);
				const auto& l_animation_model_transform = m_model_transforms->get_node_transform(*l_mesh_node);
				auto& l_output_transform = m_skinning_model_transforms.get_node_transform(*l_mesh_node);

				l_output_transform = l_mesh_node_inverse_transform * l_animation_model_transform;
			}
			
			const auto& l_collider = m_sub_mesh->get_mesh_collider().get_skinned_collider();
			
			if(m_skinned_vertices.empty())
			{
				m_skinned_vertices.resize(l_collider.get_vertices_count());
			}

			const auto* l_skinning_vertices = l_collider.get_vertices();
			const auto* l_16bit_indices = l_collider.get_16bit_indices();
			const auto* l_32bit_indices = l_collider.get_32bit_indices();
			
			for(auto l_face_ite = 0U; l_face_ite < l_collider.get_faces_count(); ++l_face_ite)
			{
				for (auto l_vertex_ite = 0U; l_vertex_ite < 3; ++l_vertex_ite)
				{
					bcSIZE l_vertex_index;
					if (l_16bit_indices)
					{
						l_vertex_index = l_16bit_indices[l_face_ite * 3 + l_vertex_ite];
					}
					else
					{
						l_vertex_index = l_32bit_indices[l_face_ite * 3 + l_vertex_ite];
					}

					const auto& l_skinning_vertex = l_skinning_vertices[l_vertex_index];
					core::bc_vector4f l_final_vertex = core::bc_vector4f(0, 0, 0, 0);

					for(auto l_weight_ite = 0U; l_weight_ite < 4; ++l_weight_ite)
					{
						const auto l_bone_weight = l_skinning_vertex.m_weights[l_weight_ite];
						if(l_bone_weight <= 0.0f)
						{
							continue;
						}
						
						const auto l_bone_index = l_skinning_vertex.m_indices[l_weight_ite];
						l_final_vertex += m_skinning_model_transforms[l_bone_index] * core::bc_vector4f(l_skinning_vertex.m_position, 1) * l_bone_weight;
					}

					m_skinned_vertices[l_vertex_index] = l_final_vertex.xyz();
				}
			}

			return true;
		}
	}
}