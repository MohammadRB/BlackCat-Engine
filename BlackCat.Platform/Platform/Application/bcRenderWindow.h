// [2/27/2015 MRB]

#pragma once

#include "Platform/PlatformPCH.h"

namespace black_cat
{
	namespace platform
	{
		enum class bc_messagebox_type
		{
			none,
			error,
			warning,
			information
		};

		enum class bc_messagebox_buttom
		{
			ok,
			ok_cancel,
			retry_cancel,
			yes_no,
			yes_no_cancel
		};

		enum class bc_messagebox_value
		{
			ok,
			cancel,
			retry,
			yes,
			no
		};

		template< core_platform::bc_platform >
		struct bc_render_window_parameter_pack
		{
		public:
			bc_render_window_parameter_pack(core::bc_estring p_caption, bcUINT32 p_width, bcUINT32 p_height)
				: m_caption(std::move(p_caption)),
				m_width(p_width),
				m_height(p_height)
			{
			}

			core::bc_estring m_caption;
			bcUINT32 m_width;
			bcUINT32 m_height;
		};

		using bc_render_window_parameter = bc_render_window_parameter_pack< core_platform::g_current_platform >;

		template< core_platform::bc_platform >
		struct bc_render_window_pack
		{
			
		};

		template< core_platform::bc_platform TPlatform >
		class bc_render_window_proxy : private core_platform::bc_no_copy
		{
		public:
			using platform_pack = bc_render_window_pack< TPlatform >;

		public:
			bc_render_window_proxy(bc_render_window_parameter& p_parameters);

			bc_render_window_proxy(bc_render_window_proxy&& p_other) noexcept(true);

			~bc_render_window_proxy();

			bc_render_window_proxy& operator =(bc_render_window_proxy&& p_other) noexcept(true);

			bcUINT32 width() const noexcept(true);

			void width(bcUINT32 p_width) noexcept(true);

			bcUINT32 height() const noexcept(true);

			void height(bcUINT32 p_height) noexcept(true);

			bcUINT32 left() const noexcept(true);

			void left(bcUINT32 p_left) noexcept(true);

			bcUINT32 top() const noexcept(true);

			void top(bcUINT32 p_top) noexcept(true);

			void set_size(bcUINT32 p_width, bcUINT32 p_height) noexcept(true);

			void set_position(bcUINT32 p_left, bcUINT32 p_top) noexcept(true);

			core::bc_estring caption() const;

			void caption(core::bc_estring& p_caption);

			void caption(core::bc_estring&& p_caption);

			bool is_minimized() const noexcept(true);

			void show(bool p_visible);

			void close() noexcept(true);

			bc_messagebox_value messagebox(core::bc_estring p_caption, core::bc_estring p_text, bc_messagebox_type p_type, bc_messagebox_buttom p_button);

			platform_pack& get_platform_pack()
			{
				return m_platform_pack;
			}

		protected:

		private:
			platform_pack m_platform_pack;
		};

		using bc_render_window = bc_render_window_proxy< core_platform::g_current_platform >;
	}
}