// [03/20/2015 MRB]

#include "Core/CorePCH.h"
#include "Core/Utility/bcServiceManager.h"

namespace black_cat
{
	namespace core
	{
		template BC_COREDLL_EXP class bc_singleton< bc_service_manager() >;
	}
}