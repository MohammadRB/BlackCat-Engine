// [27/2/2015 MRB]

#pragma once

#include "Platform/PlatformPCH.h"
#include "Platform/Application/bcWindow.h"

namespace black_cat::platform
{
	template<bc_platform TPlatform>
	class bc_platform_application;

	enum class bc_messagebox_type
	{
		none,
		error,
		warning,
		information
	};

	enum class bc_messagebox_button
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

	enum class bc_window_state
	{
		normal,
		minimized,
		maximized
	};

	template<bc_platform>
	struct bc_platform_basic_window_parameter_pack
	{
	public:
		bc_platform_basic_window_parameter_pack(const bcECHAR* p_caption, bcUINT p_width, bcUINT p_height)
			: m_caption(p_caption),
			  m_width(p_width),
			  m_height(p_height)
		{
		}

		const bcECHAR* m_caption;
		bcUINT m_width;
		bcUINT m_height;
	};

	template<bc_platform TPlatform>
	struct bc_platform_basic_window_pack : bc_platform_window_pack<TPlatform>
	{
	};

	template<bc_platform TPlatform>
	class bc_platform_basic_window : public bc_platform_window<TPlatform>
	{
	public:
		using parameter = bc_platform_basic_window_parameter_pack<TPlatform>;
		using platform_pack = bc_platform_basic_window_pack<TPlatform>;
		using id = bc_platform_window<TPlatform>::id;
		friend class bc_platform_application<TPlatform>;

	public:
		explicit bc_platform_basic_window(const parameter& p_parameters);
			
		bc_platform_basic_window(bc_platform_basic_window&& p_other) noexcept;

		~bc_platform_basic_window() override;

		bc_platform_basic_window& operator =(bc_platform_basic_window&& p_other) noexcept;

		id get_id() const noexcept override;

		bcUINT get_width() const noexcept;

		void set_width(bcUINT p_width) noexcept;

		bcUINT get_height() const noexcept;

		void set_height(bcUINT p_height) noexcept;

		bcUINT get_left() const noexcept;

		void set_left(bcUINT p_left) noexcept;

		bcUINT get_top() const noexcept;

		void set_top(bcUINT p_top) noexcept;

		void set_size(bcUINT p_width, bcUINT p_height) noexcept;

		void set_position(bcUINT p_left, bcUINT p_top) noexcept;

		core::bc_estring get_caption() const override;

		void set_caption(const bcECHAR* p_caption) override;

		bc_window_state get_state() const noexcept;

		void show(bool p_visible);

		void close() noexcept override;

		void update() override;

		bc_messagebox_value show_messagebox(core::bc_estring_view p_caption, core::bc_estring_view p_text, bc_messagebox_type p_type, bc_messagebox_button p_button);
			
		platform_pack& get_platform_pack() override
		{
			return m_pack;
		}

		const platform_pack& get_platform_pack() const override
		{
			return m_pack;
		}

	private:
		platform_pack m_pack;
	};

	using bc_basic_window_parameter = bc_platform_basic_window_parameter_pack<g_current_platform>;
	using bc_basic_window = bc_platform_basic_window<g_current_platform>;
}
