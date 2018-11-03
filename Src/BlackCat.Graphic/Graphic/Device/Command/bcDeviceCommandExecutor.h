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
		template< bc_render_api TRenderApi >
		struct bc_platform_device_command_executor_pack
		{
		};

		template< bc_render_api TRenderApi >
		class bc_platform_device_command_executor : public bc_platform_device_reference<TRenderApi>
		{
		public:
			using platform_pack = bc_platform_device_command_executor_pack< TRenderApi >;

		public:
			bc_platform_device_command_executor();

			bc_platform_device_command_executor(platform_pack& p_pack);

			bc_platform_device_command_executor(const bc_platform_device_command_executor&);

			~bc_platform_device_command_executor();

			bc_platform_device_command_executor& operator=(const bc_platform_device_command_executor&);

			void excecute_command_list(bc_device_command_list p_command_list);

			bool is_valid() const noexcept override;

			bool operator==(const bc_platform_device_command_executor& p_other) const noexcept;

			bool operator!=(const bc_platform_device_command_executor& p_other) const noexcept;

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

		using bc_device_command_executor = bc_platform_device_command_executor< g_current_render_api >;
		using bc_device_command_executor_ptr = bc_device_ref< bc_device_command_executor >;
	}
}