// [11/13/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcExport.h"
#include "GraphicImp/Device/bcDeviceOutput.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_output< g_api_dx11 >::bc_platform_device_output(platform_pack p_parameter) noexcept
			: m_pack(p_parameter.m_output_handle)
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_output< g_api_dx11 >::bc_platform_device_output(bc_platform_device_output&& p_other) noexcept
			: m_pack(p_other.m_pack.m_output_handle)
		{
			p_other.m_pack.m_output_handle = nullptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_output< g_api_dx11 >::~bc_platform_device_output()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_output< g_api_dx11 >& bc_platform_device_output< g_api_dx11 >::operator=(bc_platform_device_output&& p_other) noexcept
		{
			m_pack = std::move(p_other.m_pack);
			p_other.m_pack.m_output_handle = nullptr;

			return *this;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_output< g_api_dx11 > bc_platform_device_output< g_api_dx11 >::from_window(platform::bc_platform_basic_window< core_platform::g_api_win32 >& p_render_window)
		{
			bc_platform_device_output l_device_output(platform_pack(p_render_window.get_platform_pack().m_handle));

			return l_device_output;
		}
	}
}