// [02/08/2022 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Device/bcDeviceSwapBuffer.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "GraphicImp/Font/bcSpriteBatch.h"
#include "GraphicImp/bcExport.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_sprite_batch<g_api_dx11>::bc_platform_sprite_batch(platform_pack p_parameter) noexcept
			: m_pack(std::move(p_parameter))
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_sprite_batch<g_api_dx11>::bc_platform_sprite_batch(bc_platform_sprite_batch&& p_other) noexcept
			: m_pack(std::move(p_other.m_pack))
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_sprite_batch<g_api_dx11>::~bc_platform_sprite_batch()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_sprite_batch<g_api_dx11>& bc_platform_sprite_batch<g_api_dx11>::operator=(bc_platform_sprite_batch&& p_other) noexcept
		{
			m_pack = std::move(p_other.m_pack);
			return *this;
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_sprite_batch<g_api_dx11>::begin(bc_device& p_device, const bc_device_swap_buffer& p_swap_buffer, bc_render_target_view& p_render_target) noexcept
		{
			const auto l_back_buffer_width = p_swap_buffer.get_back_buffer_width();
			const auto l_back_buffer_height = p_swap_buffer.get_back_buffer_height();

			D3D11_VIEWPORT l_viewport;
			l_viewport.TopLeftX = 0;
			l_viewport.TopLeftY = 0;
			l_viewport.Width = l_back_buffer_width;
			l_viewport.Height = l_back_buffer_height;
			l_viewport.MinDepth = 0;
			l_viewport.MaxDepth = 1;

			auto& l_device_pack = p_device.get_platform_pack();

			l_device_pack.m_immediate_context_mutex.lock(); // Device context is in use in SpriteBatch
			l_device_pack.m_immediate_context->OMSetRenderTargets(1, &p_render_target.get_platform_pack().m_render_target_view, nullptr);
			l_device_pack.m_immediate_context->RSSetViewports(1, &l_viewport);

			m_pack.m_sprite_batch->Begin();
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_sprite_batch<g_api_dx11>::end(bc_device& p_device) noexcept
		{
			auto& l_device_pack = p_device.get_platform_pack();

			m_pack.m_sprite_batch->End();

			l_device_pack.m_immediate_context->OMSetRenderTargets(0, nullptr, nullptr);
			l_device_pack.m_immediate_context->RSSetViewports(0, nullptr);
			l_device_pack.m_immediate_context_mutex.unlock();
		}
	}
}
