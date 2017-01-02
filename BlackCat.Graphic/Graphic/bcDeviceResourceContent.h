// [04/09/2016 MRB]

#pragma once

#include "Core/Container/bcString.h"
#include "Core/File/bcContent.h"
#include "Graphic/bcDeviceRef.h"

namespace black_cat
{
	namespace graphic
	{
		// Wrap device resource pointers and make required interface for content manager
		template< class TResource >
		class bc_device_resource_content : public core::bc_icontent
		{
		public:
			explicit bc_device_resource_content(bc_device_ref<TResource>&& p_resource) noexcept
				: m_resource(std::move(p_resource))
			{
			}

			bc_device_resource_content(bc_device_resource_content&&) noexcept = default;

			~bc_device_resource_content() = default;

			bc_device_resource_content& operator=(bc_device_resource_content&&)  noexcept = default;

			bc_device_ref<TResource>& get_resource() noexcept
			{
				return m_resource;
			}

			static constexpr bcCHAR* content_name();

			static constexpr bcUINT32 content_hash();

		protected:

		private:
			bc_device_ref<TResource> m_resource;
		};
	}
}