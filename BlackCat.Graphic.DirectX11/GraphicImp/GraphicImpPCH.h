// [03/30/2015 MRB]

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

// == DirectX Definitions ============================================================================ /

// DirectX Includes

#include <wrl.h>

#ifdef BC_DEBUG
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif
#endif

#include <dxgi.h>
#include <d3d11.h>
//#include <d3dx11.h>
//#include <dxerr.h>
//#include <d3dx10.h>
#include <D3Dcompiler.h>
//#include <d3d9.h>
//#include <D3d9types.h>
//#include <d3dx9.h>

//// DirectX Math
//#include <DirectXMath.h>

// Static Lib Imports
//#pragma comment(lib, "dxerr.lib") // Not compatible with VC2015
#pragma comment(lib, "dxguid.lib")
//#pragma comment(lib, "D3D9.lib")
#pragma comment(lib, "D3D11.lib")
//#pragma comment(lib, "D3DX10.lib")
//#pragma comment(lib, "D3DX11.lib")
#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "gdiplus.lib")

#ifdef BC_DEBUG
//#pragma comment(lib, "D3D11SDKLAYERS.lib")
//#pragma comment(lib, "comsuppwd.lib")
//#pragma comment(lib, "d3dx9d.lib")
#else
//#pragma comment(lib, "comsuppw.lib")
//#pragma comment(lib, "d3dx9.lib")
#endif