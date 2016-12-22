// [12/12/2016 MRB]

#include "PhysicsImp/PhysicsImpPCH.h"
#include "Core/Memory/bcAlloc.h"
#include "PhysicsImp/bcUtility.h"

namespace black_cat
{
	namespace physics
	{
		physx::PxHeightFieldDesc bc_convert_to_px_height_field(const bc_height_field_desc& p_desc)
		{
			physx::PxHeightFieldSample* l_px_samples = static_cast<physx::PxHeightFieldSample*>
			(
				bcAlloc(sizeof(physx::PxHeightFieldSample) * (p_desc.m_num_row * p_desc.m_num_column), core::bc_alloc_type::frame)
			);

			bcUINT32 l_source_index = 0;
			for (bcUINT32 x = p_desc.m_num_row - 1; x >= 0; --x)
			{
				for (bcUINT32 z = 0; z < p_desc.m_num_column; ++z)
				{
					auto l_target_index = x * p_desc.m_num_row + z;

					bcINT16 l_height = p_desc.m_samples.at< bcINT16 >(l_source_index);
					physx::PxBitAndByte l_material_index(p_desc.m_samples_material.at< bc_material_index >(l_source_index));

					l_px_samples[l_target_index] = physx::PxHeightFieldSample
					{
						l_height,
						l_material_index,
						l_material_index
					};

					++l_source_index;
				}
			}

			physx::PxHeightFieldDesc l_px_desx;
			l_px_desx.nbRows = p_desc.m_num_row;
			l_px_desx.nbColumns = p_desc.m_num_column;
			l_px_desx.samples.data = l_px_samples;
			l_px_desx.samples.stride = sizeof(physx::PxHeightFieldSample);

			return l_px_desx;
		}
	}
}