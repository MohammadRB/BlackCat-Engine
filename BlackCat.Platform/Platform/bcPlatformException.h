// [2/27/2015 MRB]

#pragma once

#include "Platform/PlatformPCH.h"
#include "Core/CoreExceptions.h"

namespace black_cat
{
	namespace platform
	{
		class bc_platform_resource_exception : std::runtime_error
		{
		public:
			bc_platform_resource_exception(const bcCHAR* p_message) : runtime_error(p_message)
			{
			}

			bc_platform_resource_exception(const bc_platform_resource_exception& p_other) : runtime_error(p_other)
			{
			}

			~bc_platform_resource_exception() = default;

		protected:

		private:
		};
	}
}