// [01/23/2021 MRB]

#pragma once

#include "PhysicsImp/Shape/bcBoundBox.h"
#include "Game/bcExport.h"
#include "Game/Object/Mesh/bcMesh.h"

namespace black_cat
{
	namespace game
	{
		class bc_mesh_node;
		class bc_sub_mesh_transform;

		class BC_GAME_DLL bc_mesh_utility
		{
		public:
			static void calculate_bound_box(const bc_mesh& p_mesh, const bc_sub_mesh_transform& p_transforms, physics::bc_bound_box& p_bound_box) noexcept;

			static void calculate_absolute_transforms(const bc_mesh& p_mesh, const core::bc_matrix4f& p_world, bc_sub_mesh_transform& p_transforms, physics::bc_bound_box& p_bound_box) noexcept;

			static void calculate_skinned_mesh_colliders_inverse_bind_pose(const bc_mesh& p_mesh, const bc_sub_mesh_transform& p_model_transforms, bc_sub_mesh_transform& p_transforms);
			
			/**
			 * \brief
			 * \tparam TArg
			 * \tparam TCallable
			 * \param p_mesh
			 * \param p_arg Root argument
			 * \param p_callable Callable object (const bc_mesh_node&, TArg&) -> TArg
			 * \param p_node
			 */
			template< typename TArg, typename TCallable >
			static void iterate_over_nodes(const bc_mesh& p_mesh, TArg& p_arg, TCallable p_callable, const bc_mesh_node* p_node = nullptr) noexcept;

		private:
			template< typename TArg, typename TCallable >
			static void _iterate_over_nodes(const bc_mesh& p_mesh, TArg& p_arg, TCallable& p_callable, const bc_mesh_node* const* p_begin, const bc_mesh_node* const* p_end) noexcept;
		};

		template< typename TArg, typename TCallable >
		void bc_mesh_utility::iterate_over_nodes(const bc_mesh& p_mesh, TArg& p_arg, TCallable p_callable, const bc_mesh_node* p_node) noexcept
		{
			const bc_mesh_node* l_root_pointer = p_node ? p_node : p_mesh.get_root();
			bc_mesh_utility::_iterate_over_nodes(p_mesh, p_arg, p_callable, &l_root_pointer, &l_root_pointer + 1);
		}

		template< typename TArg, typename TCallable >
		void bc_mesh_utility::_iterate_over_nodes(const bc_mesh& p_mesh, TArg& p_arg, TCallable& p_callable, const bc_mesh_node* const* p_begin, const bc_mesh_node* const* p_end)  noexcept
		{
			// TODO use non-recursive version if possible
			for (; p_begin != p_end; ++p_begin)
			{
				const bc_mesh_node* l_node = *p_begin;
				TArg l_arg = p_callable(*l_node, p_arg);

				const auto& l_node_children = p_mesh.get_node_children(*l_node);
				if (!l_node_children.empty())
				{
					const bc_mesh_node* const* l_begin = &l_node_children.front();
					const bc_mesh_node* const* l_end = &l_node_children.back() + 1;

					_iterate_over_nodes
					(
						p_mesh,
						l_arg,
						p_callable,
						l_begin,
						l_end
					);
				}
			}
		}
	}	
}