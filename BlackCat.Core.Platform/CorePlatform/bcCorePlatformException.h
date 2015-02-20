// [11/11/2014 MRB]

#pragma once

#include <system_error>
#include "coreplatform/coreplatformpch.h"
#include "coreplatform/bctype.h"

namespace black_cat
{
	namespace core_platform
	{
		class bc_thread_resource_exception : public std::runtime_error
		{
		public:
			explicit bc_thread_resource_exception(const bcCHAR* p_message) : std::runtime_error(p_message)
			{
			}

			bc_thread_resource_exception(const bc_thread_resource_exception& p_other) : std::runtime_error(p_other)
			{
			}

			~bc_thread_resource_exception() = default;

		protected:

		private:
		};
	}
}