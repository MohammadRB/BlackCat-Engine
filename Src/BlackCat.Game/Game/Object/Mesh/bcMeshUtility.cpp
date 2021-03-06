// [01/23/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Utility/bcLogger.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcRenderStateBuffer.h"
#include "Game/System/Render/Material/bcMaterialManager.h"
#include "Game/Object/Mesh/bcMeshUtility.h"
#include "Game/Object/Mesh/bcSubMesh.h"

namespace black_cat
{
	namespace game
	{
		bc_mesh_render_state bc_mesh_utility::create_mesh_render_states(bc_render_system& p_render_system,
			const bc_mesh& p_mesh,
			const bc_mesh_node& p_root_node, 
			const bcCHAR* p_mesh_prefix)
		{
			return create_mesh_render_states(p_render_system, p_mesh, p_root_node, core::bc_json_key_value(), p_mesh_prefix);
		}

		bc_mesh_render_state bc_mesh_utility::create_mesh_render_states(bc_render_system& p_render_system, 
			const bc_mesh& p_mesh, 
			const bc_mesh_node& p_root_node, 
			const core::bc_json_key_value& p_mesh_materials, 
			const bcCHAR* p_mesh_prefix)
		{
			core::bc_vector<bc_mesh_render_state_entry> l_render_states;
			const auto l_mesh_lod = p_mesh.get_level_of_details();

			l_render_states.reserve(p_mesh.get_mesh_count() * l_mesh_lod.get_mesh_count());
			
			for(auto l_mesh_lod_ite = 0U; l_mesh_lod_ite < l_mesh_lod.get_mesh_count(); ++l_mesh_lod_ite)
			{
				const auto& l_lod_mesh = l_mesh_lod.get_mesh(l_mesh_lod_ite);
				auto* l_render_states_ptr = &l_render_states;
				
				iterate_over_nodes
				(
					l_lod_mesh,
					l_render_states_ptr,
					[&p_render_system, &l_lod_mesh, &p_mesh_materials, p_mesh_prefix](const bc_mesh_node& p_node, decltype(l_render_states_ptr) p_result)
					{
						for (bc_mesh_node::node_index_t l_mesh_ite = 0, l_mesh_count = p_node.get_mesh_count(); l_mesh_ite < l_mesh_count; ++l_mesh_ite)
						{
							const auto& l_mesh_name = l_lod_mesh.get_node_mesh_name(p_node, l_mesh_ite);

							if (p_mesh_prefix != nullptr)
							{
								const bool l_starts_with_prefix = l_mesh_name.compare(0, std::strlen(p_mesh_prefix), p_mesh_prefix) == 0;

								if (!l_starts_with_prefix)
								{
									continue;
								}
							}

							auto l_material = l_lod_mesh.get_node_mesh_material_ptr(p_node, l_mesh_ite);
							auto l_render_state = l_lod_mesh.get_node_mesh_render_state_ptr(p_node, l_mesh_ite);
							auto l_material_ite = p_mesh_materials.find(l_mesh_name.c_str());

							if (l_material_ite != std::cend(p_mesh_materials))
							{
								core::bc_any& l_material_key = l_material_ite->second;
								auto& l_material_name = l_material_key.as_throw< core::bc_string >();
								auto l_new_material = p_render_system.get_material_manager().load_mesh_material(l_material_name.c_str());

								if (l_new_material == nullptr)
								{
									auto l_msg = core::bc_estring_frame(bcL("No material was found with key '")) +
										core::bc_to_estring_frame(l_material_name.c_str()) +
										bcL("' to associate with mesh '") +
										core::bc_to_estring_frame(l_lod_mesh.get_name()) +
										bcL("'");
									core::bc_get_service<core::bc_logger>()->log_info(l_msg.c_str());
								}
								else
								{
									l_material = l_new_material;
								}

								// TODO this causes every instance get a new render state and render instances doesn't group together in draw calls
								l_render_state = p_render_system.create_render_state
								(
									graphic::bc_primitive::trianglelist,
									l_render_state->get_vertex_buffer(),
									l_render_state->get_vertex_buffer_stride(),
									0,
									l_render_state->get_index_buffer(),
									l_render_state->get_index_type(),
									l_render_state->get_index_count(),
									0,
									{
										graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, l_material->get_diffuse_map_view()),
										graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, l_material->get_normal_map_view()),
										graphic::bc_resource_view_parameter(0, graphic::bc_shader_type::pixel, l_material->get_specular_map_view()),
									},
									{
										l_lod_mesh.get_skinned() ? p_render_system.get_per_skinned_object_cbuffer() : p_render_system.get_per_object_cbuffer(),
										graphic::bc_constant_buffer_parameter
										(
											1,
											core::bc_enum::mask_or({graphic::bc_shader_type::vertex, graphic::bc_shader_type::pixel}),
											l_material->get_parameters_cbuffer()
										)
									}
								);
							}

							p_result->push_back(bc_mesh_render_state_entry(p_node.get_transform_index(), std::move(l_render_state), std::move(l_material)));
						}

						return p_result;
					},
					&p_root_node
				);
			}

			const auto l_render_state_per_lod = l_render_states.size() / l_mesh_lod.get_mesh_count();
			return bc_mesh_render_state(std::move(l_render_states), l_render_state_per_lod);
		}

		void bc_mesh_utility::render_mesh(bc_render_state_buffer& p_buffer, 
			const bc_mesh_render_state& p_render_states, 
			const bc_sub_mesh_mat4_transform& p_transformations, 
			bcUINT32 p_lod,
			bc_render_group p_group)
		{
			auto l_begin = p_render_states.begin(p_lod);
			const auto l_end = p_render_states.end(p_lod);
			
			while (l_begin != l_end)
			{
				auto& l_entry = *l_begin;
				const auto& l_node_transformation = p_transformations[l_entry.m_node_index];

				bc_render_instance l_instance(l_node_transformation, p_group);
				p_buffer.add_render_instance(l_entry.m_render_state, l_instance);

				++l_begin;
			}
		}

		void bc_mesh_utility::render_skinned_mesh(bc_render_state_buffer& p_buffer, 
			const bc_mesh_render_state& p_render_states, 
			const bc_sub_mesh_mat4_transform& p_transformations, 
			bcUINT32 p_lod,
			bc_render_group p_group)
		{
			auto l_begin = p_render_states.begin(p_lod);
			const auto l_end = p_render_states.end(p_lod);
			
			while (l_begin != l_end)
			{
				auto& l_entry = *l_begin;
				
				bc_skinned_render_instance l_instance(&*p_transformations.begin(), p_transformations.size(), p_group);
				p_buffer.add_skinned_render_instance(l_entry.m_render_state, std::move(l_instance));

				++l_begin;
			}
		}

		void bc_mesh_utility::calculate_bound_box(const bc_mesh& p_mesh, 
			const bc_sub_mesh_mat4_transform& p_world_transforms,
			physics::bc_bound_box& p_bound_box) noexcept
		{
			p_bound_box.set_empty();

			int l_dummy = 0;
			iterate_over_nodes(p_mesh, l_dummy, [&p_mesh, &p_world_transforms, &p_bound_box](const bc_mesh_node& p_node, int)
			{
				const auto& l_node_transform = p_world_transforms.get_node_transform(p_node);

				// Update mesh bounding box based on its sub meshes
				for (bcSIZE l_mesh_ite = 0; l_mesh_ite < p_node.get_mesh_count(); ++l_mesh_ite)
				{
					auto l_node_mesh_box = p_mesh.get_node_mesh_bound_box(p_node, l_mesh_ite);
					l_node_mesh_box.transform(physics::bc_transform(l_node_transform));

					if (p_bound_box.is_empty())
					{
						p_bound_box = l_node_mesh_box;
					}
					else
					{
						p_bound_box.expand(l_node_mesh_box);
					}
				}

				return 0;
			});
		}

		void bc_mesh_utility::calculate_absolute_transforms(const bc_mesh& p_mesh, 
			const core::bc_matrix4f& p_world, 
			bc_sub_mesh_mat4_transform& p_world_transforms, 
			physics::bc_bound_box& p_bound_box) noexcept
		{
			p_bound_box.set_empty();

			iterate_over_nodes(p_mesh, p_world, [&p_mesh, &p_world_transforms, &p_bound_box](const bc_mesh_node& p_node, const core::bc_matrix4f& p_parent_transform)
			{
				const auto l_node_absolute_transformation = p_mesh.get_node_transform(p_node) * p_parent_transform;
				p_world_transforms.set_node_transform
				(
					p_node,
					l_node_absolute_transformation
				);

				// Update mesh bounding box based on its sub meshes
				for (bcSIZE l_m = 0; l_m < p_node.get_mesh_count(); ++l_m)
				{
					auto l_node_mesh_box = p_mesh.get_node_mesh_bound_box(p_node, l_m);
					l_node_mesh_box.transform(physics::bc_transform(l_node_absolute_transformation));

					if (p_bound_box.is_empty())
					{
						p_bound_box = l_node_mesh_box;
					}
					else
					{
						p_bound_box.expand(l_node_mesh_box);
					}
				}

				return l_node_absolute_transformation;
			});
		}

		void bc_mesh_utility::calculate_mesh_collider_transforms(const bc_mesh& p_mesh, 
			const bc_sub_mesh_mat4_transform& p_model_transforms, 
			bc_sub_mesh_px_transform& p_transforms)
		{
			for (const auto& l_mesh_part_collider : p_mesh.get_collider())
			{
				for (const bc_mesh_part_collider_entry& l_entry : l_mesh_part_collider.second)
				{
					const auto* l_collider_attached_node = p_mesh.find_node(l_entry.m_attached_node_transform_index);
					BC_ASSERT(l_collider_attached_node);

					const auto& l_model_transform = p_model_transforms.get_node_transform(*l_collider_attached_node);
					auto& l_output_transform = p_transforms.get_node_transform(*l_collider_attached_node);
					
					l_output_transform = physics::bc_transform(l_model_transform) * l_entry.m_local_transform;
				}
			}
		}

		void bc_mesh_utility::calculate_skinned_mesh_collider_transforms(const bc_mesh& p_mesh, 
			const bc_sub_mesh_mat4_transform& p_model_transforms, 
			bc_sub_mesh_px_transform& p_transforms)
		{
			for (const auto& l_mesh_part_collider : p_mesh.get_collider())
			{
				for (bc_mesh_part_collider_entry& l_entry : l_mesh_part_collider.second)
				{
					const auto* l_collider_attached_node = p_mesh.find_node(l_entry.m_attached_node_transform_index);
					BC_ASSERT(l_collider_attached_node);

					const auto& l_inverse_bind_pose_transform = p_mesh.get_node_inverse_bind_pose_transform(*l_collider_attached_node);
					const auto& l_model_transform = p_model_transforms.get_node_transform(*l_collider_attached_node);
					auto& l_output_transform = p_transforms.get_node_transform(*l_collider_attached_node);

					physics::bc_transform l_px_inverse_bind_pose_transform
					(
						l_inverse_bind_pose_transform.get_translation() * p_mesh.get_scale(), 
						l_inverse_bind_pose_transform.get_rotation()
					);
					
					l_output_transform = l_px_inverse_bind_pose_transform * l_entry.m_local_transform;
					l_output_transform = physics::bc_transform(l_model_transform) * l_output_transform;
				}
			}
		}
	}
}