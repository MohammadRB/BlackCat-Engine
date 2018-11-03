// [09/09/2016 MRB]

#pragma once

#include "GraphicImp/bcDeviceReference.h"
#include "GraphicImp/bcDeviceRef.h"
#include "GraphicImp/Shader/bcComputeShader.h"

#include "Graphic/Device/bcDeviceComputeState.h"
#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/bcRefCountProxy.h"

namespace black_cat
{
	namespace graphic
	{
		class _bc_device_compute_state
		{
		public:
			bc_device_compute_state_config m_config;
		};

		using bc_device_compute_state_proxy = bc_refcount_proxy<_bc_device_compute_state>;

		template<>
		struct bc_platform_device_compute_state_pack< g_api_dx11 >
		{
			bc_device_compute_state_proxy* m_compute_state_proxy;
		};

		template<>
		inline bc_platform_device_compute_state< g_api_dx11 >::bc_platform_device_compute_state()
			: m_pack()
		{
			m_pack.m_compute_state_proxy = nullptr;
		}

		template<>
		inline bc_platform_device_compute_state<g_api_dx11>::bc_platform_device_compute_state(platform_pack& p_pack)
			: m_pack()
		{
			m_pack.m_compute_state_proxy = p_pack.m_compute_state_proxy;
		}

		template<>
		inline bc_platform_device_compute_state< g_api_dx11 >::bc_platform_device_compute_state(const bc_platform_device_compute_state& p_other) noexcept
			: bc_platform_device_reference(p_other),
			m_pack()
		{
			m_pack.m_compute_state_proxy = p_other.m_pack.m_compute_state_proxy;
		}

		template<>
		inline bc_platform_device_compute_state< g_api_dx11 >::~bc_platform_device_compute_state()
		{
		}

		template<>
		inline bc_platform_device_compute_state< g_api_dx11 >& bc_platform_device_compute_state< g_api_dx11 >::operator=(const bc_platform_device_compute_state& p_other) noexcept
		{
			bc_platform_device_reference::operator=(p_other);
			m_pack.m_compute_state_proxy = p_other.m_pack.m_compute_state_proxy;

			return *this;
		}

		template<>
		inline const bc_device_compute_state_config& bc_platform_device_compute_state< g_api_dx11 >::get_config() const
		{
			return m_pack.m_compute_state_proxy->m_config;
		}

		template<>
		inline bool bc_platform_device_compute_state< g_api_dx11 >::is_valid() const noexcept
		{
			return m_pack.m_compute_state_proxy != nullptr;
		}

		template<>
		inline bool bc_platform_device_compute_state<g_api_dx11>::operator==(const bc_platform_device_compute_state& p_other) const noexcept
		{
			return m_pack.m_compute_state_proxy == p_other.m_pack.m_compute_state_proxy;
		}

		template<>
		inline bool bc_platform_device_compute_state<g_api_dx11>::operator!=(const bc_platform_device_compute_state& p_other) const noexcept
		{
			return !operator==(p_other);
		}

		template<>
		inline bool bc_platform_device_compute_state<g_api_dx11>::operator==(std::nullptr_t) const noexcept
		{
			return !is_valid();
		}

		template<>
		inline bool bc_platform_device_compute_state<g_api_dx11>::operator!=(std::nullptr_t) const noexcept
		{
			return is_valid();
		}
	}
}