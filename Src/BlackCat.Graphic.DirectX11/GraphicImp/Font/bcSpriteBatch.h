// [02/08/2022 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Graphic/Font/bcSpriteBatch.h"
#include "3rdParty/DirectXTK-master/Include/SpriteFont.h"

namespace black_cat
{
	namespace graphic
	{
		template<>
		struct bc_platform_sprite_batch_pack<g_api_dx11>
		{
			core::bc_unique_ptr<DirectX::SpriteBatch> m_sprite_batch;
		};
	}
}
