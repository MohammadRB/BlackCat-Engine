// [06/27/2016 MRB]

#pragma once

#include "Core/bcException.h"
#include "Core/Container/bcString.h"
#include "Core/File/bcContent.h"
#include "Core/File/bcContentStreamManager.h"

namespace black_cat
{
	namespace core
	{
		class bc_lazy_content
		{
		public:
			explicit bc_lazy_content(bc_string p_content_name)
				: m_content_name(std::move(p_content_name))
			{
			}

			bc_icontent_ptr get_content() const
			{
				auto* l_content_stream_manager = bc_service_manager::get().get_service< bc_content_stream_manager >();
				auto l_content = l_content_stream_manager->find_content(m_content_name.c_str());

				if(l_content == nullptr)
				{
					throw bc_key_not_found_exception("Content not found");
				}

				return l_content;
			}
			
			template< typename TContent >
			bc_content_ptr< TContent > get_content() const
			{
				auto l_content = get_content();

				return static_cast<bc_content_ptr< TContent >>(l_content);
			}

			operator bc_icontent_ptr() const
			{
				return get_content();
			}

			template< typename TContent >
			operator bc_content_ptr< TContent >() const
			{
				return get_content<TContent>();
			}

		protected:

		private:
			bc_string m_content_name;
		};
	}
}
