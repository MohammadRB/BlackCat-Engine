// [22/12/2016 MRB]

#pragma once

#include "Graphic/bcRenderApi.h"

namespace black_cat
{
	namespace graphic
	{
		template<bc_render_api TApi>
		struct bc_platform_device_reference_pack
		{
		};

		template<bc_render_api TApi>
		class bc_platform_device_reference
		{
		public:
			using platform_pack = bc_platform_device_reference_pack<TApi>;

		public:
			virtual ~bc_platform_device_reference();

			virtual bool is_valid() const noexcept = 0;

			virtual void set_debug_name(const bcCHAR* p_name) noexcept = 0;
			
			virtual platform_pack& get_platform_pack() noexcept = 0;
			
			virtual const platform_pack& get_platform_pack() const noexcept = 0;
			
		protected:
			bc_platform_device_reference() noexcept;

			explicit bc_platform_device_reference(platform_pack& p_pack) noexcept;

			bc_platform_device_reference(const bc_platform_device_reference&) noexcept;

			bc_platform_device_reference& operator=(const bc_platform_device_reference&) noexcept;
		};

		using bc_device_reference = bc_platform_device_reference<g_current_render_api>;
	}
}