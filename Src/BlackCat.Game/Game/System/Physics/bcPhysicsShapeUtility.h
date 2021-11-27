// [01/18/2017 MRB]

#pragma once

#include "Core/Math/bcVector3f.h"
#include "Core/Math/bcMatrix4f.h"
#include "PhysicsImp/Shape/bcShapeSphere.h"
#include "PhysicsImp/Shape/bcShapeBox.h"
#include "PhysicsImp/Shape/bcShapeCapsule.h"
#include "PhysicsImp/Shape/bcShapeConvexMesh.h"
#include "PhysicsImp/Shape/bcShapeTriangleMesh.h"
#include "PhysicsImp/Shape/bcBoundBox.h"
#include "PhysicsImp/Shape/bcShape.h"
#include "PhysicsImp/Fundation/bcTransform.h"
#include "PhysicsImp/Fundation/bcPhysics.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		/**
		* \brief Extract surrounding sphere for a given set of points. Points must be defined with identity transformation.
		* \param p_vertices
		* \return
		*/
		physics::bc_shape_sphere BC_GAME_DLL bc_extract_sphere_from_points(physics::bc_bounded_strided_typed_data<core::bc_vector3f> p_vertices);

		/**
		 * \brief Extract surrounding box for a given set of points. Points must be defined with identity transformation.
		 * \param p_vertices 
		 * \return 
		 */
		physics::bc_shape_box BC_GAME_DLL bc_extract_box_from_points(physics::bc_bounded_strided_typed_data<core::bc_vector3f> p_vertices);

		/**
		 * \brief Extract surrounding bound box for a given set of points. Points must be defined with identity transformation.
		 * \param p_vertices
		 * \return
		 */
		physics::bc_bound_box BC_GAME_DLL bc_extract_bound_box_from_points(physics::bc_bounded_strided_typed_data<core::bc_vector3f> p_vertices);

		/**
		* \brief Extract surrounding capsule for a given set of points. Points must be defined with identity transformation.
		* \param p_vertices
		* \return
		*/
		physics::bc_shape_capsule BC_GAME_DLL bc_extract_capsule_from_points(physics::bc_bounded_strided_typed_data<core::bc_vector3f> p_vertices);

		/**
		* \brief Extract surrounding convex mesh for a given set of points. Points must be defined with identity transformation.
		* \param p_vertices
		* \return
		*/
		physics::bc_convex_mesh_desc BC_GAME_DLL bc_extract_convex_from_points(physics::bc_bounded_strided_typed_data<core::bc_vector3f> p_vertices);

		/**
		* \brief Extract surrounding triangle mesh for a given set of points. Points must be defined with identity transformation.
		* \param p_vertices
		* \param p_indices
		* \return
		*/
		physics::bc_triangle_mesh_desc BC_GAME_DLL bc_extract_mesh_from_points(physics::bc_bounded_strided_typed_data<core::bc_vector3f> p_vertices, physics::bc_bounded_strided_data p_indices);

		physics::bc_bound_box BC_GAME_DLL bc_convert_shape_box_to_bound_box(const physics::bc_shape_box& p_shape, const physics::bc_transform& p_shape_transform);

		physics::bc_bound_box BC_GAME_DLL bc_convert_shape_box_to_bound_box(const physics::bc_shape_box& p_shape, const core::bc_matrix4f& p_shape_transform);

		physics::bc_shape_ref BC_GAME_DLL bc_copy_shape(physics::bc_physics& p_physics, physics::bc_shape p_shape);
	}
}