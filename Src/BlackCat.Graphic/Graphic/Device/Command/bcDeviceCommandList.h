// [22/01/2016 MRB]

#pragma once

#include "Graphic/bcRenderApi.h"
#include "Graphic/bcDeviceReference.h"
#include "Graphic/bcDeviceRef.h"

namespace black_cat
{
	namespace graphic
	{
		template<bc_render_api TRenderApi>
		struct bc_platform_device_command_list_pack
		{
		};

		template<bc_render_api TRenderApi>
		class bc_platform_device_command_list : public bc_platform_device_reference<TRenderApi>
		{
		public:
			using platform_pack = bc_platform_device_command_list_pack<TRenderApi>;

		public:
			bc_platform_device_command_list() noexcept;

			explicit bc_platform_device_command_list(platform_pack& p_pack) noexcept;

			bc_platform_device_command_list(const bc_platform_device_command_list&) noexcept;

			~bc_platform_device_command_list() override;

			bc_platform_device_command_list& operator=(const bc_platform_device_command_list&) noexcept;

			void finished();

			bool is_valid() const noexcept override;

			void set_debug_name(const bcCHAR* p_name) noexcept override;
			
			bool operator==(const bc_platform_device_command_list& p_other) const noexcept;

			bool operator!=(const bc_platform_device_command_list& p_other) const noexcept;

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

		private:
			platform_pack m_pack;
		};

		using bc_device_command_list = bc_platform_device_command_list<g_current_render_api>;
		using bc_device_command_list_ref = bc_device_ref<bc_device_command_list>;
	}
}