// [11/4/2014 MRB]

#pragma once

#include "coreplatform/coreplatformpch.h"
#include "coreplatform/bctype.h"
#include "coreplatform/bcplatform.h"

namespace black_cat
{
	namespace core_platform
	{
		enum class bc_processor_architecture
		{
			x86,
			x64,
			ARM,
			unknown
		};

		struct bc_basic_hardware_info
		{
		public:
			bc_processor_architecture processor_architecture;
			bcSIZE proccessor_count;
			bcSIZE page_size;
			bcUINT64 memory_size;
			void* app_min_address;
			void* app_max_address;
		};

		template< bc_platform TPlatform >
		class bc_platform_hardware_info
		{
		public:
			static void get_basic_info(bc_basic_hardware_info* p_info);

		protected:

		private:

		};

		using bc_hardware_info = bc_platform_hardware_info< g_current_platform >;
	}
}