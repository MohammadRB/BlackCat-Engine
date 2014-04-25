#include "CorePCH.h"
#include "bcString.h"

namespace BlackCat
{
	namespace Core
	{
		template 
		class BC_COREDLL_EXP bcBaseString< bcCHAR, bcCharTrait/*< P(bcCHAR) >*/, bcContainerAllocation<> >;
		template 
		class BC_COREDLL_EXP bcBaseString< bcWCHAR, bcWCharTrait/*< P(bcCHAR) >*/, bcContainerAllocation<> >;
	}
}