// [11/08/2014 MRB]

#include "CorePlatformImp/CoreWin32PCH.h"
#include "coreplatformimp/concurrency/bcconcurrencyutilityprovider.h"

namespace black_cat
{
	namespace core_platform
	{
		bcSIZE bc_thread_local_counter::m_num = 0;
	}
}