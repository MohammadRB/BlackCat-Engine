// [2/22/2015 MRB]

#pragma once

#include "CorePlatformImp/CorePlatformImpPCH.h"
#include "Platform/PlatformPCH.h"
#include "Platform/bcPlatformEvents.h"
#include "Platform/bcPlatformException.h"

#if defined BC_PLATFORMIMP_DLL
#define BC_PLATFORMIMP_DLL_EXP __declspec(dllexport)
#else
#define BC_PLATFORMIMP_DLL_EXP __declspec(dllimport)
#endif