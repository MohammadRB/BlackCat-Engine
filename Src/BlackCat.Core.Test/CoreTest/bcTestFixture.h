// [06/13/2021 MRB]

#pragma once

#include "Core/Memory/bcMemoryManagment.h"
#include "Core/Utility/bcServiceManager.h"

namespace black_cat
{
	inline void bc_test_init_memory()
	{
		core::bc_memory_manager::startup(8, 16, 10, 16, 128, 1024, 1024, 1024);
	}

	inline void bc_test_close_memory()
	{
		core::bc_memory_manager::close();
	}

	inline void bc_test_init()
	{
		bc_test_init_memory();
		core::bc_service_manager::start_up();
	}

	inline void bc_test_close()
	{
		core::bc_service_manager::close();
		bc_test_close_memory();
	}
}
