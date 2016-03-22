// [02/16/2016 MRB]

#pragma once

#include "CorePlatform/bcCorePlatformUtility.h"

namespace black_cat
{
	namespace graphic
	{
		template<class>
		class bc_resource_ptr;

		class bc_device_object : public core_platform::bc_no_copy
		{
		public:
			template<class>
			friend class bc_resource_ptr;

		public:
			bc_device_object()
				: m_ref_count(0)
			{
			}

			virtual ~bc_device_object() = default;

			bc_device_object(bc_device_object&&) = default;

			bc_device_object& operator=(bc_device_object&&) = default;
			
		protected:
			void add_ref() noexcept
			{
				++m_ref_count;
			}

			// Return true if reference count reach zero
			bool dec_ref() noexcept
			{
				bcAssert(m_ref_count > 0);
				--m_ref_count;

				return m_ref_count == 0;
			}

		private:
			bcUINT m_ref_count;
		};
	}
}