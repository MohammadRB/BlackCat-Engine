// [22/04/2022 MRB]

#pragma once

#include "Graphic/bcDeviceRef.h"
#include "Graphic/bcDeviceReference.h"
#include "Graphic/bcRenderApi.h"

namespace black_cat
{
	namespace graphic
	{
		template<bc_render_api TApi>
		struct bc_platform_device_timestamp_query_pack
		{
		};

		template<bc_render_api TApi>
		class bc_platform_device_timestamp_query : public bc_platform_device_reference<TApi>
		{
		public:
			using platform_pack = bc_platform_device_timestamp_query_pack<TApi>;

		public:
			bc_platform_device_timestamp_query();

			explicit bc_platform_device_timestamp_query(platform_pack& p_pack);

			bc_platform_device_timestamp_query(const bc_platform_device_timestamp_query&) noexcept;

			~bc_platform_device_timestamp_query() override;

			bc_platform_device_timestamp_query& operator=(const bc_platform_device_timestamp_query&) noexcept;

			bool is_valid() const noexcept override;

			void set_debug_name(const bcCHAR* p_name) noexcept override;

			platform_pack& get_platform_pack() noexcept override
			{
				return m_pack;
			}

			const platform_pack& get_platform_pack() const noexcept override
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;
		};

		using bc_device_timestamp_query = bc_platform_device_timestamp_query<g_current_render_api>;
		using bc_device_timestamp_query_ref = bc_device_ref<bc_device_timestamp_query>;
	}
}