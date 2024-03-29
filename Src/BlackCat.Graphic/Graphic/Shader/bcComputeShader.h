// [11/01/2016 MRB]

#pragma once

#include "Core/bcConstant.h"
#include "Graphic/GraphicPCH.h"
#include "Graphic/bcRenderApi.h"
#include "Graphic/bcDeviceReference.h"
#include "Graphic/bcDeviceRef.h"
#include "Graphic/bcDeviceResourceContent.h"
#include "Graphic/Shader/bcShader.h"

namespace black_cat::graphic
{
	template<bc_render_api TRenderApi>
	struct bc_platform_compute_shader_pack
	{
	};

	template<bc_render_api TRenderApi>
	class bc_platform_compute_shader : public bci_platform_shader<TRenderApi>
	{
	public:
		using platform_pack = bc_platform_compute_shader_pack<TRenderApi>;

	public:
		bc_platform_compute_shader() noexcept;

		explicit bc_platform_compute_shader(platform_pack& p_pack) noexcept;

		bc_platform_compute_shader(const bc_platform_compute_shader& p_other) noexcept;

		~bc_platform_compute_shader() override;

		bc_platform_compute_shader& operator=(const bc_platform_compute_shader& p_other) noexcept;

		bc_shader_type get_type() const override;

		bool is_valid() const noexcept override;

		void set_debug_name(const bcCHAR* p_name) noexcept override;
			
		bool operator==(const bc_platform_compute_shader& p_other) const noexcept;

		bool operator!=(const bc_platform_compute_shader& p_other) const noexcept;

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

		platform_pack& get_shader_platform_pack() noexcept override
		{
			return get_platform_pack();
		}

		const platform_pack& get_shader_platform_pack() const noexcept override
		{
			return get_platform_pack();
		}
		
	private:
		platform_pack m_pack;
	};

	constexpr bcCHAR g_compute_shader_content_name[] = BC_CONTENT_NAME(cmp_shd);
	using bc_compute_shader = bc_platform_compute_shader<g_current_render_api>;
	using bc_compute_shader_ref = bc_device_ref<bc_compute_shader>;
	using bc_compute_shader_content = bc_device_resource_content<bc_compute_shader, g_compute_shader_content_name>;
	using bc_compute_shader_content_ptr = core::bc_content_ptr<bc_compute_shader_content>;
}
