// [23/01/2021 MRB]

#pragma once

#include "Core/Container/bcVector.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Utility/bcLazy.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "PhysicsImp/Collision/bcShapeQuery.h"
#include "PhysicsImp/Collision/bcSceneQuery.h"
#include "Game/System/Render/bcRenderInstance.h"
#include "Game/Object/Mesh/bcMesh.h"
#include "Game/Object/Mesh/bcSubMeshTransform.h"
#include "Game/Object/Mesh/bcMeshRenderState.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace core
	{
		class bc_json_key_value;
	}
	
	namespace game
	{
		class bc_mesh_node;
		class bc_render_state_buffer;
		class bc_render_system;
		class bc_physics_system;

		class BC_GAME_DLL bc_mesh_utility
		{
			using mesh_render_state_container = core::bc_unordered_map_program<core::bc_string, std::pair<bc_render_state_ptr, bc_mesh_material_ptr>>;
			
		public:
			static bc_mesh_render_state create_mesh_render_states(bc_render_system& p_render_system, 
				const bc_mesh& p_mesh, 
				const bc_mesh_node& p_root_node, 
				const bcCHAR* p_mesh_prefix = nullptr);
			
			static bc_mesh_render_state create_mesh_render_states(bc_render_system& p_render_system,
				const bc_mesh& p_mesh, 
				const bc_mesh_node& p_root_node, 
				const core::bc_json_key_value& p_mesh_materials, 
				const bcCHAR* p_mesh_prefix = nullptr);

			static void render_mesh(bc_render_state_buffer& p_buffer, 
				const bc_mesh_render_state& p_render_states, 
				const bc_sub_mesh_mat4_transform& p_transformations, 
				bcUINT32 p_lod,
				bc_actor_render_group p_group);
			
			static void render_skinned_mesh(bc_render_state_buffer& p_buffer, 
				const bc_mesh_render_state& p_render_states, 
				const bc_sub_mesh_mat4_transform& p_transformations, 
				bcUINT32 p_lod,
				bc_actor_render_group p_group);
			
			static void calculate_bound_box(const bc_mesh& p_mesh, 
				const bc_sub_mesh_mat4_transform& p_world_transforms, 
				physics::bc_bound_box& p_bound_box) noexcept;

			static void calculate_absolute_transforms(const bc_mesh& p_mesh, 
				const core::bc_matrix4f& p_world,
				bc_sub_mesh_mat4_transform& p_world_transforms, 
				physics::bc_bound_box& p_bound_box) noexcept;

			static void calculate_mesh_collider_transforms(const bc_mesh& p_mesh, 
				const bc_sub_mesh_mat4_transform& p_model_transforms, 
				bc_sub_mesh_px_transform& p_transforms);
			
			static void calculate_skinned_mesh_collider_transforms(const bc_mesh& p_mesh, 
				const bc_sub_mesh_mat4_transform& p_model_transforms, 
				bc_sub_mesh_px_transform& p_transforms);

			static std::pair<physics::bc_query_hit_type, physics::bc_ray_hit> skinned_mesh_ray_hit_test(bc_physics_system& p_physics_system,
				const physics::bc_ray& p_ray,
				const physics::bc_actor& p_actor,
				const physics::bc_shape& p_shape);
			
			static void calculate_mesh_decal(const core::bc_vector3f& p_world_position,
				const core::bc_vector3f& p_world_direction,
				const core::bc_matrix4f& p_attached_node_world_transform,
				core::bc_vector3f& p_decal_local_position,
				core::bc_vector3f& p_decal_local_direction,
				core::bc_matrix3f& p_decal_local_rotation,
				core::bc_matrix4f& p_decal_world_transform);

			static void calculate_skinned_mesh_decal(const core::bc_vector3f& p_world_position,
				const core::bc_vector3f& p_world_direction,
				const core::bc_matrix4f& p_attached_node_bind_pose_transform,
				const core::bc_matrix4f& p_attached_node_inv_bind_pose_transform,
				const core::bc_matrix4f& p_attached_node_model_transform,
				const core::bc_matrix4f& p_attached_actor_world_transform,
				core::bc_vector3f& p_decal_local_position,
				core::bc_vector3f& p_decal_local_direction,
				core::bc_matrix3f& p_decal_local_rotation,
				core::bc_matrix4f& p_decal_world_transform);
			
			/**
			 * \brief
			 * \tparam TArg
			 * \tparam TVisitFunc
			 * \tparam TUnVisitFunc
			 * \param p_mesh
			 * \param p_arg Root argument
			 * \param p_visit_func Callable object (const bc_mesh_node&, TArg&) -> TArg
			 * \param p_unvisit_func Callable object (const bc_mesh_node&, const TArg&) -> void
			 * \param p_start_node
			 */
			template<typename TArg, typename TVisitFunc, typename TUnVisitFunc>
			static void iterate_over_nodes(const bc_mesh& p_mesh,
				TArg& p_arg,
				TVisitFunc p_visit_func,
				TUnVisitFunc p_unvisit_func,
				const bc_mesh_node* p_start_node = nullptr) noexcept;

			static void clear_mesh_render_states_cache();
			
		private:
			template<typename TArg, typename TVisitFunc, typename TUnVisitFunc>
			static void _iterate_over_nodes(const bc_mesh& p_mesh, 
				TArg& p_arg, 
				TVisitFunc& p_visit_func,
				TUnVisitFunc p_unvisit_func,
				const bc_mesh_node* const* p_begin, 
				const bc_mesh_node* const* p_end) noexcept;

			static core::bc_concurrent_lazy<mesh_render_state_container> m_mesh_render_states;
		};

		template<typename TArg, typename TVisitFunc, typename TUnVisitFunc>
		void bc_mesh_utility::iterate_over_nodes(const bc_mesh& p_mesh, 
			TArg& p_arg, 
			TVisitFunc p_visit_func,
			TUnVisitFunc p_unvisit_func, 
			const bc_mesh_node* p_start_node) noexcept
		{
			const bc_mesh_node* l_root_pointer = p_start_node ? p_start_node : p_mesh.get_root();
			bc_mesh_utility::_iterate_over_nodes(p_mesh, p_arg, p_visit_func, p_unvisit_func, &l_root_pointer, &l_root_pointer + 1);
		}

		template<typename TArg, typename TVisitFunc, typename TUnVisitFunc>
		void bc_mesh_utility::_iterate_over_nodes(const bc_mesh& p_mesh,
			TArg& p_arg,
			TVisitFunc& p_visit_func,
			TUnVisitFunc p_unvisit_func, 
			const bc_mesh_node* const* p_begin,
			const bc_mesh_node* const* p_end)  noexcept
		{
			// TODO use non-recursive version if possible
			for (; p_begin != p_end; ++p_begin)
			{
				const bc_mesh_node* l_node = *p_begin;
				TArg l_arg = p_visit_func(*l_node, p_arg);

				const auto& l_node_children = p_mesh.get_node_children(*l_node);
				if (!l_node_children.empty())
				{
					const bc_mesh_node* const* l_begin = &l_node_children.front();
					const bc_mesh_node* const* l_end = &l_node_children.back() + 1;

					_iterate_over_nodes
					(
						p_mesh,
						l_arg,
						p_visit_func,
						p_unvisit_func,
						l_begin,
						l_end
					);
				}

				p_unvisit_func(*l_node, p_arg);
			}
		}
	}
}