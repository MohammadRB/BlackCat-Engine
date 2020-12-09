// [2/11/2015 MRB]

#include "Core/CorePCH.h"
#include "Core/bcConstant.h"
#include "Core/Messaging/Event/bcEvent.h"
#include "Core/Messaging/Event/bcEventManager.h"
#include "Core/Container/bcString.h"

namespace black_cat
{
	namespace core
	{
		bc_ievent::~bc_ievent()
		{		
		}

		bc_ievent::bc_ievent(const bcCHAR* p_name)
			: bc_imessage(p_name)
		{
		}

		bc_event::bc_event(const bcCHAR* p_name) noexcept
			: bc_ievent(p_name)
		{
		}

		bc_event::~bc_event()
		{
		}

		bc_app_event::bc_app_event(const bcCHAR* p_name) noexcept
			: bc_event(p_name)
		{
		}

		bc_app_event::~bc_app_event()
		{
		}

		bc_app_render_event::bc_app_render_event(const bcCHAR* p_name) noexcept
			: bc_event(p_name)
		{
		}

		bc_app_render_event::~bc_app_render_event()
		{
		}
	}
}