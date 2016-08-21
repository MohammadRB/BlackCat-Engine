// [04/04/2016 MRB]

#pragma once

#include "Core/File/bcContent.h"
#include "Core/File/bcContentLoader.h"
#include "Graphic/GraphicPCH.h"
#include "Graphic/bcExport.h"
#include "Graphic/Device/bcDevice.h"
#include "Graphic/Resource/Texture/bcTextureConfig.h"
#include "Graphic/Resource/Texture/bcTexture2d.h"
#include "Graphic/Loader/bcDeviceResourceContent.h"

namespace black_cat
{
	namespace graphic
	{
		template< >
		inline const bcCHAR* bc_device_resource_content<bc_texture2d>::content_name()
		{
			return core::g_cnt_texture2d;
		}

		/*struct bc_texture_loader_parameter
		{
			bc_texture_loader_parameter(bc_device& p_device, const bc_texture_config& p_config, bc_image_format p_format)
				: m_device(p_device),
				m_config(p_config),
				m_format(p_format)
			{
			}

			bc_device& m_device;
			bc_texture_config m_config;
			bc_image_format m_format;
		};*/

		class BC_GRAPHICSHARED_DLL bc_texture_loader : public core::bc_base_content_loader
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
}