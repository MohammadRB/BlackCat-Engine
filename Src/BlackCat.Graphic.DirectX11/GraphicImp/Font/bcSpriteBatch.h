// [08/02/2022 MRB]

#pragma once

#include "Core/Memory/bcPtr.h"
#include "Core/Container/bcVector.h"
#include "Graphic/Font/bcSpriteBatch.h"
#include "3rdParty/DirectXTK/Include/SpriteFont.h"

namespace black_cat::graphic
{
	template<>
	struct bc_platform_sprite_batch_pack<g_api_dx11>
	{
		core::bc_unique_ptr<DirectX::SpriteBatch> m_sprite_batch;
		core::bc_vector<Microsoft::WRL::ComPtr<ID3D11BlendState>> m_blend_states;
	};
}
