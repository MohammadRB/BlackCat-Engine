// [06/01/2016 MRB]

#pragma once

#include "Core/bcConstant.h"
#include "Graphic/GraphicPCH.h"
#include "Graphic/bcRenderApi.h"
#include "Graphic/bcDeviceReference.h"
#include "Graphic/bcDeviceRef.h"
#include "Graphic/bcDeviceResourceContent.h"
#include "Graphic/Resource/bcResource.h"

namespace black_cat
{
	namespace graphic
	{
		template<bc_render_api TRenderApi>
		struct bc_platform_texture2d_pack
		{
		};

		template<bc_render_api TRenderApi>
		class bc_platform_texture2d : public bci_platform_resource<TRenderApi>
		{
		public:
			using platform_pack = bc_platform_texture2d_pack<TRenderApi>;

		public:
			bc_platform_texture2d() noexcept;

			explicit  bc_platform_texture2d(platform_pack& p_pack) noexcept;

			bc_platform_texture2d(const bc_platform_texture2d& p_other) noexcept;

			~bc_platform_texture2d() noexcept override;
			
			bc_platform_texture2d& operator=(const bc_platform_texture2d& p_other) noexcept;

			bcUINT32 get_width() const noexcept;

			bcUINT32 get_height() const noexcept;

			bc_resource_usage get_usage() const noexcept;

			bool is_multisampled() const noexcept;

			bc_texture_ms_config get_sample_count() const noexcept;

			bc_format get_format() const noexcept;

			bc_resource_type get_type() const noexcept override
			{
				return bc_resource_type::texture_2d;
			}

			bool is_valid() const noexcept override;

			void set_debug_name(const bcCHAR* p_name) noexcept override;
			
			bool operator==(const bc_platform_texture2d& p_other) const noexcept;

			bool operator!=(const bc_platform_texture2d& p_other) const noexcept;

			bool operator==(std::nullptr_t) const noexcept;

			bool operator!=(std::nullptr_t) const noexcept;

			platform_pack& get_platform_pack() noexcept override
			{
				return m_pack;
			}

			const platform_pack& get_platform_pack() const noexcept override
			{
				return m_pack;
			}

			platform_pack& get_resource_platform_pack() noexcept override
			{
				return get_platform_pack();
			}

			const platform_pack& get_resource_platform_pack() const noexcept override
			{
				return get_platform_pack();
			}
		
		private:
			platform_pack m_pack;
		};

		constexpr bcCHAR g_texture_content_name[] = BC_CONTENT_NAME(tex_2d);
		using bc_texture2d = bc_platform_texture2d<g_current_render_api>;
		using bc_texture2d_ref = bc_device_ref<bc_texture2d>;
		using bc_texture2d_content = bc_device_resource_content<bc_texture2d, g_texture_content_name>;
		using bc_texture2d_content_ptr = core::bc_content_ptr<bc_texture2d_content>;
	}
}