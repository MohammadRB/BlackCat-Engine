// [19/10/2021 MRB]

#include "Core/CorePCH.h"
#include "Core/Math/bcVector3f.h"
#include "Core/Utility/bcLoggerOutputStream.h"

namespace black_cat::core
{
	bc_logger_output_stream& operator<<(bc_logger_output_stream& p_stream, const bc_vector3f& p_vector)
	{
		return p_stream << p_vector.x << " " << p_vector.y << " " << p_vector.z;
	}

	bc_logger_output_stream& operator<<(bc_logger_output_stream& p_stream, const bc_vector3i& p_vector)
	{
		return p_stream << p_vector.x << " " << p_vector.y << " " << p_vector.z;
	}
}
