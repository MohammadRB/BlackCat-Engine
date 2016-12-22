// [01/08/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"
#include "Graphic/bcRenderApi.h"
#include <d3d11.h>

namespace black_cat
{
	namespace graphic
	{
#define BC_CBUFFER_ALIGN alignas(black_cat::graphic::bc_render_api_info::required_cbuffer_align())

		template< bc_render_api TRenderApi >
		class bc_platform_render_api_info
		{
		public:
			constexpr static const bcCHAR* api_name() noexcept;

			constexpr static bool is_left_handed() noexcept;

			constexpr static bcSIZE number_of_texture2d_pixels() noexcept;

			constexpr static bcSIZE number_of_ia_vertex_buffers() noexcept;

			constexpr static bcSIZE number_of_ia_index_buffers() noexcept;

			constexpr static bcSIZE number_of_om_render_target_slots() noexcept;

			constexpr static bcSIZE number_of_rs_viewport_scissorrect() noexcept;

			constexpr static bcSIZE number_of_so_streams() noexcept;

			constexpr static bcSIZE number_of_shader_constant_buffer() noexcept;

			constexpr static bcSIZE number_of_shader_sampler() noexcept;

			constexpr static bcSIZE number_of_shader_resource() noexcept;

			constexpr static bcSIZE number_of_ps_cs_uav_resource() noexcept;

			constexpr static bcSIZE required_cbuffer_align() noexcept;

		protected:

		private:
		};

		using bc_render_api_info = bc_platform_render_api_info< g_current_render_api >;

		template< bc_render_api TRenderApi >
		constexpr const bcCHAR* bc_platform_render_api_info<TRenderApi>::api_name() noexcept
		{
			return "directx11";
		}

		template< >
		constexpr bool bc_platform_render_api_info<g_api_dx11>::is_left_handed() noexcept
		{
			return true;
		}

		template< bc_render_api TRenderApi >
		constexpr bcSIZE bc_platform_render_api_info<TRenderApi>::number_of_texture2d_pixels() noexcept
		{
			// D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
			return 16384;
		}

		template< >
		constexpr bcSIZE bc_platform_render_api_info<g_api_dx11>::number_of_ia_vertex_buffers() noexcept
		{
			// D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT
			return 32;
		}

		template< >
		constexpr bcSIZE bc_platform_render_api_info<g_api_dx11>::number_of_ia_index_buffers() noexcept
		{
			// D3D11_IA_INDEX_INPUT_RESOURCE_SLOT_COUNT
			return 1;
		}

		template< >
		constexpr bcSIZE bc_platform_render_api_info<g_api_dx11>::number_of_om_render_target_slots() noexcept
		{
			// D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT
			return 8;
		}

		template< >
		constexpr bcSIZE bc_platform_render_api_info<g_api_dx11>::number_of_rs_viewport_scissorrect() noexcept
		{
			// D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE
			return 16;
		}

		template< >
		constexpr bcSIZE bc_platform_render_api_info<g_api_dx11>::number_of_so_streams() noexcept
		{
			// D3D11_SO_STREAM_COUNT
			return 4;
		}

		template< >
		constexpr bcSIZE bc_platform_render_api_info<g_api_dx11>::number_of_shader_constant_buffer() noexcept
		{
			// D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT
			return 14;
		}

		template< >
		constexpr bcSIZE bc_platform_render_api_info<g_api_dx11>::number_of_shader_sampler() noexcept
		{
			// D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT
			return 16;
		}

		template< >
		constexpr bcSIZE bc_platform_render_api_info<g_api_dx11>::number_of_shader_resource() noexcept
		{
			// D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT
			return 16;
		}

		template< >
		constexpr bcSIZE bc_platform_render_api_info<g_api_dx11>::number_of_ps_cs_uav_resource() noexcept
		{
			// D3D11_PS_CS_UAV_REGISTER_COUNT
			return 8;
		}

		template< >
		constexpr bcSIZE bc_platform_render_api_info<g_api_dx11>::required_cbuffer_align() noexcept
		{
			return 16;
		}
	}
}
