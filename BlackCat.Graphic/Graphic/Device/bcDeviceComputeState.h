// [09/09/2016 MRB]

#pragma once

#include "Graphic/bcPlatformRenderApi.h"
#include "Graphic/bcDeviceObject.h"
#include "Graphic/bcResourcePtr.h"
#include "Graphic/Shader/bcComputeShader.h"

namespace black_cat
{
	namespace graphic
	{
		struct bc_device_compute_state_config
		{
			bc_compute_shader_ptr m_compute_shader;
		};

		template< bc_platform_render_api TRenderApi >
		struct bc_platform_device_compute_state_pack
		{
			bc_device_compute_state_config m_config;
		};

		template< bc_platform_render_api TRenderApi >
		class bc_platform_device_compute_state : public bc_device_object
		{
		public:
			using platform_pack = bc_platform_device_compute_state_pack<TRenderApi>;

		public:
			bc_platform_device_compute_state();

			bc_platform_device_compute_state(bc_platform_device_compute_state&&) noexcept;

			~bc_platform_device_compute_state();

			bc_platform_device_compute_state& operator=(bc_platform_device_compute_state&&) noexcept;

			bc_device_compute_state_config& get_config()
			{
				return m_pack.m_config;
			}

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		protected:

		private:
			platform_pack m_pack;
		};

		using bc_device_compute_state = bc_platform_device_compute_state<g_current_platform_render_api>;

		using bc_device_compute_state_ptr = bc_resource_ptr<bc_device_compute_state>;
	}
}
