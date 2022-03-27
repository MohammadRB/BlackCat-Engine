// [03/28/2016 MRB]

#include "Core/CorePCH.h"
#include "Core/Content/bcContentManager.h"

namespace black_cat
{
	namespace core
	{
		bc_content_manager::bc_content_manager() = default;

		bc_content_manager::~bc_content_manager()
		{
			BC_ASSERT(m_contents.empty());
		}

		void bc_content_manager::destroy_content(const bci_content* p_content)
		{
			map_type::value_type::second_type l_value;

			{
				platform::bc_shared_mutex_guard l_guard(m_contents_mutex);

				const auto l_item = m_contents.find(p_content->_get_key());

				if (l_item == end(m_contents))
				{
					BC_ASSERT(false);
					return;
				}

				// Move value from container because contents may have inner content and destroying
				// content while lock is acquired cause deadlock
				l_value = std::move(l_item->second);
				m_contents.erase(l_item);
			}

			l_value.m_content.reset(nullptr);
		}
	}
}
