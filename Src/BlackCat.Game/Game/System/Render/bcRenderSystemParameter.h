// [06/13/2016 MRB]

#pragma once

#include "Core/Container/bcArray.h"
#include "GraphicImp/Shader/Parameter/bcConstantBufferParameter.h"
#include "GraphicImp/Shader/Parameter/bcSamplerParameter.h"
#include "GraphicImp/Shader/Parameter/bcResourceViewParameter.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "GraphicImp/Resource/View/bcDepthStencilView.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/bcRenderApiInfo.h"

namespace black_cat
{
	namespace game
	{
		constexpr bcSIZE g_render_pass_state_render_target_view_min_index = 0;
		constexpr bcSIZE g_render_pass_state_render_target_view_max_index = graphic::bc_render_api_info::number_of_om_render_target_slots() - 1;
		constexpr bcSIZE g_render_pass_state_render_target_view_count = g_render_pass_state_render_target_view_max_index - g_render_pass_state_render_target_view_min_index + 1;
		constexpr bcSIZE g_render_pass_state_sampler_min_index = 0;
		constexpr bcSIZE g_render_pass_state_sampler_max_index = 9;
		constexpr bcSIZE g_render_pass_state_sampler_count = g_render_pass_state_sampler_max_index - g_render_pass_state_sampler_min_index + 1;
		constexpr bcSIZE g_render_pass_state_shader_view_min_index = 0;
		constexpr bcSIZE g_render_pass_state_shader_view_max_index = 9;
		constexpr bcSIZE g_render_pass_state_shader_view_count = g_render_pass_state_shader_view_max_index - g_render_pass_state_shader_view_min_index + 1;
		constexpr bcSIZE g_render_pass_state_unordered_view_min_index = 0;
		constexpr bcSIZE g_render_pass_state_unordered_view_max_index = 7;
		constexpr bcSIZE g_render_pass_state_unordered_view_count = g_render_pass_state_unordered_view_max_index - g_render_pass_state_unordered_view_min_index + 1;
		constexpr bcSIZE g_render_pass_state_constant_buffer_min_index = 0;
		constexpr bcSIZE g_render_pass_state_constant_buffer_max_index = 3;
		constexpr bcSIZE g_render_pass_state_constant_buffer_count = g_render_pass_state_constant_buffer_max_index - g_render_pass_state_constant_buffer_min_index + 1;
		
		constexpr bcSIZE g_render_state_shader_view_min_index = g_render_pass_state_shader_view_max_index + 1;
		constexpr bcSIZE g_render_state_shader_view_max_index = g_render_state_shader_view_min_index + 19;
		constexpr bcSIZE g_render_state_shader_view_count = g_render_state_shader_view_max_index - g_render_state_shader_view_min_index + 1;
		constexpr bcSIZE g_render_state_constant_buffer_min_index = g_render_pass_state_constant_buffer_max_index + 1;
		constexpr bcSIZE g_render_state_constant_buffer_max_index = g_render_state_constant_buffer_min_index + 3;
		constexpr bcSIZE g_render_state_constant_buffer_count = g_render_state_constant_buffer_max_index - g_render_state_constant_buffer_min_index + 1;

		constexpr bcSIZE g_compute_state_sampler_min_index = 0;
		constexpr bcSIZE g_compute_state_sampler_max_index = 9;
		constexpr bcSIZE g_compute_state_sampler_count = g_compute_state_sampler_max_index - g_compute_state_sampler_min_index + 1;
		constexpr bcSIZE g_compute_state_shader_view_min_index = 0;
		constexpr bcSIZE g_compute_state_shader_view_max_index = 9;
		constexpr bcSIZE g_compute_state_shader_view_count = g_compute_state_shader_view_max_index - g_compute_state_shader_view_min_index + 1;
		constexpr bcSIZE g_compute_state_unordered_view_min_index = 0;
		constexpr bcSIZE g_compute_state_unordered_view_max_index = 7;
		constexpr bcSIZE g_compute_state_unordered_view_count = g_compute_state_unordered_view_max_index - g_compute_state_unordered_view_min_index + 1;
		constexpr bcSIZE g_compute_state_constant_buffer_min_index = 0;
		constexpr bcSIZE g_compute_state_constant_buffer_max_index = 7;
		constexpr bcSIZE g_compute_state_constant_buffer_count = g_compute_state_constant_buffer_max_index - g_compute_state_constant_buffer_min_index + 1;

		using bc_render_pass_state_render_target_view_array = core::bc_array
		<
			graphic::bc_render_target_view,
			g_render_pass_state_render_target_view_count
		>;
		using bc_render_pass_state_sampler_array = core::bc_array
		<
			graphic::bc_sampler_parameter,
			g_render_pass_state_sampler_count
		>;
		using bc_render_pass_state_resource_view_array = core::bc_array
		<
			graphic::bc_resource_view_parameter,
			g_render_pass_state_shader_view_count
		>;
		using bc_render_pass_state_unordered_view_array = core::bc_array
		<
			graphic::bc_resource_view_parameter,
			g_render_pass_state_unordered_view_count
		>;
		using bc_render_pass_state_constant_buffer_array = core::bc_array
		<
			graphic::bc_constant_buffer_parameter,
			g_render_pass_state_constant_buffer_count
		>;
		using bc_render_state_resource_view_array = core::bc_array
		<
			graphic::bc_resource_view_parameter,
			g_render_state_shader_view_count
		>;
		using bc_render_state_constant_buffer_array = core::bc_array
		<
			graphic::bc_constant_buffer_parameter,
			g_render_state_constant_buffer_count
		>;

		using bc_compute_state_sampler_array = core::bc_array
		<
			graphic::bc_sampler_parameter,
			g_compute_state_sampler_count
		>;
		using bc_compute_state_resource_view_array = core::bc_array
		<
			graphic::bc_resource_view_parameter,
			g_compute_state_shader_view_count
		>;
		using bc_compute_state_unordered_view_array = core::bc_array
		<
			graphic::bc_resource_view_parameter,
			g_compute_state_unordered_view_count
		>;
		using bc_compute_state_constant_buffer_array = core::bc_array
		<
			graphic::bc_constant_buffer_parameter,
			g_compute_state_constant_buffer_count
		>;
		using bc_compute_state_unordered_view_initial_count_array = core::bc_array
		<
			bcINT32,
			g_compute_state_unordered_view_count
		>;

		static_assert(g_render_pass_state_render_target_view_count <= graphic::bc_render_api_info::number_of_om_render_target_slots(), "Unsupported number of render target slot count");
		static_assert(g_render_pass_state_shader_view_count + g_render_state_shader_view_count <= graphic::bc_render_api_info::number_of_shader_resource(), "Unsupported number of shader view slot count");
		static_assert(g_render_pass_state_unordered_view_count <= graphic::bc_render_api_info::number_of_ps_cs_uav_resource(), "Unsupported number of unordered view slot count");
		static_assert(g_render_pass_state_sampler_count <= graphic::bc_render_api_info::number_of_shader_sampler(), "Unsupported number of sampler slot count");
		static_assert(g_render_pass_state_constant_buffer_count + g_render_state_constant_buffer_count <= graphic::bc_render_api_info::number_of_shader_constant_buffer(), "Unsupported number of constant buffer slot count");

		static_assert(g_compute_state_sampler_count <= graphic::bc_render_api_info::number_of_shader_sampler(), "Unsupported number of sampler slot count");
		static_assert(g_compute_state_shader_view_count <= graphic::bc_render_api_info::number_of_shader_resource(), "Unsupported number of shader view slot count");
		static_assert(g_compute_state_unordered_view_count <= graphic::bc_render_api_info::number_of_ps_cs_uav_resource(), "Unsupported number of unordered view slot count");
		static_assert(g_compute_state_constant_buffer_count <= graphic::bc_render_api_info::number_of_shader_constant_buffer(), "Unsupported number of constant buffer slot count");
	}
}