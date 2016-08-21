// [03/23/2016 MRB]

#include "PlatformImp/PlatformImpPCH.h"
#include "PlatformImp/bcExport.h"
#include "PlatformImp/bcLogIDEDebug.h"

namespace black_cat
{
	namespace platform
	{
		bcLogIDEDebug::bcLogIDEDebug()
		{
		}

		bcLogIDEDebug::~bcLogIDEDebug()
		{
		}

		void bcLogIDEDebug::on_log(core::bc_log_type p_type, const bcECHAR* p_log)
		{
			if(p_type == core::bc_log_type::debug)
			{
				OutputDebugString(bcL("BlackCat Debug:"));
				OutputDebugString(p_log);
				OutputDebugString(bcL("\n"));
			}
		}
	}
}