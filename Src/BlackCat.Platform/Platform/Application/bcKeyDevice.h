// [04/14/2016 MRB]

#pragma once

#include "CorePlatform/bcType.h"
#include "CorePlatform/bcPlatform.h"
#include "Core/Messaging/Event/bcEventManager.h"

namespace black_cat
{
	namespace platform
	{
		class bc_human_interface_device;

		enum class bc_key : bcUBYTE
		{
			// Mouse
			ms_left_button =	1,
			ms_right_button =	2,
			ms_middle_button =	4,
			ms_x1_button =		5,
			ms_x2_button =		6,
			// Keyboard
			kb_esc =			27,
			kb_f1 =				112,
			kb_f2 =				113,
			kb_f3 =				114,
			kb_f4 =				115,
			kb_f5 =				116,
			kb_f6 =				117,
			kb_f7 =				118,
			kb_f8 =				119,
			kb_f9 =				120,
			kb_f10 =			121,
			kb_f11 =			122,
			kb_f12 =			123,
			kb_print_screen =	44,
			kb_scroll_lock =	145,
			kb_pause_break =	19,
			kb_insert =			45,
			kb_home =			36,
			kb_pageup =			33,
			kb_del =			46,
			kb_end =			35,
			kb_pagedown =		34,
			kb_numlock =		144,
			kb_capslock =		20,
			kb_backspace =		8,
			kb_d0 =				48,
			kb_d1 =				49,
			kb_d2 =				50,
			kb_d3 =				51,
			kb_d4 =				52,
			kb_d5 =				53,
			kb_d6 =				54,
			kb_d7 =				55,
			kb_d8 =				56,
			kb_d9 =				57,
			kb_grave =			192,
			kb_dash =			189,
			kb_equal =			187,
			kb_open_bracket =	219,
			kb_close_bracket =	221,
			kb_back_slash =		220,
			kb_semi_colon =		186,
			kb_single_quote =	222,
			kb_comma =			188,
			kb_period =			190,
			kb_forward_slash =	191,
			kb_left_arrow =		37,
			kb_right_arrow =	39,
			kb_up_arrow =		38,
			kb_down_arrow =		40,
			kb_divide =			111,
			kb_multipy =		106,
			kb_subtract =		109,
			kb_add =			107,
			kb_decimal_point =	110,
			kb_num0 =			96,
			kb_num1 =			97,
			kb_num2 =			98,
			kb_num3 =			99,
			kb_num4 =			100,
			kb_num5 =			101,
			kb_num6 =			102,
			kb_num7 =			103,
			kb_num8 =			104,
			kb_num9 =			105,
			kb_tab =			9,
			kb_shift =			16,
			kb_ctrl =			17,
			kb_alt =			18,
			kb_left_platform =	91,
			kb_right_platform =	93,
			kb_enter =			13,
			kb_space =			32,
			kb_left_shift =		160,
			kb_left_ctrl =		162,
			kb_left_alt =		164,
			kb_right_shift =	161,
			kb_right_ctrl =		163,
			kb_right_alt =		165,
			kb_Q =				81,
			kb_W =				87,
			kb_E =				69,
			kb_R =				82,
			kb_T =				84,
			kb_Y =				89,
			kb_U =				85,
			kb_I =				73,
			kb_O =				79,
			kb_P =				80,
			kb_A =				65,
			kb_S =				83,
			kb_D =				68,
			kb_F =				70,
			kb_G =				71,
			kb_H =				72,
			kb_J =				74,
			kb_K =				75,
			kb_L =				76,
			kb_Z =				90,
			kb_X =				88,
			kb_C =				67,
			kb_V =				86,
			kb_B =				66,
			kb_N =				78,
			kb_M =				77,
		};

		enum class bc_key_state : bcUBYTE
		{
			// Key did not pressed
			up = 0,
			// Key pressed in this frame
			pressing = 1,
			// Key was pressed in this frame and last frames
			pressed = 2,
			// Key was pressed in last frame but has been released in this frame
			releasing = 4
		};

		template< core_platform::bc_platform TPlatform >
		struct bc_platform_key_device_pack
		{
			
		};

		template< core_platform::bc_platform TPlatform >
		class bc_platform_key_device
		{
		public:
			using platform_pack = bc_platform_key_device_pack< TPlatform >;
			friend class bc_human_interface_device;

		public:
			bc_platform_key_device(const bc_platform_key_device&) noexcept;

			~bc_platform_key_device();

			bc_platform_key_device& operator=(const bc_platform_key_device&) noexcept;

			void update();

			bc_key_state get_key_state(bc_key p_key) const noexcept;

		private:
			explicit bc_platform_key_device(bcUBYTE p_device_index);

			bcBYTE m_device_index;
			platform_pack m_pack;
			core::bc_event_manager* m_event_manager;
		};

		using bc_key_device = bc_platform_key_device< core_platform::g_current_platform >;
	}
}
