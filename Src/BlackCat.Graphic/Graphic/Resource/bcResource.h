// [01/03/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcDeviceReference.h"
#include "Graphic/bcDeviceRef.h"
#include "Graphic/bcRenderApi.h"
#include "Graphic/bcGraphicDefinition.h"

namespace black_cat
{
	namespace graphic
	{
		class bc_subresource_data
		{
		public:
			bc_subresource_data(const void* p_data, bcUINT p_row_pitch, bcUINT p_depth_pitch)
				: m_data(p_data), 
				m_row_pitch(p_row_pitch),
				m_depth_pitch(p_depth_pitch)
			{
			}

			const void* m_data;
			bcUINT m_row_pitch;
			bcUINT m_depth_pitch;
		};

		struct bc_mapped_resource
		{
		public:
			void* m_data;
			bcUINT m_row_pitch;
			bcUINT m_depth_pitch;
		};

		template<bc_render_api TRenderApi>
		struct bci_platform_resource_pack
		{
		};

		template<bc_render_api TRenderApi>
		class bci_platform_resource : public bc_platform_device_reference<TRenderApi>
		{
		public:
			using platform_pack = bci_platform_resource_pack<TRenderApi>;

		public:
			virtual ~bci_platform_resource() override;

			virtual bc_resource_type get_type() const noexcept = 0;

			bc_resource_eviction_priority get_eviction_priority() const;

			void set_eviction_priority(bc_resource_eviction_priority p_eviction_priority);

			virtual platform_pack& get_resource_platform_pack() noexcept = 0;

			virtual const platform_pack& get_resource_platform_pack() const noexcept = 0;
		
		protected:
			bci_platform_resource() noexcept;

			explicit bci_platform_resource(platform_pack& p_pack) noexcept;

			bci_platform_resource(const bci_platform_resource& p_other) noexcept;

			bci_platform_resource& operator=(const bci_platform_resource& p_other) noexcept;
		};

		using bci_resource = bci_platform_resource<g_current_render_api>;
	}
}