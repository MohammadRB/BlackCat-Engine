// [10/12/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Core/Math/bcCoordinate.h"
#include "GraphicImp/bcRenderApiInfo.h"
#include "Physics/bcUtility.h"
#include "PhysicsImp/PhysicsImpPCH.h"
#include "PhysicsImp/Shape/bcConvexMesh.h"
#include "PhysicsImp/Shape/bcTriangleMesh.h"
#include "PhysicsImp/Shape/bcHeightField.h"

namespace black_cat
{
	namespace physics
	{
		
		/**
		 * \brief Reinterpret input buffer as another type in Px library and use TInitializer callback to overwrite every object in place
		 * \tparam TBcType 
		 * \tparam TPxType 
		 * \tparam TInitializer TBcType(TPxType&)
		 * \param p_buffer 
		 * \param p_element_count 
		 * \param p_initializer 
		 */
		template<class TBcType, class TPxType, class TInitializer>
		void bc_overwrite_output_array(TBcType* p_buffer, bcUINT32 p_element_count, TInitializer p_initializer)
		{
			auto* l_px_types = reinterpret_cast<TPxType*>(p_buffer);

			// Initialize objects in reverse order to prevent data overwrite(in case of wrapper objects are bigger) 
			// when writing to the array
			for (bcINT32 i = p_element_count - 1; i >= 0; --i)
			{
				TPxType& l_px_type = l_px_types[i];
				TBcType& l_bc_type = p_buffer[i];

				new (&l_bc_type) TBcType(p_initializer(l_px_type));
			}
		}

		physx::PxHeightFieldDesc bc_convert_to_px_height_field(const bc_height_field_desc& p_desc, physx::PxHeightFieldSample* p_px_sample_buffer);

		physx::PxConvexMeshDesc bc_convert_to_px_convex_mesh(const bc_convex_mesh_desc& p_desc, physx::PxVec3* p_px_point_buffer);

		physx::PxTriangleMeshDesc bc_convert_to_px_triangle_mesh(const bc_triangle_mesh_desc& p_desc, physx::PxVec3* p_px_point_buffer, bcBYTE* p_px_index_buffer);

		inline physx::PxVec3 bc_to_right_hand(const core::bc_vector3f& p_vector)
		{
			if constexpr (graphic::bc_render_api_info::use_left_handed())
			{
				return physx::PxVec3(p_vector.x, p_vector.y, -p_vector.z);
			}

			return physx::PxVec3(p_vector.x, p_vector.y, p_vector.z);
		}

		inline core::bc_vector3f bc_to_game_hand(const physx::PxVec3& p_vector)
		{
			if constexpr (graphic::bc_render_api_info::use_left_handed())
			{
				return core::bc_vector3f(p_vector.x, p_vector.y, -p_vector.z);
			}

			return core::bc_vector3f(p_vector.x, p_vector.y, p_vector.z);
		}

		inline physx::PxMat33 bc_to_right_hand(const core::bc_matrix3f& p_mat)
		{
			if constexpr (graphic::bc_render_api_info::use_left_handed())
			{
				return physx::PxMat33
				(
					physx::PxVec3(p_mat[0], p_mat[1], -p_mat[2]),
					physx::PxVec3(p_mat[3], p_mat[4], -p_mat[5]),
					physx::PxVec3(-p_mat[6], -p_mat[7], p_mat[8])
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
			if constexpr (graphic::bc_render_api_info::use_left_handed())
			{
				return core::bc_matrix3f
				(
					p_mat.column0.x, p_mat.column0.y, -p_mat.column0.z,
					p_mat.column1.x, p_mat.column1.y, -p_mat.column1.z,
					-p_mat.column2.x, -p_mat.column2.y, p_mat.column2.z
				);
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
			if constexpr (graphic::bc_render_api_info::use_left_handed())
			{
				return physx::PxMat44
				(
					physx::PxVec4(p_mat[0], p_mat[1], -p_mat[2], p_mat[3]),
					physx::PxVec4(p_mat[4], p_mat[5], -p_mat[6], p_mat[7]),
					physx::PxVec4(-p_mat[8], -p_mat[9], p_mat[10], p_mat[11]),
					physx::PxVec4(p_mat[12], p_mat[13], -p_mat[14], p_mat[15])
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
			if constexpr (graphic::bc_render_api_info::use_left_handed())
			{
				return core::bc_matrix4f
				(
					p_mat.column0.x, p_mat.column0.y, -p_mat.column0.z, p_mat.column0.w,
					p_mat.column1.x, p_mat.column1.y, -p_mat.column1.z, p_mat.column1.w,
					-p_mat.column2.x, -p_mat.column2.y, p_mat.column2.z, p_mat.column2.w,
					p_mat.column3.x, p_mat.column3.y, -p_mat.column3.z, p_mat.column3.w
				);
			}

			return core::bc_matrix4f
			(
				p_mat.column0.x, p_mat.column0.y, p_mat.column0.z, p_mat.column0.w,
				p_mat.column1.x, p_mat.column1.y, p_mat.column1.z, p_mat.column1.w,
				p_mat.column2.x, p_mat.column2.y, p_mat.column2.z, p_mat.column2.w,
				p_mat.column3.x, p_mat.column3.y, p_mat.column3.z, p_mat.column3.w
			);
		}

		inline physx::PxExtendedVec3 bc_to_right_hand_ex(const core::bc_vector3f& p_vector)
		{
			if constexpr (graphic::bc_render_api_info::use_left_handed())
			{
				return physx::PxExtendedVec3(p_vector.x, p_vector.y, -p_vector.z);
			}

			return physx::PxExtendedVec3(p_vector.x, p_vector.y, p_vector.z);
		}

		inline core::bc_vector3f bc_to_game_hand_ex(const physx::PxExtendedVec3& p_vector)
		{
			if constexpr (graphic::bc_render_api_info::use_left_handed())
			{
				return core::bc_vector3f(p_vector.x, p_vector.y, -p_vector.z);
			}

			return core::bc_vector3f(p_vector.x, p_vector.y, p_vector.z);
		}
	}
}