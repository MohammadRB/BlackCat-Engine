// [06/18/2021 MRB]

#pragma once

#include "Graphic/bcDeviceRef.h"
#include "Graphic/bcDeviceReference.h"
#include "Graphic/bcRenderApi.h"

namespace black_cat
{
	namespace graphic
	{
		template<bc_render_api>
		class bc_platform_device;
		using bc_device = bc_platform_device<g_current_render_api>;
		
		template<bc_render_api>
		class bc_platform_texture2d;
		using bc_texture2d = bc_platform_texture2d<g_current_render_api>;
		using bc_texture2d_ref = bc_device_ref<bc_texture2d>;
		
		template<bc_render_api TApi>
		struct bc_platform_device_swap_buffer_pack
		{
		};

		template<bc_render_api TApi>
		class bc_platform_device_swap_buffer : public bc_platform_device_reference<TApi>
		{
		public:
			using platform_pack = bc_platform_device_swap_buffer_pack<TApi>;

		public:
			bc_platform_device_swap_buffer();

			explicit bc_platform_device_swap_buffer(platform_pack& p_pack);

			bc_platform_device_swap_buffer(const bc_platform_device_swap_buffer&) noexcept;

			~bc_platform_device_swap_buffer() override;

			bc_platform_device_swap_buffer& operator=(const bc_platform_device_swap_buffer&) noexcept;

			bool get_vsync() const;

			void set_vsync(bool p_sync);
			
			bool get_full_screen() const;

			void set_full_screen(bc_device& p_device, bool p_full_screen);

			bc_texture2d get_back_buffer_texture() const;

			bcUINT32 get_back_buffer_width() const;

			bcUINT32 get_back_buffer_height() const;

			bc_format get_back_buffer_format() const;

			/**
			 * \brief Resize back buffer and send bc_app_event_device_reset event to resize all other resizable resources.
			 * If device is in fullscreen mode change resolution too.
			 * \param p_device
			 * \param p_width
			 * \param p_height
			 */
			void resize_back_buffer(bc_device& p_device, bcUINT p_width, bcUINT p_height);

			/**
			* \brief Resize back buffer and send bc_app_event_device_reset event to resize all other resizable resources.
			* If device is in fullscreen mode change resolution too.
			* \param p_device
			* \param p_width
			* \param p_height
			* \param p_format
			*/
			void resize_back_buffer(bc_device& p_device, bcUINT p_width, bcUINT p_height, bc_format p_format);

			void present();
			
			bool is_valid() const noexcept override;

			void set_debug_name(const bcCHAR* p_name) noexcept override;

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

		using bc_device_swap_buffer = bc_platform_device_swap_buffer<g_current_render_api>;
		using bc_device_swap_buffer_ref = bc_device_ref<bc_device_swap_buffer>;
	}	
}