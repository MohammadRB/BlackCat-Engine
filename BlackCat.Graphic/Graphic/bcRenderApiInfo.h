// [01/08/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcPlatformRenderApi.h"

namespace black_cat
{
	namespace graphic
	{
		template< bc_platform_render_api TRenderApi >
		class bc_platform_render_api_info
		{
		public:
			constexpr static bcUINT number_of_ia_vertex_buffers() noexcept;

			constexpr static bcUINT number_of_ia_index_buffers() noexcept;

			constexpr static bcUINT number_of_om_render_target_slots() noexcept;

			constexpr static bcUINT number_of_ps_cs_uav_registers() noexcept;

			constexpr static bcUINT32 number_of_rs_viewport_scissorrect() noexcept;

			constexpr static bcUINT32 number_of_so_streams() noexcept;

			constexpr static bcUINT32 number_of_shader_constant_buffer() noexcept;

			constexpr static bcUINT32 number_of_shader_sampler() noexcept;

			constexpr static bcUINT32 number_of_shader_resource() noexcept;

		protected:

		private:
		};

		using bc_render_api_info = bc_platform_render_api_info< g_current_platform_render_api >;

		template< >
		constexpr bcUINT bc_platform_render_api_info<g_api_dx11>::number_of_ia_vertex_buffers() noexcept
		{
			// D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT
			return 32;
		}

		template< >
		constexpr bcUINT bc_platform_render_api_info<g_api_dx11>::number_of_ia_index_buffers() noexcept
		{
			// D3D11_IA_INDEX_INPUT_RESOURCE_SLOT_COUNT
			return 1;
		}

		template< >
		constexpr bcUINT bc_platform_render_api_info<g_api_dx11>::number_of_om_render_target_slots() noexcept
		{
			// D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT
			return 8;
		}

		template< >
		constexpr bcUINT bc_platform_render_api_info<g_api_dx11>::number_of_ps_cs_uav_registers() noexcept
		{
			// D3D11_PS_CS_UAV_REGISTER_COUNT
			return 8;
		}

		template< >
		constexpr bcUINT bc_platform_render_api_info<g_api_dx11>::number_of_rs_viewport_scissorrect() noexcept
		{
			// D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE
			return 16;
		}

		template< >
		constexpr bcUINT bc_platform_render_api_info<g_api_dx11>::number_of_so_streams() noexcept
		{
			// D3D11_SO_STREAM_COUNT
			return 4;
		}

		template< >
		constexpr bcUINT bc_platform_render_api_info<g_api_dx11>::number_of_shader_constant_buffer() noexcept
		{
			// D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT
			return 14;
		}

		template< >
		constexpr bcUINT bc_platform_render_api_info<g_api_dx11>::number_of_shader_sampler() noexcept
		{
			// D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT
			return 16;
		}

		template< >
		constexpr bcUINT bc_platform_render_api_info<g_api_dx11>::number_of_shader_resource() noexcept
		{
			// D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT
			return 16;
		}
	}
}