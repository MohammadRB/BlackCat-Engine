#ifndef bcEventListener_H
#define bcEventListener_H

#include "CorePCH.h"
#include "bcEvent.h"

namespace BlackCat
{
	namespace Core
	{
		class BC_COREDLL_EXP bcEventListener
		{
		public:
			bcEventListener( ) {}; 
			virtual ~bcEventListener( ) {};

			//virtual std::wstring GetName( ) = 0;
			virtual bcBOOL HandleEvent( bcEvent* pEvent ) = 0;
		};
	}
}

#endif