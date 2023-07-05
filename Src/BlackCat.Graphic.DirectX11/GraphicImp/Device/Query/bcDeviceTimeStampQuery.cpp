// [22/04/2022 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/Device/Query/bcDeviceTimeStampQuery.h"
#include "GraphicImp/bcExport.h"

namespace black_cat::graphic
{
	template<>
	BC_GRAPHICIMP_DLL
	bc_platform_device_timestamp_query<g_api_dx11>::bc_platform_device_timestamp_query()
	{
		m_pack.m_query = nullptr;
	}

	template<>
	BC_GRAPHICIMP_DLL
	bc_platform_device_timestamp_query<g_api_dx11>::bc_platform_device_timestamp_query(platform_pack& p_pack)
		: bc_platform_device_reference(p_pack),
		  m_pack(p_pack)
	{
	}

	template<>
	BC_GRAPHICIMP_DLL
	bc_platform_device_timestamp_query<g_api_dx11>::bc_platform_device_timestamp_query(const bc_platform_device_timestamp_query& p_other) noexcept
		: bc_platform_device_reference(p_other),
		  m_pack(p_other.m_pack)
	{
	}

	template<>
	BC_GRAPHICIMP_DLL
	bc_platform_device_timestamp_query<g_api_dx11>::~bc_platform_device_timestamp_query() = default;

	template<>
	BC_GRAPHICIMP_DLL
	bc_platform_device_timestamp_query<g_api_dx11>& bc_platform_device_timestamp_query<g_api_dx11>::operator=(const bc_platform_device_timestamp_query& p_other) noexcept
	{
		bc_platform_device_reference::operator=(p_other);
		m_pack = p_other.m_pack;

		return *this;
	}

	template<>
	BC_GRAPHICIMP_DLL
	bool bc_platform_device_timestamp_query<g_api_dx11>::is_valid() const noexcept
	{
		return m_pack.m_query != nullptr;
	}

	template<>
	BC_GRAPHICIMP_DLL
	void bc_platform_device_timestamp_query<g_api_dx11>::set_debug_name(const bcCHAR* p_name) noexcept
	{
		if (is_valid())
		{
			m_pack.m_query->SetPrivateData(WKPDID_D3DDebugObjectName, std::strlen(p_name), p_name);
		}
	}
}
