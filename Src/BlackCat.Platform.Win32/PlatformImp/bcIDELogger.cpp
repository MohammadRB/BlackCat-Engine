// [23/03/2016 MRB]

#include "PlatformImp/PlatformImpPCH.h"
#include "PlatformImp/bcExport.h"
#include "PlatformImp/bcIDELogger.h"

namespace black_cat::platform
{
	bc_ide_logger::bc_ide_logger()
	{
	}

	bc_ide_logger::~bc_ide_logger()
	{
	}

	void bc_ide_logger::on_log(core::bc_log_type p_type, const bcECHAR* p_log)
	{
		if(p_type == core::bc_log_type::info)
		{
			OutputDebugString(bcL("BlackCat Info: "));
			OutputDebugString(p_log);
			OutputDebugString(bcL("\n"));
		}
		else if(p_type == core::bc_log_type::debug)
		{
			OutputDebugString(bcL("BlackCat Debug: "));
			OutputDebugString(p_log);
			OutputDebugString(bcL("\n"));
		}
		else if(p_type == core::bc_log_type::error)
		{
			OutputDebugString(bcL("BlackCat Error: "));
			OutputDebugString(p_log);
			OutputDebugString(bcL("\n"));
		}
	}
}
