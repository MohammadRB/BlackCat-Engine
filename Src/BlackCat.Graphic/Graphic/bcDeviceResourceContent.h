// [04/09/2016 MRB]

#pragma once

#include "Core/Content/bcContent.h"
#include "Graphic/bcDeviceRef.h"

namespace black_cat
{
	namespace graphic
	{
		/**
		 * \brief Wrap device resource pointers and make required interface for content manager.
		 * \tparam TResource
		 * \tparam TContentName
		 */
		template< class TResource, const bcCHAR TContentName[] >
		class bc_device_resource_content : public core::bci_content
		{
		public:
			explicit bc_device_resource_content(bc_device_ref<TResource>&& p_resource) noexcept
				: m_resource(std::move(p_resource))
			{
			}

			bc_device_resource_content(bc_device_resource_content&&) noexcept = default;

			~bc_device_resource_content() = default;

			bc_device_resource_content& operator=(bc_device_resource_content&&)  noexcept = default;

			TResource get_resource() noexcept
			{
				return m_resource.get();
			}

			static constexpr const bcCHAR* content_name()
			{
				return TContentName;
			}

			static constexpr bcUINT32 content_hash()
			{
				return BC_COMPILE_TIME_STRING_HASH(content_name());
			}

		private:
			bc_device_ref<TResource> m_resource;
		};
	}
}