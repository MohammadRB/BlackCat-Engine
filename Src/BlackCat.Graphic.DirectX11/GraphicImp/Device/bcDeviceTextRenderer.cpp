// [11/08/2020 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Device/bcDeviceTextRenderer.h"

namespace black_cat
{
	namespace graphic
	{
		template< >
		BC_GRAPHICIMP_DLL
		bc_platform_device_text_renderer<g_api_dx11>::bc_platform_device_text_renderer(platform_pack p_parameter)
			: m_pack(std::move(p_parameter))
		{
		}

		template< >
		BC_GRAPHICIMP_DLL
		bc_platform_device_text_renderer<g_api_dx11>::bc_platform_device_text_renderer(bc_platform_device_text_renderer&& p_other) noexcept
		{
			m_pack.m_sprite_font = std::move(p_other.m_pack.m_sprite_font);
			m_pack.m_sprite_batch = std::move(p_other.m_pack.m_sprite_batch);
		}

		template< >
		BC_GRAPHICIMP_DLL
		bc_platform_device_text_renderer<g_api_dx11>::~bc_platform_device_text_renderer()
		{	
		}

		template< >
		BC_GRAPHICIMP_DLL
		bc_platform_device_text_renderer<g_api_dx11>& bc_platform_device_text_renderer<g_api_dx11>::operator=(bc_platform_device_text_renderer&& p_other) noexcept
		{
			m_pack.m_sprite_font = std::move(p_other.m_pack.m_sprite_font);
			m_pack.m_sprite_batch = std::move(p_other.m_pack.m_sprite_batch);
			return *this;
		}

		template< >
		BC_GRAPHICIMP_DLL
		core::bc_vector2f bc_platform_device_text_renderer<g_api_dx11>::measure_text(const bcWCHAR* p_text)
		{
			DirectX::XMFLOAT2 l_origin{ 0,0 };
			DirectX::XMStoreFloat2(&l_origin, m_pack.m_sprite_font->MeasureString(p_text));

			return { l_origin.x, l_origin.y };
		}

		template< >
		BC_GRAPHICIMP_DLL
		void bc_platform_device_text_renderer<g_api_dx11>::draw_texts(bc_device& p_device, bc_render_target_view& p_back_buffer_view, const bc_device_text* p_texts, bcSIZE p_count)
		{
			const auto l_back_buffer_width = p_device.get_back_buffer_width();
			const auto l_back_buffer_height = p_device.get_back_buffer_height();

			D3D11_VIEWPORT l_viewport;
			l_viewport.TopLeftX = 0;
			l_viewport.TopLeftY = 0;
			l_viewport.Width = l_back_buffer_width;
			l_viewport.Height = l_back_buffer_height;
			l_viewport.MinDepth = 0;
			l_viewport.MaxDepth = 1;

			auto& l_device_pack = p_device.get_platform_pack();
			
			{
				// Device context is in use in SpriteBatch
				core_platform::bc_mutex_guard l_lock(l_device_pack.m_immediate_context_mutex);

				l_device_pack.m_immediate_context->OMSetRenderTargets(1, &p_back_buffer_view.get_platform_pack().m_render_target_view, nullptr);
				l_device_pack.m_immediate_context->RSSetViewports(1, &l_viewport);
								
				m_pack.m_sprite_batch->Begin();

				for (bcSIZE l_i = 0; l_i < p_count; ++l_i)
				{
					auto* l_text = &p_texts[l_i];
					auto l_position = DirectX::XMFLOAT2(l_text->m_position.x, l_text->m_position.y);
					auto l_color = DirectX::XMFLOAT4(l_text->m_color.x, l_text->m_color.y, l_text->m_color.z, 1);
					auto l_origin = DirectX::XMFLOAT2{ 0,0 };

					m_pack.m_sprite_font->DrawString
					(
						m_pack.m_sprite_batch.get(),
						l_text->m_text.c_str(),
						l_position,
						DirectX::XMLoadFloat4(&l_color),
						0.f,
						l_origin,
						l_text->m_scale
					);
				}

				m_pack.m_sprite_batch->End();

				l_device_pack.m_immediate_context->OMSetRenderTargets(0, nullptr, nullptr);
				l_device_pack.m_immediate_context->RSSetViewports(0, nullptr);
			}
		}
	}
}