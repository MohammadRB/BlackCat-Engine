// [04/04/2016 MRB]

#pragma once

#include "Core/Content/bcContentLoader.h"
#include "GraphicImp/Resource/Texture/bcTextureConfig.h"
#include "App/bcExport.h"

namespace black_cat
{
	class BC_DLL bc_texture_loader : public core::bc_base_content_loader
	{
	public:
		explicit bc_texture_loader();

		bc_texture_loader(bc_texture_loader&&) noexcept;

		~bc_texture_loader() override;

		bc_texture_loader& operator=(bc_texture_loader&&) noexcept;

		bool support_offline_processing() const override;

		void content_processing(core::bc_content_loading_context& p_context) const override;

		void content_processing(core::bc_content_saving_context& p_context) const override;

		static graphic::bc_texture_config s_default_config;
	};
}