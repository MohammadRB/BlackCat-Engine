// [11/08/2020 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Graphic/Device/bcDeviceTextRenderer.h"
#include "3rdParty/DirectXTK-master/Include/SpriteFont.h"

namespace black_cat
{
	namespace graphic
	{
		template< >
		struct bc_platform_device_text_renderer_pack<g_api_dx11>
		{
			core::bc_unique_ptr<DirectX::SpriteFont> m_sprite_font;
			core::bc_unique_ptr<DirectX::SpriteBatch> m_sprite_batch;
		};
	}
}