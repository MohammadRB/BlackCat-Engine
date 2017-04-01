// [04/04/2016 MRB]

#pragma once

#include "Core/File/bcContent.h"
#include "Core/File/bcContentLoader.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Resource/Texture/bcTextureConfig.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	class BC_BLACKCAT_DLL bc_texture_loader : public core::bc_base_content_loader
	{
	public:
		explicit bc_texture_loader();

		bc_texture_loader(bc_texture_loader&&) noexcept;

		~bc_texture_loader();

		bc_texture_loader& operator=(bc_texture_loader&&) noexcept;

		void content_offline_processing(core::bc_content_loader_context& p_context) const override;

		void content_processing(core::bc_content_loader_context& p_context) const override;

		static graphic::bc_texture_config s_default_config;

	protected:

	private:
	};
}