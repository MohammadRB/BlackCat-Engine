#ifndef bcCoreEvents_H
#define bcCoreEvents_H

#include "CorePCH.h"
#include "bcEvent.h"
#include "bcString.h"

namespace BlackCat
{
	namespace Core
	{
		static class BC_COREDLL_EXP bcCoreEvents
		{
		public:
			/*bcInline static const bcWString& EventError()
			{
				static const bcWString lError(L"ErrorMsg");
				return lError;
			};
			bcInline static const bcWString& EventWarning()
			{
				static const bcWString lError(L"WarningMsg");
				return lError;
			};*/

			static struct EventError
			{
			public:
				bcInline operator const bcWString&() const
				{
					static const bcWString lError(L"ErrorMsg");
					return lError;
				};
				bcInline operator const bcUINT32() const
				{
					static const bcUINT32 lError(bcWString(L"ErrorMsg").Hash());
					return lError;
				};
			};
			static struct EventWarning
			{
			public:
				bcInline operator const bcWString&() const
				{
					static const bcWString lError(L"WarningMsg");
					return lError;
				};
				bcInline operator const bcUINT32() const
				{
					static const bcUINT32 lError(bcWString(L"WarningMsg").Hash());
					return lError;
				};
			};

			static const EventError mEventError;
			static const EventWarning mEventWarning;

			/*static struct EventSngCreated
			{
			public:
				bcInline operator const bcWString&() const
				{
					static const bcWString lError(L"SngCreated");
					return lError;
				};
				bcInline operator const bcUINT32() const
				{
					static const bcUINT32 lError(bcWString(L"SngCreated").Hash());
					return lError;
				};
			};
			static struct EventSngGeted
			{
			public:
				bcInline operator const bcWString&() const
				{
					static const bcWString lError(L"SngGeted");
					return lError;
				};
				bcInline operator const bcUINT32() const
				{
					static const bcUINT32 lError(bcWString(L"SngGeted").Hash());
					return lError;
				};
			};
			static struct EventSngDestroyed
			{
			public:
				bcInline operator const bcWString&() const
				{
					static const bcWString lError(L"SngDestroyed");
					return lError;
				};
				bcInline operator const bcUINT32() const
				{
					static const bcUINT32 lError(bcWString(L"SngDestroyed").Hash());
					return lError;
				};
			};

			static const EventSngCreated mEventSngCreated;
			static const EventSngGeted mEventSngGeted;
			static const EventSngDestroyed mEventSngDestroyed;*/
		};

		class BC_COREDLL_EXP bcEventError : public bcEvent
		{
		protected:
			bcWString mErrorMsg;

		public:
			bcEventError(const bcWCHAR* pErrorMsg)
			{
				mErrorMsg = pErrorMsg;
			};
			bcEventError(bcWString pErrorMsg)
			{
				mErrorMsg = pErrorMsg;
			};
			virtual ~bcEventError()
			{
			};

			virtual const bcUINT32 GetEventIndex() const
			{
				return bcCoreEvents::mEventError;
			};
			virtual const bcWString& GetEventName() const
			{
				return bcCoreEvents::mEventError;
			};
			/*virtual operator const bcUINT32()
			{
				static const bcUINT32 lError(bcWString(L"ErrorMsg").Hash());
				return lError;
			};
			virtual operator const bcWString&()
			{
				static const bcWString lError(L"ErrorMsg");
				return lError;
			};*/

			const bcWString& GetErrorMsg() const
			{
				return mErrorMsg;
			};
		};

		class BC_COREDLL_EXP bcEventWarning : public bcEvent
		{
		protected:
			bcWString mWarningMsg;

		public:
			bcEventWarning(const bcWCHAR* pWarningMsg)
			{
				mWarningMsg = pWarningMsg;
			};
			bcEventWarning(bcWString pWarningMsg)
			{
				mWarningMsg = pWarningMsg;
			};
			virtual ~bcEventWarning()
			{
			};

			virtual const bcUINT32 GetEventIndex() const
			{
				return bcCoreEvents::mEventWarning;
			};
			virtual const bcWString& GetEventName() const
			{
				return bcCoreEvents::mEventWarning;
			};
			/*virtual operator const bcUINT32()
			{
				static const bcUINT32 lError(bcWString(L"WarningMsg").Hash());
				return lError;
			};
			virtual operator const bcWString&()
			{
				static const bcWString lError(L"WarningMsg");
				return lError;
			};*/
			
			const bcWString& GetWarningMsg() const
			{
				return mWarningMsg;
			};
		};
	}
}

#endif