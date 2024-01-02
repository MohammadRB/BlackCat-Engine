// [30/03/2015 MRB]

#pragma once

#pragma warning( disable : 4996 )

#include "CorePlatform/CorePlatformPCH.h"
#include "CorePlatformImp/CorePlatformImpPCH.h"
#include "Core/CorePCH.h"
#include "Platform/PlatformPCH.h"
#include "PlatformImp/PlatformImpPCH.h"
#include "Graphic/GraphicPCH.h"
#include "Graphic/bcRenderApi.h"
#include "Graphic/bcGraphicDefinition.h"
#include "Graphic/bcException.h"
#include "GraphicImp/bcUtility.h"

#ifdef BC_DEBUG
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif

#include <wrl.h>
#include <dxgi.h>
#include <d3d11.h>
#include <D3Dcompiler.h>

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "gdiplus.lib")

#ifdef BC_DEBUG
//#pragma comment(lib, "D3D11SDKLAYERS.lib")
//#pragma comment(lib, "comsuppwd.lib")
#else
//#pragma comment(lib, "comsuppw.lib")
#endif