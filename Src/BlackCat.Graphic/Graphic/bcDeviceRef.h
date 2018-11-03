// [12/22/2016 MRB]

#pragma once

#include "Graphic/bcRenderApi.h"
#include "Graphic/bcDeviceReference.h"

namespace black_cat
{
	namespace graphic
	{
		template< bc_render_api TApi, class TObject >
		struct bc_platform_device_ref_pack
		{
		};

		/**
		 * \brief Store a strong reference to device reference objects
		 * \tparam TApi
		 * \tparam TObject
		 */
		template< bc_render_api TApi, class TObject >
		class bc_platform_device_ref
		{
		public:
			using type = TObject;
			using platform_pack = bc_platform_device_ref_pack< TApi, TObject >;

		public:
			bc_platform_device_ref();

			/**
			 * \brief Construct associated object with parameter and add reference to object.
			 * \param p_object 
			 */
			template
			<
				typename = std::enable_if_t
				<
					std::is_base_of_v< bc_platform_device_reference< TApi >, std::decay_t< type > >
				>
			>
			explicit bc_platform_device_ref(const type& p_object);

			bc_platform_device_ref(bc_platform_device_ref&& p_other) noexcept;

			~bc_platform_device_ref();

			bc_platform_device_ref& operator=(bc_platform_device_ref&& p_other) noexcept;

			type* operator->() noexcept;

			const type* operator->() const noexcept;

			type& operator*();

			const type& operator*() const;

			type& get();

			const type& get() const;

			/**
			 * \brief Reset and release associated object.
			 */
			void reset();

			/**
			 * \brief Replace associated object. release old object and add reference to new object.
			 * \param p_object 
			 */
			void reset(const type& p_object);

			bool operator==(const bc_platform_device_ref& p_other) const noexcept;

			bool operator!=(const bc_platform_device_ref& p_other) const noexcept;

			bool operator==(std::nullptr_t) const noexcept;

			bool operator!=(std::nullptr_t) const noexcept;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			platform_pack m_pack;
		};

		template< typename TObject >
		using bc_device_ref = bc_platform_device_ref< g_current_render_api, TObject >;
	}
}