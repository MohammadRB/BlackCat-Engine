// [12/10/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Math/bcCoordinate.h"
#include "GraphicImp/bcRenderApiInfo.h"
//#include "Physics/Shape/bcShapeGeometry.h"
//#include "Physics/Shape/bcShapeSphere.h"
//#include "Physics/Shape/bcShapeBox.h"
//#include "Physics/Shape/bcShapeCapsule.h"
//#include "Physics/Shape/bcShapePlane.h"
//#include "Physics/Shape/bcShapeConvexMesh.h"
//#include "Physics/Shape/bcShapeTriangleMesh.h"
//#include "Physics/Shape/bcShapeHeightField.h"
#include "Physics/bcUtility.h"
#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/Shape/bcConvexMesh.h"
#include "PhysicsImp/Shape/bcTriangleMesh.h"
#include "PhysicsImp/Shape/bcHeightField.h"

namespace black_cat
{
	namespace physics
	{
		template< class TBcType, class TPxType, class TInitializer >
		void bc_overwrite_output_array(TBcType* p_buffer, bcUINT32 p_element_count, TInitializer p_initializer)
		{
			auto* l_px_types = reinterpret_cast< TPxType** >(p_buffer);

			// Initialize objects in reverse order to prevent data overwrite(in case of wrapper objects are bigger) 
			// when writing to the array
			for (bcINT32 i = p_element_count - 1; i >= 0; --i)
			{
				TPxType* l_px_type = l_px_types[i];
				TBcType* l_bc_type = &p_buffer[i];

				p_initializer(l_bc_type, l_px_type);
			}
		}

		physx::PxHeightFieldDesc bc_convert_to_px_height_field(const bc_height_field_desc& p_desc, physx::PxHeightFieldSample* p_px_sample_buffer);

		physx::PxConvexMeshDesc bc_convert_to_px_convex_mesh(const bc_convex_mesh_desc& p_desc, physx::PxVec3* p_px_point_buffer);

		physx::PxTriangleMeshDesc bc_convert_to_px_triangle_mesh(const bc_triangle_mesh_desc& p_desc, physx::PxVec3* p_px_point_buffer, bcBYTE* p_px_index_buffer);

		inline physx::PxVec3 bc_to_right_hand(const core::bc_vector3f& p_vector)
		{
			if(graphic::bc_render_api_info::is_left_handed())
			{
				auto l_vec = p_vector;

				core::bc_to_right_hand(l_vec);

				return physx::PxVec3(l_vec.x, l_vec.y, l_vec.z);
			}

			return physx::PxVec3(p_vector.x, p_vector.y, p_vector.z);
		}

		inline core::bc_vector3f bc_to_game_hand(const physx::PxVec3& p_vector)
		{
			if(graphic::bc_render_api_info::is_left_handed())
			{
				core::bc_vector3f l_vec(p_vector.x, p_vector.y, p_vector.z);

				core::bc_to_left_hand(l_vec);

				return l_vec;
			}

			return core::bc_vector3f(p_vector.x, p_vector.y, p_vector.z);
		}

		inline physx::PxMat33 bc_to_right_hand(const core::bc_matrix3f& p_mat)
		{
			if (graphic::bc_render_api_info::is_left_handed())
			{
				auto l_mat = p_mat;

				core::bc_to_right_hand(l_mat);

				return physx::PxMat33
				(
					physx::PxVec3(l_mat[0], l_mat[1], l_mat[2]),
					physx::PxVec3(l_mat[3], l_mat[4], l_mat[5]),
					physx::PxVec3(l_mat[6], l_mat[7], l_mat[8])
				);
			}

			return physx::PxMat33
			(
				physx::PxVec3(p_mat[0], p_mat[1], p_mat[2]),
				physx::PxVec3(p_mat[3], p_mat[4], p_mat[5]),
				physx::PxVec3(p_mat[6], p_mat[7], p_mat[8])
			);
		}

		inline core::bc_matrix3f bc_to_game_hand(const physx::PxMat33& p_mat)
		{
			if (graphic::bc_render_api_info::is_left_handed())
			{
				core::bc_matrix3f l_mat
				(
					p_mat.column0.x, p_mat.column0.y, p_mat.column0.z,
					p_mat.column1.x, p_mat.column1.y, p_mat.column1.z,
					p_mat.column2.x, p_mat.column2.y, p_mat.column2.z
				);

				core::bc_to_left_hand(l_mat);

				return l_mat;
			}

			return core::bc_matrix3f
			(
				p_mat.column0.x, p_mat.column0.y, p_mat.column0.z,
				p_mat.column1.x, p_mat.column1.y, p_mat.column1.z,
				p_mat.column2.x, p_mat.column2.y, p_mat.column2.z
			);
		}

		inline physx::PxMat44 bc_to_right_hand(const core::bc_matrix4f& p_mat)
		{
			if (graphic::bc_render_api_info::is_left_handed())
			{
				auto l_mat = p_mat;

				core::bc_to_right_hand(l_mat);

				return physx::PxMat44
				(
					physx::PxVec4(l_mat[0], l_mat[1], l_mat[2], l_mat[3]),
					physx::PxVec4(l_mat[4], l_mat[5], l_mat[6], l_mat[7]),
					physx::PxVec4(l_mat[8], l_mat[9], l_mat[10], l_mat[11]),
					physx::PxVec4(l_mat[12], l_mat[13], l_mat[14], l_mat[15])
				);
			}

			return physx::PxMat44
			(
				physx::PxVec4(p_mat[0], p_mat[1], p_mat[2], p_mat[3]),
				physx::PxVec4(p_mat[4], p_mat[5], p_mat[6], p_mat[7]),
				physx::PxVec4(p_mat[8], p_mat[9], p_mat[10], p_mat[11]),
				physx::PxVec4(p_mat[12], p_mat[13], p_mat[14], p_mat[15])
			);
		}

		inline core::bc_matrix4f bc_to_game_hand(const physx::PxMat44& p_mat)
		{
			if (graphic::bc_render_api_info::is_left_handed())
			{
				core::bc_matrix4f l_mat
				(
					p_mat.column0.x, p_mat.column0.y, p_mat.column0.z, p_mat.column0.w,
					p_mat.column1.x, p_mat.column1.y, p_mat.column1.z, p_mat.column1.w,
					p_mat.column2.x, p_mat.column2.y, p_mat.column2.z, p_mat.column2.w,
					p_mat.column3.x, p_mat.column3.y, p_mat.column3.z, p_mat.column3.w
				);

				core::bc_to_left_hand(l_mat);

				return l_mat;
			}

			return core::bc_matrix4f
			(
				p_mat.column0.x, p_mat.column0.y, p_mat.column0.z, p_mat.column0.w,
				p_mat.column1.x, p_mat.column1.y, p_mat.column1.z, p_mat.column1.w,
				p_mat.column2.x, p_mat.column2.y, p_mat.column2.z, p_mat.column2.w,
				p_mat.column3.x, p_mat.column3.y, p_mat.column3.z, p_mat.column3.w
			);
		}
	}
}