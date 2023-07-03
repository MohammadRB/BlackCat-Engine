// [14/04/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "CorePlatform/bcPlatform.h"
#include "Core/Messaging/Event/bcEventManager.h"

namespace black_cat
{
	namespace platform
	{
		class bc_human_interface_device;

		struct bc_pointing_device_state
		{
			bc_pointing_device_state()
				: m_x(0), m_y(0), m_dx(0), m_dy(0)
			{
			}

			bcINT16 m_x;
			bcINT16 m_y;
			bcINT16 m_dx;
			bcINT16 m_dy;
		};

		template<bc_platform TPlatform>
		struct bc_platform_pointing_device_pack
		{
		};

		template<bc_platform TPlatform>
		class bc_platform_pointing_device
		{
		public:
			using platform_pack = bc_platform_pointing_device_pack<TPlatform>;
			friend class bc_human_interface_device;

		public:
			bc_platform_pointing_device(const bc_platform_pointing_device&) noexcept;

			~bc_platform_pointing_device();

			bc_platform_pointing_device& operator=(const bc_platform_pointing_device&) noexcept;

			bool get_visibility() const noexcept;

			void set_visibility(bool p_show) noexcept;

			bc_pointing_device_state get_state() const noexcept;

			void set_position(bcINT16 p_x, bcINT16 p_y) noexcept;

			bc_pointing_device_state update();

		private:
			explicit bc_platform_pointing_device(bcUBYTE p_device_index);

			bcBYTE m_device_index;
			platform_pack m_pack;
			core::bc_event_manager* m_event_manager;
		};

		using bc_pointing_device = bc_platform_pointing_device<g_current_platform>;
	}
}