// [12/22/2016 MRB]

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

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}
			
		protected:
			bc_platform_device_reference();

			explicit bc_platform_device_reference(platform_pack& p_pack);

			bc_platform_device_reference(const bc_platform_device_reference&);

			bc_platform_device_reference& operator=(const bc_platform_device_reference&);

		private:
			platform_pack m_pack;
		};

		using bc_device_reference = bc_platform_device_reference<g_current_render_api>;
	}
}