#ifndef bcEventManager_H
#define bcEventManager_H

#include "CorePCH.h"
#include "bcEvent.h"
#include "bcEventListener.h"
#include "bcEventHandle.h"
#include "bcMap.h"
#include "bcVector.h"

namespace BlackCat
{
	namespace Core
	{
		class BC_COREDLL_EXP bcEventManager
		{
		public:
			typedef bcEventHandle< bcBOOL( bcEvent* ) > EventHandleType;
			typedef bcDelegate< bcBOOL( bcEvent* ) > DelegateType;
		private:

		protected:
			static bcEventManager* mInstance;

			bcMap< bcUINT32, EventHandleType > mHandlers;
			bcVector< bcEvent*, 25 > mEventQueue;

		public:
			bcEventManager();
			~bcEventManager();

			void Initialize();
			static void StartUp();
			static void Close();
			bcInline static bcEventManager* Get()
			{
				return bcEventManager::mInstance;
			};

			void AddEventListener(bcWString pEventName, bcEventListener* pListener);
			void DelEventListener(bcWString pEventName, bcEventListener* pListener);

			bcBOOL ProcessEvent(bcEvent* pEvent);
			bcBOOL QueueEvent(bcEvent* pEvent);
			bcBOOL ProcessEventQueue();
		};
	}
}

#endif