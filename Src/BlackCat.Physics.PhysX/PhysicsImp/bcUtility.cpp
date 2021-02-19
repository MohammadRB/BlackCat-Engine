// [12/12/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"

#include "Core/Utility/bcLogger.h"
#include "Core/Utility/bcEnumOperand.h"
#include "PhysicsImp/bcUtility.h"

namespace black_cat
{
	namespace physics
	{
		physx::PxHeightFieldDesc bc_convert_to_px_height_field(const bc_height_field_desc& p_desc, physx::PxHeightFieldSample* p_px_sample_buffer)
		{
			bool l_has_invalid_material = false;
			const bool l_has_material = p_desc.m_samples_material.m_data;
			bcUINT32 l_source_index = 0;

			for (bcUINT32 x = 0; x < p_desc.m_num_row; ++x)
			{
				for (bcUINT32 z = 0; z < p_desc.m_num_column; ++z)
				{
					const auto l_zx_index = z * p_desc.m_num_row + x;

					const bcINT16 l_height = p_desc.m_samples.at(l_source_index);
					const bc_material_index l_material = l_has_material ? p_desc.m_samples_material.at(l_source_index) : 0;
					if(l_material > 127)
					{
						l_has_invalid_material = true;
					}
					
					const physx::PxBitAndByte l_material_index(l_material);

					p_px_sample_buffer[l_zx_index] = physx::PxHeightFieldSample
					{
						l_height,
						l_material_index,
						l_material_index
					};

					++l_source_index;
				}
			}

			physx::PxHeightFieldDesc l_px_desc;
			l_px_desc.nbRows = p_desc.m_num_row;
			l_px_desc.nbColumns = p_desc.m_num_column;
			l_px_desc.samples.data = p_px_sample_buffer;
			l_px_desc.samples.stride = sizeof(physx::PxHeightFieldSample);

			if(l_has_invalid_material)
			{
				core::bc_get_service<core::bc_logger>()->log_debug(bcL("Terrain material indices higher that 127 is not supported."));
			}
			
			return l_px_desc;
		}

		physx::PxConvexMeshDesc bc_convert_to_px_convex_mesh(const bc_convex_mesh_desc& p_desc, physx::PxVec3* p_px_point_buffer)
		{
			for(bcUINT32 i = 0; i < p_desc.m_points.m_count; ++i)
			{
				const auto& l_point = p_desc.m_points.at(i);

				p_px_point_buffer[i] = bc_to_right_hand(l_point);
			}

			physx::PxConvexMeshDesc l_px_desc;
			l_px_desc.points.data = p_px_point_buffer;
			l_px_desc.points.stride = sizeof(physx::PxVec3);
			l_px_desc.points.count = p_desc.m_points.m_count;
			l_px_desc.vertexLimit = p_desc.m_vertex_limit;
			l_px_desc.flags = static_cast<physx::PxConvexFlag::Enum>(p_desc.m_flags);

			return l_px_desc;
		}

		physx::PxTriangleMeshDesc bc_convert_to_px_triangle_mesh(const bc_triangle_mesh_desc& p_desc, physx::PxVec3* p_px_point_buffer, bcBYTE* p_px_index_buffer)
		{
			for (bcUINT32 i = 0; i < p_desc.m_points.m_count; ++i)
			{
				const auto& l_point = p_desc.m_points.at(i);

				p_px_point_buffer[i] = bc_to_right_hand(l_point);
			}

			if(core::bc_enum::has(p_desc.m_flag, bc_triangle_mesh_flag::use_16bit_index))
			{
				auto* l_indices = reinterpret_cast<bcUINT16*>(p_px_index_buffer);

				for(bcUINT32 i = 0; i < p_desc.m_indices.m_count / 3; ++i)
				{
					const auto l_index = i * 3;

					l_indices[l_index] = p_desc.m_indices.at<bcUINT16>(l_index + 2);
					l_indices[l_index + 1] = p_desc.m_indices.at<bcUINT16>(l_index + 1);
					l_indices[l_index + 2] = p_desc.m_indices.at<bcUINT16>(l_index);
				}
			}
			else
			{
				auto* l_indices = reinterpret_cast<bcUINT32*>(p_px_index_buffer);

				for (bcUINT32 i = 0; i < p_desc.m_indices.m_count / 3; ++i)
				{
					const auto l_index = i * 3;

					l_indices[l_index] = p_desc.m_indices.at<bcUINT32>(l_index + 2);
					l_indices[l_index + 1] = p_desc.m_indices.at<bcUINT32>(l_index + 1);
					l_indices[l_index + 2] = p_desc.m_indices.at<bcUINT32>(l_index);
				}
			}

			physx::PxTriangleMeshDesc l_px_desc;
			l_px_desc.points.data = p_px_point_buffer;
			l_px_desc.points.stride = sizeof(physx::PxVec3);
			l_px_desc.points.count = p_desc.m_points.m_count;
			l_px_desc.triangles.data = p_px_index_buffer;
			l_px_desc.triangles.stride = p_desc.m_indices.m_stride * 3;
			l_px_desc.triangles.count = p_desc.m_indices.m_count / 3;
			l_px_desc.materialIndices.data = static_cast< const physx::PxMaterialTableIndex* >(p_desc.m_material_indices.m_data);
			l_px_desc.materialIndices.stride = p_desc.m_material_indices.m_stride;
			l_px_desc.flags = static_cast< physx::PxMeshFlag::Enum >(p_desc.m_flag);

			return l_px_desc;
		}
	}
}