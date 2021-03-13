// [04/16/2016 MRB]

#include "PlatformImp/PlatformImpPCH.h"
#include "PlatformImp/Application/bcHumanInterfaceDevice.h"
#include "Platform/bcException.h"

namespace black_cat
{
	namespace platform
	{
		inline bcUBYTE bc_human_interface_device::get_key_device_count()
		{
			return 1;
		}

		inline bcUBYTE bc_human_interface_device::get_pointing_device_count()
		{
			return 1;
		}

		inline bcUBYTE bc_human_interface_device::get_accelerometer_device_count()
		{
			return 0;
		}

		inline bc_key_device bc_human_interface_device::create_key_device(bcUBYTE p_index)
		{
			if (p_index > get_key_device_count() - 1)
			{
				throw bc_invalid_argument_exception("Invalid device index");
			}

			return bc_key_device(p_index);
		}

		inline bc_pointing_device bc_human_interface_device::create_pointing_device(bcUBYTE p_index)
		{
			if (p_index > get_pointing_device_count() - 1)
			{
				throw bc_invalid_argument_exception("Invalid device index");
			}

			return bc_pointing_device(p_index);
		}
	}
}