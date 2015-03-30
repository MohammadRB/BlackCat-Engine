// [2/22/2015 MRB]

#pragma once

#include "Core/CorePCH.h"
#include "Core/bcCoreUtility.h"
#include "Core/Memory/bcMemoryManagment.h"
#include "Core/Memory/bcAlloc.h"
#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcAllocator.h"
#include "Core/Container/bcContainer.h"
#include "Core/Container/bcIterator.h"
#include "Core/Container/bcArray.h"
#include "Core/Container/bcConcurrentQueue.h"
#include "Core/Container/bcDeque.h"
#include "Core/Container/bcList.h"
#include "Core/Container/bcMap.h"
#include "Core/Container/bcString.h"
#include "Core/Container/bcTree.h"
#include "Core/Container/bcUnorderedMap.h"
#include "Core/Container/bcVector.h"
#include "Core/Concurrency/bcThreadManager.h"
#include "Core/Event/bcEvent.h"
#include "Core/Event/bcEventHandler.h"
#include "Core/Event/bcCoreEvents.h"
#include "Core/Event/bcEventManager.h"
#include "Core/Utility/bcDelegate.h"
#include "Core/Utility/bcServiceManager.h"

#if defined BC_PLATFORM_DLL
#define BC_PLATFORM_DLL_EXP __declspec(dllexport)
#else
#define BC_PLATFORM_DLL_EXP __declspec(dllimport)
#endif