// [01/22/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcRenderApi.h"
#include "Graphic/bcDeviceReference.h"
#include "Graphic/bcDeviceRef.h"
#include "Graphic/Device/bcDevice.h"
#include "Graphic/Device/Command/bcDeviceCommandList.h"

namespace black_cat
{
	namespace graphic
	{
		template<bc_render_api TRenderApi>
		struct bc_platform_device_command_executor_pack
		{
		};

		template<bc_render_api TRenderApi>
		class bc_platform_device_command_executor : public bc_platform_device_reference<TRenderApi>
		{
		public:
			using platform_pack = bc_platform_device_command_executor_pack<TRenderApi>;

		public:
			bc_platform_device_command_executor() noexcept;

			bc_platform_device_command_executor(platform_pack& p_pack) noexcept;

			bc_platform_device_command_executor(const bc_platform_device_command_executor&) noexcept;

			~bc_platform_device_command_executor() override;

			bc_platform_device_command_executor& operator=(const bc_platform_device_command_executor&) noexcept;

			void execute_command_list(bc_device_command_list p_command_list);

			bool is_valid() const noexcept override;

			void set_debug_name(const bcCHAR* p_name) noexcept override;
			
			bool operator==(const bc_platform_device_command_executor& p_other) const noexcept;

			bool operator!=(const bc_platform_device_command_executor& p_other) const noexcept;

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

		using bc_device_command_executor = bc_platform_device_command_executor<g_current_render_api>;
		using bc_device_command_executor_ref = bc_device_ref<bc_device_command_executor>;
	}
}