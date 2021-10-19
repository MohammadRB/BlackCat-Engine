// [10/19/2021 MRB]

#include "Core/CorePCH.h"
#include "Core/Math/bcVector4f.h"
#include "Core/Utility/bcLoggerOutputStream.h"

namespace black_cat
{
	namespace core
	{
		bc_logger_output_stream& operator<<(bc_logger_output_stream& p_stream, const bc_vector4f& p_vector)
		{
			return p_stream << p_vector.x << " " << p_vector.y << " " << p_vector.z << " " << p_vector.w;
		}

		bc_logger_output_stream& operator<<(bc_logger_output_stream& p_stream, const bc_vector4i& p_vector)
		{
			return p_stream << p_vector.x << " " << p_vector.y << " " << p_vector.z << " " << p_vector.w;
		}
	}
}