//  [9/16/2013 MRB]

#pragma once

#include "CorePCH.h"

namespace BlackCat
{
	namespace Core
	{
		enum bcLockOperation
		{
			bcLockOperation_Light = 100,
			bcLockOperation_Medium = 50,
			bcLockOperation_Heavy = 1
		};

		enum bcMutexType
		{
			bcMutex_Plain = 0x01,
			bcMutex_Try	= 0x02,
			bcMutex_Timed	= 0x04,
			bcMutex_Recursive = 0x100
		};
		enum bcLockProperty
		{
			bcLock_Adapt,
			bcLock_Defer,
			bcLock_Try
		};
		
		enum bcCV_Status
		{
			bcCV_Timeout,
			bcCV_NoTimeout
		};

		enum bcFutureStatus
		{
			bcFuture_Ready,
			bcFuture_Timeout,
			bcFuture_Deferred
		};

		enum bcMemoryOrder
		{
			bcMemoryOrder_Relaxed,
			bcMemoryOrder_Consume,
			bcMemoryOrder_Acquire,
			bcMemoryOrder_Release,
			bcMemoryOrder_AcqRel,
			bcMemoryOrder_SeqCst
		};

		enum bcConcurrencyLibrary
		{
			bcConcurrencyLibrary_None,
			bcConcurrencyLibrary_STD
		};
	}
}