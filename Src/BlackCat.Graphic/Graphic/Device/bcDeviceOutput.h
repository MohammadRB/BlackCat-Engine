// [13/11/2016 MRB]

#pragma once

#include "PlatformImp/Application/bcBasicWindow.h"
#include "Graphic/bcRenderApi.h"

namespace black_cat::graphic
{
	template<bc_render_api TRenderApi>
	struct bc_platform_device_output_pack
	{
	};

	template<bc_render_api TRenderApi>
	class bc_platform_device_output
	{
	public:
		using platform_pack = bc_platform_device_output_pack<TRenderApi>;

	public:
		bc_platform_device_output() noexcept;
			
		bc_platform_device_output(platform_pack p_parameter) noexcept;

		bc_platform_device_output(bc_platform_device_output&&) noexcept;

		~bc_platform_device_output();

		bc_platform_device_output& operator=(bc_platform_device_output&&) noexcept;

		static bc_platform_device_output from_window(platform::bc_basic_window& p_render_window);

		bool is_valid() const noexcept;
			
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

	using bc_device_output = bc_platform_device_output<g_current_render_api>;
}
