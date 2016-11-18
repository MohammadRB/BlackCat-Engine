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
	inline constexpr bcCHAR* graphic::bc_device_resource_content< graphic::bc_texture2d >::content_name()
	{
		return BC_CONTENT_NAME(texture_shader);
	}

	template< >
	inline constexpr bcUINT32 graphic::bc_device_resource_content< graphic::bc_texture2d >::content_hash()
	{
		return bc_compile_time_string_hash(BC_CONTENT_NAME(texture_shader));
	}

	class BC_BLACKCAT_DLL_EXP bc_texture_loader : public core::bc_base_content_loader
	{
	public:
		explicit bc_texture_loader();

		bc_texture_loader(bc_texture_loader&&) noexcept;

		~bc_texture_loader();

		bc_texture_loader& operator=(bc_texture_loader&&) noexcept;

		void content_offline_processing(core::bc_content_loader_context& p_context) override;

		void content_processing(core::bc_content_loader_context& p_context) override;

	protected:

	private:
	};
}