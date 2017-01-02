// [01/11/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcRenderApi.h"
#include "Graphic/bcDeviceReference.h"
#include "Graphic/bcDeviceRef.h"
#include "Graphic/Shader/bcShader.h"
#include "Graphic/bcDeviceResourceContent.h"

namespace black_cat
{
	namespace graphic
	{
		template< bc_render_api TRenderApi >
		struct bc_platform_pixel_shader_pack
		{
		};

		template< bc_render_api TRenderApi >
		class bc_platform_pixel_shader : public bc_platform_ishader<TRenderApi>
		{
		public:
			using platform_pack = bc_platform_pixel_shader_pack<TRenderApi>;

		public:
			bc_platform_pixel_shader();

			explicit bc_platform_pixel_shader(platform_pack& p_pack);

			bc_platform_pixel_shader(const bc_platform_pixel_shader& p_other);

			~bc_platform_pixel_shader();

			bc_platform_pixel_shader& operator=(const bc_platform_pixel_shader& p_other);

			bc_shader_type get_type() const override;

			bool is_valid() const noexcept override;

			bool operator==(const bc_platform_pixel_shader& p_other) const noexcept;

			bool operator!=(const bc_platform_pixel_shader& p_other) const noexcept;

			bool operator==(std::nullptr_t) const noexcept;

			bool operator!=(std::nullptr_t) const noexcept;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			platform_pack m_pack;
		};

		using bc_pixel_shader = bc_platform_pixel_shader< g_current_render_api >;
		using bc_pixel_shader_ptr = bc_device_ref< bc_pixel_shader >;
		using bc_pixel_shader_content = bc_device_resource_content< bc_pixel_shader >;
		using bc_pixel_shader_content_ptr = core::bc_content_ptr<bc_pixel_shader_content>;
	}
}