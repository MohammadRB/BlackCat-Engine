// [23/01/2021 MRB]

#include "Game/GamePCH.h"

#include "Core/Container/bcStringStream.h"
#include "Core/Utility/bcLogger.h"
#include "Game/System/Render/bcRenderSystem.h"
#include "Game/System/Render/bcRenderStateBuffer.h"
#include "Game/System/Render/Material/bcMaterialManager.h"
#include "Game/System/Physics/bcPhysicsSystem.h"
#include "Game/System/Physics/bcActorGroup.h"
#include "Game/Object/Mesh/bcMeshUtility.h"
#include "Game/Object/Mesh/bcSubMesh.h"
#include "Game/Object/Scene/Component/bcMediateComponent.h"
#include "Game/Object/Scene/Component/bcSkinnedMeshComponent.h"
#include "Game/Object/Scene/ActorComponent/bcActor.hpp"
#include "Game/System/Animation/Job/bcMeshColliderSkinningAnimationJob.h"
#include "Game/bcUtility.h"

namespace black_cat::game
{
	core::bc_concurrent_lazy<bc_mesh_utility::mesh_render_state_container> bc_mesh_utility::m_mesh_render_states;
		
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
						const auto l_mesh_part_name = l_lod_mesh.get_node_mesh_name(p_node, l_mesh_ite);

						if (p_mesh_prefix != nullptr)
						{
							const bool l_starts_with_prefix = l_mesh_part_name.compare(0, std::strlen(p_mesh_prefix), p_mesh_prefix) == 0;

							if (!l_starts_with_prefix)
							{
								continue;
							}
						}

						auto l_material = l_lod_mesh.get_node_mesh_material_ptr(p_node, l_mesh_ite);
						auto l_render_state = l_lod_mesh.get_node_mesh_render_state_ptr(p_node, l_mesh_ite);
						auto l_material_ite = p_mesh_materials.find(l_mesh_part_name.data());

						if(l_material_ite == std::cend(p_mesh_materials))
						{
							l_material_ite = p_mesh_materials.find("*");
						}

						if (l_material_ite != std::cend(p_mesh_materials))
						{
							core::bc_any& l_material_key = l_material_ite->second;
							const auto& l_material_name = l_material_key.as_throw<core::bc_string>();
							auto l_cache_name = (core::bc_string_stream() << l_lod_mesh.get_name() << l_mesh_part_name << l_material_name).str();
							auto l_cache_entry = m_mesh_render_states->find(l_cache_name);

							if(l_cache_entry == std::end(*m_mesh_render_states))
							{
								auto l_new_material = p_render_system.get_material_manager().load_mesh_material(l_material_name.c_str());

								if (l_new_material == nullptr)
								{
									auto l_msg = core::bc_estring_stream() << bcL("No material was found with key '")
											<< l_material_name
											<< bcL("' to associate with mesh '")
											<< l_lod_mesh.get_name()
											<< bcL("'");
									core::bc_get_service<core::bc_logger>()->log_info(l_msg.str().c_str());
								}
								else
								{
									l_material = l_new_material;
								}

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

								// TODO find a way to cleanup this render states and materials
								l_cache_entry = m_mesh_render_states->insert(std::make_pair
									(
										std::move(l_cache_name),
										std::make_pair(std::move(l_render_state), std::move(l_material))
									)).first;
							}

							l_render_state = l_cache_entry->second.first;
							l_material = l_cache_entry->second.second;
						}

						p_result->push_back(bc_mesh_render_state_entry(p_node.get_index(), std::move(l_render_state), std::move(l_material)));
					}

					return p_result;
				},
				[](const bc_mesh_node&, decltype(l_render_states_ptr))
				{
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
	                                  bc_actor_render_group p_group)
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
	                                          bc_actor_render_group p_group)
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
		                   }, 
		                   [](const bc_mesh_node&, int)
		                   {
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
		                   },
		                   [](const bc_mesh_node&, const core::bc_matrix4f&)
		                   {
		                   });
	}

	void bc_mesh_utility::calculate_mesh_collider_transforms(const bc_mesh& p_mesh, 
	                                                         const bc_sub_mesh_mat4_transform& p_model_transforms, 
	                                                         bc_sub_mesh_px_transform& p_transforms)
	{
		const auto& l_mesh_collider = p_mesh.get_collider();
			
		for (const bc_mesh_part_collider_entry& l_collider : l_mesh_collider.get_colliders())
		{
			const auto* l_collider_attached_node = p_mesh.find_node(l_collider.m_attached_node_index);
			BC_ASSERT(l_collider_attached_node);

			const auto& l_model_transform = p_model_transforms.get_node_transform(*l_collider_attached_node);
			auto& l_output_transform = p_transforms.get_node_transform(*l_collider_attached_node);
				
			l_output_transform = physics::bc_transform(l_model_transform) * l_collider.m_local_transform;
		}
	}

	void bc_mesh_utility::calculate_skinned_mesh_collider_transforms(const bc_mesh& p_mesh, 
	                                                                 const bc_sub_mesh_mat4_transform& p_model_transforms, 
	                                                                 bc_sub_mesh_px_transform& p_transforms)
	{
		const auto& l_mesh_collider = p_mesh.get_collider();
			
		for (const bc_mesh_part_collider_entry& l_collider : l_mesh_collider.get_colliders())
		{
			const auto* l_collider_attached_node = p_mesh.find_node(l_collider.m_attached_node_index);
			BC_ASSERT(l_collider_attached_node);

			const auto& l_inverse_bind_pose_transform = p_mesh.get_node_inverse_bind_pose_transform(*l_collider_attached_node);
			const auto& l_model_transform = p_model_transforms.get_node_transform(*l_collider_attached_node);
			auto& l_output_transform = p_transforms.get_node_transform(*l_collider_attached_node);

			physics::bc_transform l_px_inverse_bind_pose_transform
			(
				l_inverse_bind_pose_transform.get_translation() * p_mesh.get_scale(), 
				l_inverse_bind_pose_transform.get_rotation()
			);
				
			l_output_transform = l_px_inverse_bind_pose_transform * l_collider.m_local_transform;
			l_output_transform = physics::bc_transform(l_model_transform) * l_output_transform;
		}
	}

	std::pair<physics::bc_query_hit_type, physics::bc_ray_hit> bc_mesh_utility::skinned_mesh_ray_hit_test(bc_physics_system& p_physics_system,
		const physics::bc_ray& p_ray,
		const physics::bc_actor& p_actor,
		const physics::bc_shape& p_shape)
	{
		const auto l_shape_query_flag = p_shape.get_query_flags();
		const auto l_shape_query_type = core::bc_enum::has(l_shape_query_flag, physics::bc_shape_query_flag::touching) ?
			                                physics::bc_query_hit_type::touch :
			                                physics::bc_query_hit_type::block;

		if (p_shape.get_query_group() != static_cast<physics::bc_query_group>(bc_actor_physics_group::skinned_mesh))
		{
			return std::make_pair(l_shape_query_type, physics::bc_ray_hit());
		}

		const auto l_actor = p_physics_system.get_game_actor(p_actor);
		const auto* l_mediate_component = l_actor.get_component<bc_mediate_component>();
		const auto* l_skinned_mesh_component = l_actor.get_component<bc_skinned_mesh_component>();

		BC_ASSERT(l_skinned_mesh_component);

		bc_mesh_collider_skinning_animation_job l_skinning_job
		(
			*l_skinned_mesh_component->get_skeleton(),
			l_skinned_mesh_component->get_mesh(),
			l_skinned_mesh_component->get_model_transforms()
		);
		l_skinning_job.run(platform::bc_clock::update_param(0, 0, 0));

		const physics::bc_triangle_mesh_desc l_triangle_mesh_desc
		(
			physics::bc_bounded_strided_typed_data<core::bc_vector3f>
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
		auto l_triangle_mesh = p_physics_system.get_physics().create_runtime_triangle_mesh(l_triangle_mesh_desc);

		/*auto l_mat = p_physics_system.get_physics().create_material(10, 10, 0.1);
			const_cast<physics::bc_rigid_actor&>(p_filter_data.m_actor).create_shape
			(
				physics::bc_shape_triangle_mesh(l_triangle_mesh.get()),
				l_mat.get(),
				physics::bc_shape_flag::visualization
			);*/
			
		physics::bc_ray_hit l_hit;
		const auto l_hit_count = physics::bc_shape_query::ray_cast
		(
			p_ray,
			physics::bc_shape_triangle_mesh(l_triangle_mesh.get()),
			physics::bc_transform(l_mediate_component->get_world_transform()),
			physics::bc_hit_flag::hit_info,
			&l_hit,
			1
		);

		return std::make_pair(l_hit_count > 0 ? l_shape_query_type : physics::bc_query_hit_type::none, l_hit);
	}
		
	void bc_mesh_utility::calculate_mesh_decal(const core::bc_vector3f& p_world_position,
	                                           const core::bc_vector3f& p_world_direction,
	                                           const core::bc_matrix4f& p_attached_node_world_transform,
	                                           core::bc_vector3f& p_decal_local_position,
	                                           core::bc_vector3f& p_decal_local_direction,
	                                           core::bc_matrix3f& p_decal_local_rotation,
	                                           core::bc_matrix4f& p_decal_world_transform)
	{
		// We should remove any scale transform because calculation is based on world position which has scaled accordingly
		auto l_node_world_transform = p_attached_node_world_transform;
		l_node_world_transform.make_neutralize_scale();
		const auto l_inv_node_world_transform = l_node_world_transform.inverse();

		p_decal_local_position = (l_inv_node_world_transform * core::bc_vector4f(p_world_position, 1)).xyz();
		p_decal_local_direction = core::bc_vector3f::normalize((l_inv_node_world_transform * core::bc_vector4f(p_world_direction, 0)).xyz());
		p_decal_world_transform = l_node_world_transform;
		p_decal_local_rotation = bc_matrix3f_rotation_between_two_vector(core::bc_vector3f::up(), p_decal_local_direction);
	}

	void bc_mesh_utility::calculate_skinned_mesh_decal(const core::bc_vector3f& p_world_position,
	                                                   const core::bc_vector3f& p_world_direction,
	                                                   const core::bc_matrix4f& p_attached_node_bind_pose_transform,
	                                                   const core::bc_matrix4f& p_attached_node_inv_bind_pose_transform,
	                                                   const core::bc_matrix4f& p_attached_node_model_transform,
	                                                   const core::bc_matrix4f& p_attached_actor_world_transform,
	                                                   core::bc_vector3f& p_decal_local_position,
	                                                   core::bc_vector3f& p_decal_local_direction,
	                                                   core::bc_matrix3f& p_decal_local_rotation,
	                                                   core::bc_matrix4f& p_decal_world_transform)
	{
		const auto l_inv_node_world_transform = (p_attached_node_model_transform * p_attached_actor_world_transform).inverse();

		p_decal_local_position = (l_inv_node_world_transform * core::bc_vector4f(p_world_position, 1)).xyz();
		p_decal_local_position = (p_attached_node_bind_pose_transform * core::bc_vector4f(p_decal_local_position, 1)).xyz();
		p_decal_local_direction = (l_inv_node_world_transform * core::bc_vector4f(p_world_direction, 0)).xyz();
		p_decal_local_direction = (p_attached_node_bind_pose_transform * core::bc_vector4f(p_decal_local_direction, 0)).xyz();
		p_decal_local_direction.normalize();
		p_decal_world_transform = p_attached_node_inv_bind_pose_transform * (p_attached_node_model_transform * p_attached_actor_world_transform);
		p_decal_local_rotation = bc_matrix3f_rotation_between_two_vector(core::bc_vector3f::up(), p_decal_local_direction);
	}

	void bc_mesh_utility::clear_mesh_render_states_cache()
	{
		m_mesh_render_states = core::bc_concurrent_lazy<mesh_render_state_container>();
	}
}
