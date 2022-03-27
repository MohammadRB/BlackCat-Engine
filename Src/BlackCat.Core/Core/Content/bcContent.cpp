// [04/04/2016 MRB]

#include "Core/CorePCH.h"
#include "Core/Content/bcContent.h"
#include "Core/Content/bcContentManager.h"

namespace black_cat
{
	namespace core
	{
		_bc_content_ptr_deleter::_bc_content_ptr_deleter()
			: m_content_manager(nullptr) 
		{
		}

		_bc_content_ptr_deleter::_bc_content_ptr_deleter(bc_content_manager* p_content_manager)
			: m_content_manager(p_content_manager)
		{
		}

		void _bc_content_ptr_deleter::operator()(const bci_content* p_content) const
		{
			BC_ASSERT(m_content_manager != nullptr);

			m_content_manager->destroy_content(p_content);
		}
	}
}
