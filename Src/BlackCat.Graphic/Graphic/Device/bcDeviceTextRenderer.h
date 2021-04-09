// [11/08/2020 MRB]

#pragma once

#include "Core/Container/bcString.h"
#include "Core/Math/bcVector2f.h"
#include "Core/Math/bcVector3f.h"
#include "Graphic/bcRenderApiInfo.h"

namespace black_cat
{
	namespace graphic
	{
		template< bc_render_api TRenderApi >
		class bc_platform_device;
		using bc_device = bc_platform_device< g_current_render_api >;

		template< bc_render_api TRenderApi >
		class bc_platform_render_target_view;
		using bc_render_target_view = bc_platform_render_target_view< g_current_render_api >;
		
		struct bc_device_text
		{
			bc_device_text(core::bc_wstring p_text, const core::bc_vector2i& p_position)
				: bc_device_text(std::move(p_text), p_position, core::bc_vector3f(1, 1, 1))
			{
			}

			bc_device_text(core::bc_wstring p_text, const core::bc_vector2i& p_position, const core::bc_vector3f& p_color)
				: bc_device_text(std::move(p_text), p_position, p_color, 1)
			{
			}

			bc_device_text(core::bc_wstring p_text, const core::bc_vector2i& p_position, float p_scale)
				: bc_device_text(std::move(p_text), p_position, core::bc_vector3f(1, 1, 1), p_scale)
			{
			}

			bc_device_text(core::bc_wstring p_text, const core::bc_vector2i& p_position, const core::bc_vector3f& p_color, float p_scale)
				: m_text(std::move(p_text)),
				m_position(p_position),
				m_color(p_color),
				m_scale(p_scale)
			{
			}

			bc_device_text(const bc_device_text&) = default;
			
			bc_device_text(bc_device_text&&) = default;

			~bc_device_text() = default;

			bc_device_text& operator=(const bc_device_text&) = default;
			
			bc_device_text& operator=(bc_device_text&&) = default;
			
			core::bc_wstring m_text;
			core::bc_vector2i m_position;
			core::bc_vector3f m_color;
			float m_scale;
		};
		
		template< bc_render_api TRenderApi >
		struct bc_platform_device_text_renderer_pack
		{
		};

		template< bc_render_api TRenderApi >
		class bc_platform_device_text_renderer
		{
		public:
			using platform_pack = bc_platform_device_text_renderer_pack<TRenderApi>;

		public:
			explicit bc_platform_device_text_renderer(platform_pack p_parameter) noexcept;
			
			bc_platform_device_text_renderer(bc_platform_device_text_renderer&&) noexcept;

			~bc_platform_device_text_renderer();

			bc_platform_device_text_renderer& operator=(bc_platform_device_text_renderer&&) noexcept;

			core::bc_vector2f measure_text(const bcWCHAR* p_text);
			
			void draw_texts(bc_device& p_device, bc_render_target_view& p_back_buffer_view, const bc_device_text* p_texts, bcSIZE p_count);
			
			platform_pack& get_platform_pack() noexcept
			{
				return m_pack;
			}

			const platform_pack& get_platform_pack() const noexcept
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;
		};

		using bc_device_text_renderer = bc_platform_device_text_renderer< g_current_render_api >;
	}
}