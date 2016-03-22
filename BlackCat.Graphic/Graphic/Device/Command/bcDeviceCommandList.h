// [01/22/2016 MRB]

#pragma once

#include "Graphic/bcPlatformRenderApi.h"
#include "Graphic/bcDeviceObject.h"
#include "Graphic/bcResourcePtr.h"

namespace black_cat
{
	namespace graphic
	{
		template< bc_platform_render_api TRenderApi >
		struct bc_platform_device_command_list_pack
		{

		};

		template< bc_platform_render_api TRenderApi >
		class bc_platform_device_command_list : public bc_device_object
		{
		public:
			using platform_pack = bc_platform_device_command_list_pack<TRenderApi>;

		public:
			bc_platform_device_command_list();

			bc_platform_device_command_list(bc_platform_device_command_list&&);

			~bc_platform_device_command_list();

			bc_platform_device_command_list& operator=(bc_platform_device_command_list&&);

			void reset();

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			platform_pack m_pack;
		};

		using bc_device_command_list = bc_platform_device_command_list< g_current_platform_render_api >;

		using bc_device_command_list_ptr = bc_resource_ptr< bc_device_command_list >;
	}
}