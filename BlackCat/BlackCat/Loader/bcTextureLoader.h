// [04/04/2016 MRB]

#pragma once

#include "Core/File/bcContent.h"
#include "Core/File/bcContentLoader.h"
#include "Graphic/bcDeviceResourceContent.h"
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/Resource/Texture/bcTextureConfig.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "BlackCat/bcExport.h"

namespace black_cat
{
	template< >
	inline const bcCHAR* graphic::bc_device_resource_content< graphic::bc_texture2d >::content_name()
	{
		return core::g_cnt_texture2d;
	}

	class BC_BLACKCAT_DLL_EXP bc_texture_loader : public core::bc_base_content_loader
	{
	public:
		explicit bc_texture_loader();

		bc_texture_loader(bc_texture_loader&&);

		~bc_texture_loader();

		bc_texture_loader& operator=(bc_texture_loader&&);

		void content_offline_processing(core::bc_content_loader_context& p_context) override;

		void content_processing(core::bc_content_loader_context& p_context) override;

	protected:

	private:
	};
}