// [2/27/2015 MRB]

#pragma once

#include "Platform/PlatformPCH.h"
#include "Platform/Application/bcWindow.h"

namespace black_cat
{
	namespace platform
	{
		template<platform::bc_platform TPlatform>
		class bc_platform_application;

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

		template<platform::bc_platform>
		struct bc_platform_basic_window_parameter_pack
		{
		public:
			bc_platform_basic_window_parameter_pack(const bcECHAR* p_caption, bcUINT32 p_width, bcUINT32 p_height)
				: m_caption(p_caption),
				m_width(p_width),
				m_height(p_height)
			{
			}

			const bcECHAR* m_caption;
			bcUINT32 m_width;
			bcUINT32 m_height;
		};

		using bc_basic_window_parameter = bc_platform_basic_window_parameter_pack<platform::g_current_platform>;

		template<platform::bc_platform>
		struct bc_platform_basic_window_pack
		{
		};

		template<platform::bc_platform TPlatform>
		class bc_platform_basic_window : public bc_platform_window<TPlatform>
		{
		public:
			using platform_pack = bc_platform_basic_window_pack<TPlatform>;
			using parameter = bc_basic_window_parameter;
			using bc_platform_window<TPlatform>::id;
			friend class bc_platform_application<TPlatform>;

		public:
			explicit bc_platform_basic_window(const bc_basic_window_parameter& p_parameters);
			
			bc_platform_basic_window(bc_platform_basic_window&& p_other) noexcept;

			~bc_platform_basic_window();

			bc_platform_basic_window& operator =(bc_platform_basic_window&& p_other) noexcept;

			id get_id() const noexcept override;

			bcUINT32 get_width() const noexcept;

			void set_width(bcUINT32 p_width) noexcept;

			bcUINT32 get_height() const noexcept;

			void set_height(bcUINT32 p_height) noexcept;

			bcUINT32 get_left() const noexcept;

			void set_left(bcUINT32 p_left) noexcept;

			bcUINT32 get_top() const noexcept;

			void set_top(bcUINT32 p_top) noexcept;

			void set_size(bcUINT32 p_width, bcUINT32 p_height) noexcept;

			void set_position(bcUINT32 p_left, bcUINT32 p_top) noexcept;

			core::bc_estring get_caption() const override;

			void set_caption(const bcECHAR* p_caption) override;

			bool is_minimized() const noexcept;

			void show(bool p_visible);

			void close() noexcept override;

			void update() override;

			bc_messagebox_value messagebox(const bcECHAR* p_caption, const bcECHAR* p_text, bc_messagebox_type p_type, bc_messagebox_buttom p_button);

			bc_messagebox_value messagebox(core::bc_estring_frame p_caption, core::bc_estring_frame p_text, bc_messagebox_type p_type, bc_messagebox_buttom p_button);

			platform_pack& get_platform_pack()
			{
				return m_pack;
			}

		private:
			platform_pack m_pack;
		};

		using bc_basic_window = bc_platform_basic_window<platform::g_current_platform>;
	}
}