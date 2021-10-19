// [10/19/2021 MRB]

#include "Core/CorePCH.h"
#include "Core/Math/bcVector2f.h"
#include "Core/Utility/bcLoggerOutputStream.h"

namespace black_cat
{
	namespace core
	{
		bc_logger_output_stream& operator<<(bc_logger_output_stream& p_stream, const bc_vector2f& p_vector)
		{
			return p_stream << p_vector.x << " " << p_vector.y;
		}

		bc_logger_output_stream& operator<<(bc_logger_output_stream& p_stream, const bc_vector2i& p_vector)
		{
			return p_stream << p_vector.x << " " << p_vector.y;
		}
	}
}