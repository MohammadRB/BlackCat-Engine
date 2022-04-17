// [06/18/2021 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Device/bcDeviceSwapBuffer.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/bcExport.h"

using namespace Microsoft::WRL;

namespace black_cat
{
	namespace graphic
	{
		void _check_best_dxgi_fullscreen_setting(ID3D11Device* p_device, IDXGISwapChain* p_swap_chain, DXGI_MODE_DESC* p_new_desc, DXGI_MODE_DESC* p_best_desc)
		{
			ComPtr<IDXGIDevice> l_dxgi_device;
			ComPtr<IDXGIAdapter> l_adapter;
			ComPtr<IDXGIOutput> l_output;

			dx_call(p_swap_chain->GetDevice(__uuidof(IDXGIDevice), reinterpret_cast<void**>(l_dxgi_device.GetAddressOf())));
			l_dxgi_device->GetAdapter(l_adapter.GetAddressOf());
			l_adapter->EnumOutputs(0, l_output.GetAddressOf());

			dx_call(l_output->FindClosestMatchingMode(p_new_desc, p_best_desc, p_device));
		}
		
		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_swap_buffer<g_api_dx11>::bc_platform_device_swap_buffer()
			: m_pack()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_swap_buffer<g_api_dx11>::bc_platform_device_swap_buffer(platform_pack& p_pack)
			: m_pack(p_pack)
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_swap_buffer<g_api_dx11>::bc_platform_device_swap_buffer(const bc_platform_device_swap_buffer& p_other) noexcept
			: m_pack(p_other.m_pack)
		{
			
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_swap_buffer<g_api_dx11>::~bc_platform_device_swap_buffer() = default;

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device_swap_buffer<g_api_dx11>& bc_platform_device_swap_buffer<g_api_dx11>::operator=(const bc_platform_device_swap_buffer& p_other) noexcept
		{
			m_pack = p_other.m_pack;
			return *this;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_device_swap_buffer<g_api_dx11>::get_vsync() const
		{
			return m_pack.m_vsync;
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_swap_buffer<g_api_dx11>::set_vsync(bool p_sync)
		{
			m_pack.m_vsync = p_sync;
		}
		
		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_device_swap_buffer<g_api_dx11>::get_full_screen() const
		{
			DXGI_SWAP_CHAIN_DESC l_desc;
			m_pack.m_swap_chain->GetDesc(&l_desc);

			return !l_desc.Windowed;
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_swap_buffer<g_api_dx11>::set_full_screen(bc_device& p_device, bool p_full_screen)
		{
			if (p_full_screen)
			{
				DXGI_SWAP_CHAIN_DESC l_swap_chain_desc;
				DXGI_MODE_DESC l_best_mode_desc;

				m_pack.m_swap_chain->GetDesc(&l_swap_chain_desc);

				_check_best_dxgi_fullscreen_setting
				(
					p_device.get_platform_pack().m_device.Get(),
					m_pack.m_swap_chain,
					&l_swap_chain_desc.BufferDesc,
					&l_best_mode_desc
				);
				
				// Will send a WM_SIZE event so back buffer and all other buffers will be resizing by inter app events
				dx_call(m_pack.m_swap_chain->ResizeTarget(&l_best_mode_desc));
			}

			dx_call(m_pack.m_swap_chain->SetFullscreenState(p_full_screen, nullptr));
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_texture2d bc_platform_device_swap_buffer<g_api_dx11>::get_back_buffer_texture() const
		{
			ComPtr<ID3D11Texture2D> l_surface;
			dx_call(m_pack.m_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(l_surface.GetAddressOf())));

			bc_texture2d::platform_pack l_pack;
			l_pack.m_resource = l_surface.Get();
			l_pack.m_texture = l_surface.Get();

			return bc_texture2d(l_pack);
		}

		template<>
		BC_GRAPHICIMP_DLL
		bcUINT32 bc_platform_device_swap_buffer<g_api_dx11>::get_back_buffer_width() const
		{
			return get_back_buffer_texture().get_width();
		}

		template<>
		BC_GRAPHICIMP_DLL
		bcUINT32 bc_platform_device_swap_buffer<g_api_dx11>::get_back_buffer_height() const
		{
			return get_back_buffer_texture().get_height();
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_format bc_platform_device_swap_buffer<g_api_dx11>::get_back_buffer_format() const
		{
			return get_back_buffer_texture().get_format();
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_swap_buffer<g_api_dx11>::resize_back_buffer(bc_device& p_device, bcUINT p_width, bcUINT p_height)
		{
			resize_back_buffer(p_device, p_width, p_height, get_back_buffer_format());
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_swap_buffer<g_api_dx11>::resize_back_buffer(bc_device& p_device, bcUINT p_width, bcUINT p_height, bc_format p_format)
		{
			bc_texture2d l_back_buffer = get_back_buffer_texture();

			// If we are in fullscreen state change resolution too
			if (get_full_screen()) // TODO check for 
			{
				DXGI_SWAP_CHAIN_DESC l_new_mode_desc;
				DXGI_MODE_DESC l_best_mode_desc;

				m_pack.m_swap_chain->GetDesc(&l_new_mode_desc);
				l_new_mode_desc.BufferDesc.Width = p_width;
				l_new_mode_desc.BufferDesc.Height = p_height;
				l_new_mode_desc.BufferDesc.Format = bc_graphic_cast(p_format);

				_check_best_dxgi_fullscreen_setting
				(
					p_device.get_platform_pack().m_device.Get(),
					m_pack.m_swap_chain,
					&l_new_mode_desc.BufferDesc,
					&l_best_mode_desc
				);
				dx_call(m_pack.m_swap_chain->ResizeTarget(&l_best_mode_desc));
			}

			if (l_back_buffer.get_width() != p_width || l_back_buffer.get_height() != p_height)
			{
				dx_call(m_pack.m_swap_chain->ResizeBuffers
				(
					2,
					p_width,
					p_height,
					bc_graphic_cast(p_format),
					DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
				));
			}
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_swap_buffer<g_api_dx11>::present()
		{
			const auto l_result = m_pack.m_swap_chain->Present(m_pack.m_vsync ? 1 : 0, 0);
		}
		
		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_device_swap_buffer<g_api_dx11>::is_valid() const noexcept
		{
			return m_pack.m_swap_chain != nullptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device_swap_buffer<g_api_dx11>::set_debug_name(const bcCHAR* p_name) noexcept
		{
			if (is_valid())
			{
				m_pack.m_swap_chain->SetPrivateData(WKPDID_D3DDebugObjectName, std::strlen(p_name), p_name);
			}
		}
	}	
}