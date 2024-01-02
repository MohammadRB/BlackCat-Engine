// [08/01/2016 MRB]

#pragma once

#define NOMINMAX

#include "CorePlatform/bcType.h"
#include "Graphic/bcRenderApi.h"

#include <d3d11.h>

namespace black_cat::graphic
{
	template<bc_render_api TRenderApi>
	class bc_platform_render_api_info
	{
	public:
		constexpr static const bcCHAR* api_name() noexcept;

		constexpr static bool use_left_handed() noexcept;

		constexpr static bool use_top_left_texcoord() noexcept;

		constexpr static bool use_column_matrix() noexcept;

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

		constexpr static bcSIZE max_tessellation_factor() noexcept;
	};

	using bc_render_api_info = bc_platform_render_api_info<g_current_render_api>;

#define BC_CBUFFER_ALIGN alignas(black_cat::graphic::bc_render_api_info::required_cbuffer_align())

	template<>
	constexpr const bcCHAR* bc_platform_render_api_info<g_api_dx11>::api_name() noexcept
	{
		return "directx11";
	}

	template<>
	constexpr bool bc_platform_render_api_info<g_api_dx11>::use_left_handed() noexcept
	{
		return true;
	}

	template<>
	constexpr bool bc_platform_render_api_info<g_api_dx11>::use_top_left_texcoord() noexcept
	{
		return true;
	}

	template<>
	constexpr bool bc_platform_render_api_info<g_api_dx11>::use_column_matrix() noexcept
	{
		return true;
	}

	template<>
	constexpr bcSIZE bc_platform_render_api_info<g_api_dx11>::number_of_texture2d_pixels() noexcept
	{
		return D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
	}

	template<>
	constexpr bcSIZE bc_platform_render_api_info<g_api_dx11>::number_of_ia_vertex_buffers() noexcept
	{
		return D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT;
	}

	template<>
	constexpr bcSIZE bc_platform_render_api_info<g_api_dx11>::number_of_ia_index_buffers() noexcept
	{
		return D3D11_IA_INDEX_INPUT_RESOURCE_SLOT_COUNT;
	}

	template<>
	constexpr bcSIZE bc_platform_render_api_info<g_api_dx11>::number_of_om_render_target_slots() noexcept
	{
		return D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT;
	}

	template<>
	constexpr bcSIZE bc_platform_render_api_info<g_api_dx11>::number_of_rs_viewport_scissorrect() noexcept
	{
		return D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	}

	template<>
	constexpr bcSIZE bc_platform_render_api_info<g_api_dx11>::number_of_so_streams() noexcept
	{
		return D3D11_SO_STREAM_COUNT;
	}

	template<>
	constexpr bcSIZE bc_platform_render_api_info<g_api_dx11>::number_of_shader_constant_buffer() noexcept
	{
		return D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT;
	}

	template<>
	constexpr bcSIZE bc_platform_render_api_info<g_api_dx11>::number_of_shader_sampler() noexcept
	{
		return D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT;
	}

	template<>
	constexpr bcSIZE bc_platform_render_api_info<g_api_dx11>::number_of_shader_resource() noexcept
	{
		return D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT;
	}

	template<>
	constexpr bcSIZE bc_platform_render_api_info<g_api_dx11>::number_of_ps_cs_uav_resource() noexcept
	{
		return D3D11_PS_CS_UAV_REGISTER_COUNT;
	}

	template<>
	constexpr bcSIZE bc_platform_render_api_info<g_api_dx11>::required_cbuffer_align() noexcept
	{
		return D3D11_COMMONSHADER_CONSTANT_BUFFER_PARTIAL_UPDATE_EXTENTS_BYTE_ALIGNMENT;
	}

	template<>
	constexpr bcSIZE bc_platform_render_api_info<g_api_dx11>::max_tessellation_factor() noexcept
	{
		return D3D11_TESSELLATOR_MAX_TESSELLATION_FACTOR;
	}
}
