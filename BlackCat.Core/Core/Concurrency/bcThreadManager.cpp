
#include "CorePCH.h"
#include "bcThreadManager.h"

namespace BlackCat
{
	namespace Core
	{
		void BlackCat::Core::checkForInterruption()
		{
			bcThreadManager::Get()->checkForInterruption();
		}
	}
}



