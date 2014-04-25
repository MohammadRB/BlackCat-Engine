#include "CorePCH.h"
#include "bcEventManager.h"

namespace BlackCat
{
	namespace Core
	{
		BC_COREDLL_EXP bcEventManager* bcEventManager::mInstance;

		bcEventManager::bcEventManager()
		{
		};
		bcEventManager::~bcEventManager()
		{
		};

		void bcEventManager::Initialize()
		{
		};
		void bcEventManager::StartUp()
		{
			bcEventManager::mInstance = new bcEventManager();
			bcEventManager::mInstance->Initialize();
		};
		void bcEventManager::Close()
		{
			delete bcEventManager::mInstance;
		};

		void bcEventManager::AddEventListener(bcWString pEventName, bcEventListener* pListener)
		{
			bcUINT32 lHash = pEventName.Hash();
			auto lIte = mHandlers.Find(lHash);
			
			// If we have this EventHandler type only add a delegate to it
			if(lIte.validate())
				*lIte += DelegateType(pListener, &bcEventListener::HandleEvent);
			// Else create an EventHandle and add a delegate to it and add it
			else
			{
				EventHandleType lEventHwn;
				lEventHwn += DelegateType(pListener, &bcEventListener::HandleEvent);
				mHandlers.PushBack(bcPair< bcUINT32, EventHandleType >(lHash, lEventHwn));
			}
		};
		void bcEventManager::DelEventListener(bcWString pEventName, bcEventListener* pListener)
		{
			bcUINT32 lHash = pEventName.Hash();
			auto lIte = mHandlers.Find(lHash);

			bcAssert(lIte.validate());

			*lIte -= DelegateType(pListener, &bcEventListener::HandleEvent);

			// If this EventHandle don't has any delegate remove it from 
			if(lIte->DelegateCount() == 0)
				mHandlers.Erase(lIte);
		};
		
		bcBOOL bcEventManager::ProcessEvent(bcEvent* pEvent)
		{
			if(pEvent == NULL)
				return bcFALSE;

			bcUINT32 lHash = pEvent->GetEventIndex();
			auto lIte = mHandlers.Find(lHash);
			
			bcAssert(lIte.validate());

			for(bcUINT32 i = 0; i < lIte->DelegateCount(); ++i)
			{
				(*lIte)(pEvent);
			}

			return bcTRUE;
		};
		bcBOOL bcEventManager::QueueEvent(bcEvent* pEvent)
		{
			return bcTRUE;
		};
		bcBOOL bcEventManager::ProcessEventQueue()
		{
			return bcTRUE;
		};
	}
}