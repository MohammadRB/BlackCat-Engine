// [01/03/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcRenderApi.h"
#include "Graphic/bcDeviceRef.h"
#include "Graphic/Resource/bcResource.h"
#include "Graphic/Resource/Buffer/bcBufferConfig.h"

namespace black_cat
{
	namespace graphic
	{
		template<bc_render_api TRenderApi>
		struct bc_platform_buffer_pack
		{
		};
		
		template<bc_render_api TRenderApi>
		class bc_platform_buffer : public bci_platform_resource<TRenderApi>
		{
		public:
			using platform_pack = bc_platform_buffer_pack<TRenderApi>;

		public:
			bc_platform_buffer();

			explicit bc_platform_buffer(platform_pack& p_pack);

			bc_platform_buffer(const bc_platform_buffer& p_other);

			~bc_platform_buffer();

			bc_platform_buffer& operator=(const bc_platform_buffer& p_other);

			bcUINT get_byte_width() const;

			bc_resource_usage get_usage() const;

			bcUINT get_structure_byte_stride() const;

			bool is_valid() const noexcept override;

			void set_debug_name(const bcCHAR* p_name) noexcept override;
			
			bool operator==(const bc_platform_buffer& p_other) const noexcept;

			bool operator!=(const bc_platform_buffer& p_other) const noexcept;

			bool operator==(std::nullptr_t) const noexcept;

			bool operator!=(std::nullptr_t) const noexcept;

			bc_resource_type get_type() const override
			{
				return bc_resource_type::buffer;
			}

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;
		};

		using bc_buffer = bc_platform_buffer<g_current_render_api>;
		using bc_buffer_ref = bc_device_ref< bc_buffer >;
	}
}