// [03/23/2016 MRB]

#pragma once

#include "Core/Utility/bcLogger.h"
#include "Platform/bcExport.h"

namespace black_cat
{
	namespace platform
	{
		class BC_PLATFORMSHARED_DLL bcLogIDEDebug : public core::bc_ilog_listener
		{
		public:
			bcLogIDEDebug();

			~bcLogIDEDebug();

			void on_log(core::bc_log_type p_type, const bcECHAR* p_log) override;
		};
	}
}