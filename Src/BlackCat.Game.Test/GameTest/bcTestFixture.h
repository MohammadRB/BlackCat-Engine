// [06/13/2021 MRB]

#pragma once

#include "Core/Memory/bcMemoryManagment.h"
#include "Core/Utility/bcServiceManager.h"
#include "PlatformImp/Network/bcNetworkDefinitions.h"

namespace black_cat
{
	inline void bc_test_init_socket_library()
	{
		platform::bc_initialize_socket_library();
	}

	inline void bc_test_close_socket_library()
	{
		platform::bc_cleanup_socket_library();
	}

	inline void bc_test_init()
	{
		core::bc_memory_manager::startup(8, 32, 10, 16, 128, 1024, 1024, 1024);
		core::bc_service_manager::start_up();
		core::bc_register_service(core::bc_make_service<core::bc_logger>());
	}

	inline void bc_test_close()
	{
		core::bc_service_manager::close();
		core::bc_memory_manager::close();
	}
}