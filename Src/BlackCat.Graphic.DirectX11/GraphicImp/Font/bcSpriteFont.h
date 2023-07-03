// [08/02/2022 MRB]

#pragma once

#include "GraphicImp/Font/bcSpriteBatch.h"

#include "Core/Memory/bcPtr.h"
#include "Graphic/Font/bcSpriteFont.h"
#include "3rdParty/DirectXTK/Include/SpriteFont.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		struct bc_platform_sprite_font_pack<g_api_dx11>
		{
			core::bc_unique_ptr<DirectX::SpriteFont> m_sprite_font;
		};
	}
}
