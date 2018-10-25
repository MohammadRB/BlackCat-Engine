// [01/08/2016 MRB]

#pragma once

#include "Graphic/GraphicPCH.h"

namespace black_cat
{
	namespace graphic
	{
		enum class bc_pipeline_stage
		{
			input_assembler_stage = 1,
			vertex_stage = 2,
			hull_stage = 4,
			domain_stage = 8,
			geometry_stage = 16,
			pixel_stage = 32,
			compute_stage = 64,
			stream_output_stage = 128,
			rasterizer_stage = 256,
			output_merger_stage = 512,
		};

		enum class bc_comparison_func
		{
			never = 1,
			less = 2,
			equal = 3,
			less_equal = 4,
			greater = 5,
			not_equal = 6,
			greater_equal = 7,
			always = 8
		};

		enum class bc_primitive
		{
			undefined = 0,
			pointlist = 1,
			linelist = 2,
			linestrip = 3,
			trianglelist = 4,
			trianglestrip = 5,
			linelist_adj = 10,
			linestrip_adj = 11,
			trianglelist_adj = 12,
			trianglestrip_adj = 13,
			control_point_1_patchlist = 33,
			control_point_2_patchlist = 34,
			control_point_3_patchlist = 35,
			control_point_4_patchlist = 36,
		};

		enum class bc_color_write
		{
			red = 1,
			green = 2,
			blue = 4,
			alpha = 8,
			all = (((red | green) | blue) | alpha)
		};

		// -- Resources --------------------------------------------------------------------------------

		struct bc_texture_ms_config
		{
		public:
			bc_texture_ms_config(bcUINT p_count, bcUINT p_quality)
				: m_count(p_count), m_quality(p_quality)
			{
			}

			bcUINT m_count, m_quality;
		};

		enum class bc_resource_type
		{
			buffer,
			texture_1d,
			texture_2d,
			texture_3d,
			texture_cube
		};

		enum class bc_resource_view_type
		{
			none = 0,
			shader = 1,
			unordered = 2,
			render_target = 4,
			depth_stencil = 8
		};

		enum class bc_resource_eviction_priority : bcUINT
		{
			minimum = 0x28000000,
			low = 0x50000000,
			normal = 0x78000000,
			high = 0xa0000000,
			maximum = 0xc8000000,
		};

		enum class bc_resource_usage
		{
			gpu_rw = 0,
			gpu_r = 1,
			gpu_r_cpu_w = 2,
			gpu_r_cpu_r = 3
		};

		/*enum class bc_resource_bind
		{
			vertex_buffer = 0x1L,
			index_buffer = 0x2L,
			constant_buffer = 0x4L,
			shader_resource = 0x8L,
			stream_output = 0x10L,
			render_target = 0x20L,
			depth_stencil = 0x40L,
			unordered_access = 0x80L
		};*/

		enum class bc_resource_map
		{
			read = 1,
			write = 2,
			read_write = 3,
			write_discard = 4,
			write_no_overwrite = 5
		};

		enum class bc_format
		{
			unknown = 0,
			R32G32B32A32_TYPELESS = 1,
			R32G32B32A32_FLOAT = 2,
			R32G32B32A32_UINT = 3,
			R32G32B32A32_SINT = 4,
			R32G32B32_TYPELESS = 5,
			R32G32B32_FLOAT = 6,
			R32G32B32_UINT = 7,
			R32G32B32_SINT = 8,
			R16G16B16A16_TYPELESS = 9,
			R16G16B16A16_FLOAT = 10,
			R16G16B16A16_UNORM = 11,
			R16G16B16A16_UINT = 12,
			R16G16B16A16_SNORM = 13,
			R16G16B16A16_SINT = 14,
			R32G32_TYPELESS = 15,
			R32G32_FLOAT = 16,
			R32G32_UINT = 17,
			R32G32_SINT = 18,
			R32G8X24_TYPELESS = 19,
			D32_FLOAT_S8X24_UINT = 20,
			R32_FLOAT_X8X24_TYPELESS = 21,
			X32_TYPELESS_G8X24_UINT = 22,
			R10G10B10A2_TYPELESS = 23,
			R10G10B10A2_UNORM = 24,
			R10G10B10A2_UINT = 25,
			R11G11B10_FLOAT = 26,
			R8G8B8A8_TYPELESS = 27,
			R8G8B8A8_UNORM = 28,
			R8G8B8A8_UNORM_SRGB = 29,
			R8G8B8A8_UINT = 30,
			R8G8B8A8_SNORM = 31,
			R8G8B8A8_SINT = 32,
			R16G16_TYPELESS = 33,
			R16G16_FLOAT = 34,
			R16G16_UNORM = 35,
			R16G16_UINT = 36,
			R16G16_SNORM = 37,
			R16G16_SINT = 38,
			R32_TYPELESS = 39,
			D32_FLOAT = 40,
			R32_FLOAT = 41,
			R32_UINT = 42,
			R32_SINT = 43,
			R24G8_TYPELESS = 44,
			D24_UNORM_S8_UINT = 45,
			R24_UNORM_X8_TYPELESS = 46,
			X24_TYPELESS_G8_UINT = 47,
			R8G8_TYPELESS = 48,
			R8G8_UNORM = 49,
			R8G8_UINT = 50,
			R8G8_SNORM = 51,
			R8G8_SINT = 52,
			R16_TYPELESS = 53,
			R16_FLOAT = 54,
			D16_UNORM = 55,
			R16_UNORM = 56,
			R16_UINT = 57,
			R16_SNORM = 58,
			R16_SINT = 59,
			R8_TYPELESS = 60,
			R8_UNORM = 61,
			R8_UINT = 62,
			R8_SNORM = 63,
			R8_SINT = 64,
			A8_UNORM = 65,
			R1_UNORM = 66,
			R9G9B9E5_SHAREDEXP = 67,
			R8G8_B8G8_UNORM = 68,
			G8R8_G8B8_UNORM = 69,
			BC1_TYPELESS = 70,
			BC1_UNORM = 71,
			BC1_UNORM_SRGB = 72,
			BC2_TYPELESS = 73,
			BC2_UNORM = 74,
			BC2_UNORM_SRGB = 75,
			BC3_TYPELESS = 76,
			BC3_UNORM = 77,
			BC3_UNORM_SRGB = 78,
			BC4_TYPELESS = 79,
			BC4_UNORM = 80,
			BC4_SNORM = 81,
			BC5_TYPELESS = 82,
			BC5_UNORM = 83,
			BC5_SNORM = 84,
			B5G6R5_UNORM = 85,
			B5G5R5A1_UNORM = 86,
			B8G8R8A8_UNORM = 87,
			B8G8R8X8_UNORM = 88,
			R10G10B10_XR_BIAS_A2_UNORM = 89,
			B8G8R8A8_TYPELESS = 90,
			B8G8R8A8_UNORM_SRGB = 91,
			B8G8R8X8_TYPELESS = 92,
			B8G8R8X8_UNORM_SRGB = 93,
			BC6H_TYPELESS = 94,
			BC6H_UF16 = 95,
			BC6H_SF16 = 96,
			BC7_TYPELESS = 97,
			BC7_UNORM = 98,
			BC7_UNORM_SRGB = 99,
			//AYUV = 100,
			//Y410 = 101,
			//Y416 = 102,
			//NV12 = 103,
			//P010 = 104,
			//P016 = 105,
			//420_OPAQUE = 106,
			//YUY2 = 107,
			//Y210 = 108,
			//Y216 = 109,
			//NV11 = 110,
			//AI44 = 111,
			//IA44 = 112,
			//P8 = 113,
			//A8P8 = 114,
			//B4G4R4A4_UNORM = 115,
			//FORCE_UINT = 0xffffffff
		};

		// -- Samplers --------------------------------------------------------------------------------

		enum class bc_filter
		{
			min_mag_mip_point = 0,
			min_mag_point_mip_linear = 0x1,
			min_point_mag_linear_mip_point = 0x4,
			min_point_mag_mip_linear = 0x5,
			min_linear_mag_mip_point = 0x10,
			min_linear_mag_point_mip_linear = 0x11,
			min_mag_linear_mip_point = 0x14,
			min_mag_mip_linear = 0x15,
			anisotropic = 0x55,
			comparison_min_mag_mip_point = 0x80,
			comparison_min_mag_point_mip_linear = 0x81,
			comparison_min_point_mag_linear_mip_point = 0x84,
			comparison_min_point_mag_mip_linear = 0x85,
			comparison_min_linear_mag_mip_point = 0x90,
			comparison_min_linear_mag_point_mip_linear = 0x91,
			comparison_min_mag_linear_mip_point = 0x94,
			comparison_min_mag_mip_linear = 0x95,
			comparison_anisotropic = 0xd5
			/*minimum_min_mag_mip_point = 0x100,
			minimum_min_mag_point_mip_linear = 0x101,
			minimum_min_point_mag_linear_mip_point = 0x104,
			minimum_min_point_mag_mip_linear = 0x105,
			minimum_min_linear_mag_mip_point = 0x110,
			minimum_min_linear_mag_point_mip_linear = 0x111,
			minimum_min_mag_linear_mip_point = 0x114,
			minimum_min_mag_mip_linear = 0x115,
			minimum_anisotropic = 0x155,
			maximum_min_mag_mip_point = 0x180,
			maximum_min_mag_point_mip_linear = 0x181,
			maximum_min_point_mag_linear_mip_point = 0x184,
			maximum_min_point_mag_mip_linear = 0x185,
			maximum_min_linear_mag_mip_point = 0x190,
			maximum_min_linear_mag_point_mip_linear = 0x191,
			maximum_min_mag_linear_mip_point = 0x194,
			maximum_min_mag_mip_linear = 0x195,
			maximum_anisotropic = 0x1d5*/
		};

		enum class bc_texture_address_mode
		{
			wrap = 1,
			mirror = 2,
			clamp = 3,
			border = 4,
			mirror_once = 5
		};

		enum class bc_image_format
		{
			dds,
			bmp,
			png,
			gif,
			tiff,
			jpg
		};

		// -- Blend --------------------------------------------------------------------------------

		enum class bc_blend
		{
			zero = 1,
			one = 2,
			src_color = 3,
			inv_src_color = 4,
			src_alpha = 5,
			inv_src_alpha = 6,
			dest_alpha = 7,
			inv_dest_alpha = 8,
			dest_color = 9,
			inv_dest_color = 10,
			src_alpha_sat = 11,
			blend_factor = 14,
			inv_blend_factor = 15,
			src1_color = 16,
			inv_src1_color = 17,
			src1_ahpha = 18,
			inv_src1_alpha = 19
		};

		enum class bc_blend_op
		{
			add = 1,
			subtract = 2,
			rev_subtract = 3,
			min = 4,
			max = 5
		};

		// -- DepthStencil --------------------------------------------------------------------------------

		enum class bc_depth_write_mask
		{
			zero = 0,
			all = 1
		};

		enum class bc_stencil_op
		{
			keep = 1,
			zero = 2,
			replace = 3,
			incr_sat = 4,
			decr_sat = 5,
			invert = 6,
			incr = 7,
			decr = 8
		};

		// -- Rasterizer --------------------------------------------------------------------------------

		enum class bc_fill_mode
		{
			wireframe = 2,
			solid = 3
		};

		enum class bc_cull_mode
		{
			none = 1,
			front = 2,
			back = 3
		};

		// -- InputLayout --------------------------------------------------------------------------------

		enum class bc_input_classification
		{
			per_vertex_data = 0,
			per_instance_data = 1
		};
	}
}