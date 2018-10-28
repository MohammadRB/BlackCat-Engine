// [03/06/2017 MRB]

#pragma once

#include "Core/Math/bcVector4f.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "Game/bcExport.h"

namespace black_cat
{
	namespace game
	{
		struct bc_render_material;
		class bc_material_manager;

		class BC_GAME_DLL _bc_material_ptr_deleter
		{
		public:
			_bc_material_ptr_deleter();

			_bc_material_ptr_deleter(bc_material_manager* p_material_manager);

			void operator()(bc_render_material* p_material) const;

			bc_material_manager* m_material_manager;
		};

		struct bc_render_material : public core::bc_ref_count
		{
		public:
			core::bc_vector4f m_diffuse;
			bcFLOAT m_specular_intensity;
			bcFLOAT m_specular_power;

			graphic::bc_texture2d_content_ptr m_diffuse_map;
			graphic::bc_texture2d_content_ptr m_specular_map;
			graphic::bc_texture2d_content_ptr m_normal_map;
		};

		using bc_material_ptr = core::bc_ref_count_ptr< bc_render_material, _bc_material_ptr_deleter >;
	}
}