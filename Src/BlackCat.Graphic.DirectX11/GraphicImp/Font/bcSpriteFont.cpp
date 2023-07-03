// [08/02/2022 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "GraphicImp/Font/bcSpriteFont.h"
#include "GraphicImp/bcExport.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_sprite_font<g_api_dx11>::bc_platform_sprite_font(platform_pack p_parameter) noexcept
			: m_pack(std::move(p_parameter))
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_sprite_font<g_api_dx11>::bc_platform_sprite_font(bc_platform_sprite_font&& p_other) noexcept
			: m_pack(std::move(p_other.m_pack))
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_sprite_font<g_api_dx11>::~bc_platform_sprite_font()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_sprite_font<g_api_dx11>& bc_platform_sprite_font<g_api_dx11>::operator=(bc_platform_sprite_font&& p_other) noexcept
		{
			m_pack = std::move(p_other.m_pack);
			return *this;
		}

		template<>
		BC_GRAPHICIMP_DLL
		core::bc_vector2f bc_platform_sprite_font<g_api_dx11>::measure_string(const bcWCHAR* p_text)
		{
			DirectX::XMFLOAT2 l_origin{ 0,0 };
			DirectX::XMStoreFloat2(&l_origin, m_pack.m_sprite_font->MeasureString(p_text));

			return { l_origin.x, l_origin.y };
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_sprite_font<g_api_dx11>::draw_string(bc_sprite_batch& p_batch,
			const bcWCHAR* p_string,
			const core::bc_vector2i& p_position,
			const core::bc_vector3f& p_color,
			bcFLOAT p_scale)
		{
			const auto l_position = DirectX::XMFLOAT2(p_position.x, p_position.y);
			const auto l_color = DirectX::XMFLOAT4(p_color.x, p_color.y, p_color.z, 1);
			constexpr auto l_origin = DirectX::XMFLOAT2{ 0,0 };

			m_pack.m_sprite_font->DrawString
			(
				p_batch.get_platform_pack().m_sprite_batch.get(),
				p_string,
				l_position,
				DirectX::XMLoadFloat4(&l_color),
				0.f,
				l_origin,
				p_scale
			);
		}
	}
}
