// [01/22/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcRenderApi.h"
#include "Graphic/bcDeviceObject.h"
#include "Graphic/bcResourcePtr.h"
#include "Graphic/Device/bcDevice.h"
#include "Graphic/Device/Command/bcDeviceCommandList.h"

namespace black_cat
{
	namespace graphic
	{
		template< bc_render_api TRenderApi >
		struct bc_platform_device_command_executer_pack
		{
		};

		template< bc_render_api TRenderApi >
		class bc_platform_device_command_executer : public bc_device_object
		{
		public:
			using platform_pack = bc_platform_device_command_executer_pack< TRenderApi >;

		public:
			explicit bc_platform_device_command_executer(bc_device& p_device);

			bc_platform_device_command_executer(bc_platform_device_command_executer&&);

			~bc_platform_device_command_executer();

			bc_platform_device_command_executer& operator=(bc_platform_device_command_executer&&);

			void excecute_command_list(bc_device_command_list* p_command_list);

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			bc_device& m_device;
			platform_pack m_pack;
		};

		using bc_device_command_executer = bc_platform_device_command_executer< g_current_render_api >;

		using bc_device_command_executer_ptr = bc_resource_ptr< bc_device_command_executer >;
	}
}