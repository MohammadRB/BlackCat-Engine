// [01/11/2016 MRB]

#pragma once

#include "Core/bcConstant.h"
#include "Graphic/GraphicPCH.h"
#include "Graphic/bcRenderApi.h"
#include "Graphic/bcDeviceReference.h"
#include "Graphic/bcDeviceRef.h"
#include "Graphic/bcDeviceResourceContent.h"
#include "Graphic/Shader/bcShader.h"

namespace black_cat
{
	namespace graphic
	{
		template< bc_render_api TRenderApi >
		struct bc_platform_domain_shader_pack
		{
		};

		template< bc_render_api TRenderApi >
		class bc_platform_domain_shader : public bc_platform_ishader<TRenderApi>
		{
		public:
			using platform_pack = bc_platform_domain_shader_pack<TRenderApi>;

		public:
			bc_platform_domain_shader();

			explicit bc_platform_domain_shader(platform_pack& p_pack);

			bc_platform_domain_shader(const bc_platform_domain_shader& p_other);

			~bc_platform_domain_shader();

			bc_platform_domain_shader& operator=(const bc_platform_domain_shader& p_other);

			bc_shader_type get_type() const override;

			bool is_valid() const noexcept override;

			void set_debug_name(const bcCHAR* p_name) noexcept override;
			
			bool operator==(const bc_platform_domain_shader& p_other) const noexcept;

			bool operator!=(const bc_platform_domain_shader& p_other) const noexcept;

			bool operator==(std::nullptr_t) const noexcept;

			bool operator!=(std::nullptr_t) const noexcept;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;
		};

		constexpr bcCHAR g_domain_shader_content_name[] = BC_CONTENT_NAME(dmn_shd);
		using bc_domain_shader = bc_platform_domain_shader< g_current_render_api >;
		using bc_domain_shader_ref = bc_device_ref< bc_domain_shader >;
		using bc_domain_shader_content = bc_device_resource_content< bc_domain_shader, g_domain_shader_content_name >;
		using bc_domain_shader_content_ptr = core::bc_content_ptr<bc_domain_shader_content>;
	}
}