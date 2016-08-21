// [04/04/2016 MRB]

#pragma once

#include "Core/File/bcContentLoader.h"
#include "Graphic/GraphicPCH.h"
#include "Graphic/bcExport.h"
#include "Graphic/Device/bcDevice.h"
#include "Graphic/Shader/bcpixelShader.h"
#include "Graphic/Loader/bcDeviceResourceContent.h"

namespace black_cat
{
	namespace graphic
	{
		template< >
		inline const bcCHAR* bc_device_resource_content<bc_pixel_shader>::content_name()
		{
			return core::g_cnt_pixel_shader;
		}

		/*struct bc_pixel_shader_loader_parameter
		{
			bc_pixel_shader_loader_parameter(bc_device& p_device, const bcCHAR* p_function)
				: m_device(p_device),
				m_function(p_function)
			{
			}

			bc_device& m_device;
			const bcCHAR* m_function;
		};*/

		class BC_GRAPHICSHARED_DLL bc_pixel_shader_loader : public core::bc_base_content_loader
		{
		public:
			explicit bc_pixel_shader_loader();

			bc_pixel_shader_loader(bc_pixel_shader_loader&&);

			~bc_pixel_shader_loader();

			bc_pixel_shader_loader& operator=(bc_pixel_shader_loader&&);

			void content_offline_processing(core::bc_content_loader_context& p_context) override;

			void content_processing(core::bc_content_loader_context& p_context) override;

		protected:

		private:
		};
	}
}