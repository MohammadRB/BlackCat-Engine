#ifndef bcEvent_H
#define bcEvent_H

#include "CorePCH.h"
#include "bcString.h"

namespace BlackCat
{
	namespace Core
	{
		//enum eEVENT
		//{
		//	//GUI_CURSOR_OVER,
		//	//GUI_CURSOR_ENTER,
		//	//GUI_CURSOR_LEAVE,
		//	//GUI_CURSOR_BUTTON_DOWN,
		//	//GUI_CURSOR_BUTTON_PRESSED,
		//	//GUI_CURSOR_BUTTON_UP,
		//	//GUI_UI_BUTTON_PRESSED,
		//	RENDER_FRAME_START,
		//	SYSTEM_LBUTTON_DOWN,
		//	SYSTEM_LBUTTON_UP,
		//	SYSTEM_MBUTTON_DOWN,
		//	SYSTEM_MBUTTON_UP,
		//	SYSTEM_RBUTTON_DOWN,
		//	SYSTEM_RBUTTON_UP,
		//	SYSTEM_MOUSE_LEAVE,
		//	SYSTEM_MOUSE_WHEEL,
		//	SYSTEM_MOUSE_MOVE,
		//	SYSTEM_KEYBOARD_KEYDOWN,
		//	SYSTEM_KEYBOARD_KEYUP,
		//	SYSTEM_KEYBOARD_CHAR,
		//	WINDOW_RESIZE,
		//	LOG_MESSAGE,
		//	INFO_MESSAGE,
		//	ERROR_MESSAGE,
		//	//TEXT_CONTROLLER_STATE_REQUEST,
		//	//ENTITY3D_CONTROLLER_STATE_REQUEST,
		//	NUM_EVENTS
		//};

		/*template< const bcWCHAR* mSTR > struct bcEventEnum
		{
		public:
			bcInline operator const bcWString&() const
			{
				static const bcWString lEventName(mStr);
				return lEventName;
			};
			bcInline operator const bcUINT32() const
			{
				static const bcUINT32 lEventIndex(bcWString(mStr).Hash());
				return lEventIndex;
			};
		};*/

		class BC_COREDLL_EXP bcEvent
		{
		public:
			virtual const bcUINT32 GetEventIndex() const = 0;
			virtual const bcWString& GetEventName() const = 0;
			/*virtual operator const bcUINT32() const = 0;
			virtual operator const bcWString&() const = 0;*/
			//virtual eEVENT GetEventType() = 0;
		};
	}
}
#endif