// [09/09/2016 MRB]

#pragma once

#include "Graphic/bcRenderApi.h"
#include "Graphic/bcDeviceReference.h"
#include "Graphic/bcDeviceRef.h"
#include "Graphic/Shader/bcComputeShader.h"

namespace black_cat
{
	namespace graphic
	{
		struct bc_device_compute_state_config
		{
			bc_compute_shader m_compute_shader;
		};

		template<bc_render_api TRenderApi>
		struct bc_platform_device_compute_state_pack
		{
		};

		template<bc_render_api TRenderApi>
		class bc_platform_device_compute_state : public bc_platform_device_reference<TRenderApi>
		{
		public:
			using platform_pack = bc_platform_device_compute_state_pack<TRenderApi>;

		public:
			bc_platform_device_compute_state() noexcept;

			explicit bc_platform_device_compute_state(platform_pack& p_pack) noexcept;

			bc_platform_device_compute_state(const bc_platform_device_compute_state&) noexcept;

			~bc_platform_device_compute_state();

			bc_platform_device_compute_state& operator=(const bc_platform_device_compute_state&) noexcept;

			const bc_device_compute_state_config& get_config() const;

			bool is_valid() const noexcept override;

			void set_debug_name(const bcCHAR* p_name) noexcept override;
			
			bool operator==(const bc_platform_device_compute_state& p_other) const noexcept;

			bool operator!=(const bc_platform_device_compute_state& p_other) const noexcept;

			bool operator==(std::nullptr_t) const noexcept;

			bool operator!=(std::nullptr_t) const noexcept;

			platform_pack& get_platform_pack() noexcept override
			{
				return m_pack;
			}

			const platform_pack& get_platform_pack() const noexcept override
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;
		};

		using bc_device_compute_state = bc_platform_device_compute_state<g_current_render_api>;
		using bc_device_compute_state_ref = bc_device_ref<bc_device_compute_state>;
	}
}