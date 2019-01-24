// [06/27/2016 MRB]

#pragma once

#include "Core/bcException.h"
#include "Core/Container/bcString.h"
#include "Core/Content/bcContent.h"
#include "Core/Content/bcContentStreamManager.h"

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
				return get_content(m_content_name.c_str());
			}
			
			template< typename TContent >
			bc_content_ptr< TContent > get_content() const
			{
				return get_content<TContent>(m_content_name.c_str());
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

			static bc_icontent_ptr get_content(const bcCHAR* p_content_name)
			{
				static auto* l_content_stream_manager = bc_get_service< bc_content_stream_manager >();
				auto l_content = l_content_stream_manager->find_content(p_content_name);

				if (l_content == nullptr)
				{
					throw bc_key_not_found_exception("Content not found");
				}

				return l_content;
			}

			template< typename TContent >
			static bc_content_ptr< TContent > get_content(const bcCHAR* p_content_name)
			{
				auto l_content = get_content(p_content_name);
				if(l_content == nullptr)
				{
					return l_content;
				}

				return static_cast<bc_content_ptr< TContent >>(l_content);
			}

		protected:

		private:
			bc_string m_content_name;
		};
	}
}