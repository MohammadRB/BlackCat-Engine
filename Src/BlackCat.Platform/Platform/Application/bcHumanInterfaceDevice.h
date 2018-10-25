// [04/15/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "Platform/bcExport.h"
#include "Platform/Application/bcKeyDevice.h"
#include "Platform/Application/bcPointingDevice.h"

namespace black_cat
{
	namespace platform
	{
		class BC_PLATFORMSHARED_DLL bc_human_interface_device
		{
		public:
			static bcUBYTE get_key_device_count();

			static bcUBYTE get_pointing_device_count();

			static bcUBYTE get_accelerometer_device_count();

			// Create a 'key device' that represent any device with key, like keyborad
			static bc_key_device create_key_device(bcUBYTE p_index);

			// Creare a 'pointing device' that represent any device with the ability to point to objects
			// like mouse or touch screens(note that mouse key events will be listened by bc_key_device)
			static bc_pointing_device create_pointing_device(bcUBYTE p_index);

			// Create a 'accelerometer device' that represent any device with analog sensors
			//static bc_accelerometer_device create_accelerometer_device(bcUINT p_index);
		};
	}
}