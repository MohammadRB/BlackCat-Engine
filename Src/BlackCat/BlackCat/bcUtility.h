// [01/18/2017 MRB]

#pragma once

#include "PhysicsImp/bcUtility.h"
#include "PhysicsImp/Shape/bcShapeSphere.h"
#include "PhysicsImp/Shape/bcShapeBox.h"
#include "PhysicsImp/Shape/bcShapeCapsule.h"
#include "PhysicsImp/Shape/bcShapeConvexMesh.h"
#include "PhysicsImp/Shape/bcShapeTriangleMesh.h"

namespace black_cat
{
	/**
	* \brief Extract surrounding sphere for a given set of points. Points must be defined with identity transformation.
	* \param p_vertices
	* \return
	*/
	physics::bc_shape_sphere bc_extract_sphere_from_points(physics::bc_bounded_strided_typed_data< core::bc_vector3f > p_vertices);

	/**
	 * \brief Extract surrounding box for a given set of points. Points must be defined with identity transformation.
	 * \param p_vertices 
	 * \return 
	 */
	physics::bc_shape_box bc_extract_box_from_points(physics::bc_bounded_strided_typed_data< core::bc_vector3f > p_vertices);

	/**
	* \brief Extract surrounding capsule for a given set of points. Points must be defined with identity transformation.
	* \param p_vertices
	* \return
	*/
	physics::bc_shape_capsule bc_extract_capsule_from_points(physics::bc_bounded_strided_typed_data< core::bc_vector3f > p_vertices);

	/**
	* \brief Extract surrounding convex mesh for a given set of points. Points must be defined with identity transformation.
	* \param p_vertices
	* \return
	*/
	physics::bc_convex_mesh_desc bc_extract_convex_from_points(physics::bc_bounded_strided_typed_data< core::bc_vector3f > p_vertices);

	/**
	* \brief Extract surrounding triangle mesh for a given set of points. Points must be defined with identity transformation.
	* \param p_vertices
	* \return
	*/
	physics::bc_triangle_mesh_desc bc_extract_mesh_from_points(physics::bc_bounded_strided_typed_data< core::bc_vector3f > p_vertices, 
		physics::bc_bounded_strided_data p_indices);
}