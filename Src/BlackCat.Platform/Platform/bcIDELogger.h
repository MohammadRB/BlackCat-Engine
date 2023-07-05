// [23/03/2016 MRB]

#pragma once

#include "Core/Utility/bcLogger.h"
#include "Platform/bcExport.h"

namespace black_cat::platform
{
	class BC_PLATFORMSHARED_DLL bc_ide_logger : public core::bci_log_listener
	{
	public:
		bc_ide_logger();

		~bc_ide_logger();

		void on_log(core::bc_log_type p_type, const bcECHAR* p_log) override;
	};
}
