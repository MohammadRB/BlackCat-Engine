// [10/01/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcRenderApi.h"
#include "Graphic/bcDeviceReference.h"
#include "Graphic/bcDeviceRef.h"

namespace black_cat::graphic
{
	enum class bc_shader_type : bcBYTE
	{
		vertex = 1,
		hull = 2,
		domain = 4,
		geometry = 8,
		pixel = 16,
		compute = 32
	};

	struct bc_shader_macro
	{
		const bcCHAR* m_name;
		const bcCHAR* m_value;
	};
		
	template<bc_render_api TRenderApi>
	struct bc_platform_compiled_shader_pack
	{
	};

	template<bc_render_api TRenderApi>
	class bc_platform_compiled_shader : public bc_platform_device_reference<TRenderApi>
	{
	public:
		using platform_pack = bc_platform_compiled_shader_pack<TRenderApi>;

	public:
		bc_platform_compiled_shader() noexcept;

		explicit bc_platform_compiled_shader(platform_pack& p_pack) noexcept;

		bc_platform_compiled_shader(const bc_platform_compiled_shader& p_other) noexcept;

		~bc_platform_compiled_shader() override;

		bc_platform_compiled_shader& operator=(const bc_platform_compiled_shader& p_other) noexcept;

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

	using bc_compiled_shader = bc_platform_compiled_shader<g_current_render_api>;
	using bc_compiled_shader_ptr = bc_device_ref<bc_compiled_shader>;

	template<bc_render_api TRenderApi>
	struct bci_platform_shader_pack
	{
	};

	template<bc_render_api TRenderApi>
	class bci_platform_shader : public bc_platform_device_reference<TRenderApi>
	{
	public:
		using platform_pack = bci_platform_shader_pack<TRenderApi>;

	public:
		virtual ~bci_platform_shader() override;

		virtual bc_shader_type get_type() const = 0;

		virtual platform_pack& get_shader_platform_pack() noexcept = 0;

		virtual const platform_pack& get_shader_platform_pack() const noexcept = 0;

	protected:
		bci_platform_shader() noexcept;

		explicit bci_platform_shader(platform_pack& p_pack) noexcept;

		bci_platform_shader(const bci_platform_shader& p_other) noexcept;

		bci_platform_shader& operator=(const bci_platform_shader& p_other) noexcept;
	};

	using bci_shader = bci_platform_shader<g_current_render_api>;
}
