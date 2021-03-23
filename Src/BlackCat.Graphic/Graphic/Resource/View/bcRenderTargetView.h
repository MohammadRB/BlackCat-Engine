// [02/11/2016 MRB]

#pragma once

#include "Graphic/bcRenderApi.h"
#include "Graphic/bcGraphicDefinition.h"
#include "Graphic/bcDeviceReference.h"
#include "Graphic/bcDeviceRef.h"
#include "Graphic/Resource/View/bcIResourceView.h"
#include "Graphic/Resource/View/bcRenderTargetViewConfig.h"

namespace black_cat
{
	namespace graphic
	{
		template< bc_render_api TRenderApi >
		struct bc_platform_render_target_view_pack
		{
		};

		template< bc_render_api TRenderApi >
		class bc_platform_render_target_view : public bci_platform_resource_view<TRenderApi>
		{
		public:
			using platform_pack = bc_platform_render_target_view_pack<TRenderApi>;

		public:
			bc_platform_render_target_view();

			explicit bc_platform_render_target_view(platform_pack& p_pack);

			bc_platform_render_target_view(const bc_platform_render_target_view&);

			~bc_platform_render_target_view();

			bc_platform_render_target_view& operator=(const bc_platform_render_target_view&);

			bc_resource_view_type get_view_type() const override;

			bool is_valid() const noexcept override;

			void set_debug_name(const bcCHAR* p_name) noexcept override;
			
			bool operator==(const bc_platform_render_target_view& p_other) const noexcept;

			bool operator!=(const bc_platform_render_target_view& p_other) const noexcept;

			bool operator==(std::nullptr_t) const noexcept;

			bool operator!=(std::nullptr_t) const noexcept;

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;
		};

		using bc_render_target_view = bc_platform_render_target_view< g_current_render_api >;
		using bc_render_target_view_ref = bc_device_ref<bc_render_target_view>;
	}
}