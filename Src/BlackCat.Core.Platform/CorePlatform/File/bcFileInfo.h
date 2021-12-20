// [03/31/2016 MRB]

#pragma once

#include "CorePlatform/bcPlatform.h"
#include "CorePlatform/bcType.h"

namespace black_cat
{
	namespace core_platform
	{
		struct bc_file_time
		{
			bcUINT16 m_year;
			bcUINT16 m_month;
			bcUINT16 m_day_of_week;
			bcUINT16 m_day;
			bcUINT16 m_hour;
			bcUINT16 m_minute;
			bcUINT16 m_second;
			bcUINT16 m_milliseconds;
			bcUINT64 m_total_milliseconds;
		};

		struct bc_basic_file_info
		{
			bool m_exist;
			bc_file_time m_create_time;
			bc_file_time m_last_access_time;
			bc_file_time m_last_write_time;
			bcUINT64 m_size;
		};

		template<bc_platform TPlatform>
		class bc_platform_file_info
		{
		public:
			static bool exist(const bcECHAR* p_file_name);

			static bool get_basic_info(const bcECHAR* p_file_name, bc_basic_file_info* p_info);
		};

		using bc_file_info = bc_platform_file_info<g_current_platform>;
	}
}