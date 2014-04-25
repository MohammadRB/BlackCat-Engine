//  [8/20/2013 MRB]

#pragma once

#include "CorePCH.h"

namespace BlackCat
{
	namespace Core
	{
		namespace CoreOS
		{
#define BC_Thread_Local __declspec(thread)

			bcInline bcBOOL isPointerOnStack(const void* pVoid)
			{
				void* dwStackTop = 0;
				void* dwStackLowCurrent = 0;
				asm
				{
					mov EAX, FS:[4]
					mov dwStackTop, eax
					mov EAX, FS : [8]
					mov dwStackLowCurrent, eax
				}
				if (pVoid <= dwStackTop && pVoid >= dwStackLowCurrent)
					return true;

				return false;
			}
		}
	}
}
