// [02/12/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "Core/Utility/bcEnumOperand.h"
#include "GraphicImp/Resource/Buffer/bcBufferConfig.h"
#include "GraphicImp/Resource/Texture/bcTextureConfig.h"
#include "GraphicImp/Resource/View/bcResourceViewConfig.h"
#include "GraphicImp/Resource/bcResourceConfig.h"

namespace black_cat
{
	namespace graphic
	{
		// -- Buffer configuration modifier ---------------------------------------------------------------------------
		
		bc_buffer_configure_modifier::bc_buffer_configure_modifier(bc_buffer_config p_config) noexcept
			: m_config(p_config)
		{
		}
		
		bc_buffer_config bc_buffer_configure_modifier::as_vertex_buffer() noexcept
		{
			bool l_is_stream_output = m_config.get_platform_pack().m_desc.BindFlags & 
				static_cast<bcUINT>(D3D11_BIND_STREAM_OUTPUT) == static_cast<bcUINT>(D3D11_BIND_STREAM_OUTPUT);
			
			m_config.get_platform_pack().m_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER |
				(l_is_stream_output ? D3D11_BIND_STREAM_OUTPUT : 0);

			return m_config;
		}
				
		bc_buffer_config bc_buffer_configure_modifier::as_index_buffer() noexcept
		{
			m_config.get_platform_pack().m_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

			return m_config;
		}
				
		bc_buffer_config bc_buffer_configure_modifier::as_constant_buffer() noexcept
		{
			m_config.get_platform_pack().m_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

			return m_config;
		}
				
		bc_buffer_config bc_buffer_configure_modifier::as_structured_buffer(bcUINT p_element_size) noexcept
		{
			m_config.get_platform_pack().m_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			m_config.get_platform_pack().m_desc.StructureByteStride = p_element_size;

			return m_config;
		}
				
		bc_buffer_config bc_buffer_configure_modifier::as_append_consume(bcUINT p_element_size) noexcept
		{
			m_config.get_platform_pack().m_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			m_config.get_platform_pack().m_desc.StructureByteStride = p_element_size;

			return m_config;
		}
				
		bc_buffer_config bc_buffer_configure_modifier::as_byte_address_buffer() noexcept
		{
			m_config.get_platform_pack().m_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

			return m_config;
		}

		bc_buffer_config bc_buffer_configure_modifier::as_indirect_args_buffer() noexcept
		{
			m_config.get_platform_pack().m_desc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;

			return m_config;
		}

		// -- Texture configuration modifier ---------------------------------------------------------------------------

		bc_texture_configure_modifier::bc_texture_configure_modifier(bc_texture_config p_config) noexcept: m_config(p_config)
		{
		}

		bc_texture_config bc_texture_configure_modifier::as_normal_texture() noexcept
		{
			return m_config;
		}

		bc_texture_config bc_texture_configure_modifier::as_depth_stencil_texture() noexcept
		{
			DXGI_FORMAT l_new_format;

			switch (m_config.get_platform_pack().m_desc.Format)
			{
			case DXGI_FORMAT_R16_TYPELESS:
			case DXGI_FORMAT_D16_UNORM:
				l_new_format = DXGI_FORMAT_R16_TYPELESS;
				break;
			case DXGI_FORMAT_R24G8_TYPELESS:
			case DXGI_FORMAT_D24_UNORM_S8_UINT:
				l_new_format = DXGI_FORMAT_R24G8_TYPELESS;
				break;
			case DXGI_FORMAT_R32_TYPELESS:
			case DXGI_FORMAT_D32_FLOAT:
				l_new_format = DXGI_FORMAT_R32_TYPELESS;
				break;
			default:
				bcAssert(false);
				break;
			}

			m_config.get_platform_pack().m_desc.Format = l_new_format;

			return m_config;
		}
				
		bc_texture_config bc_texture_configure_modifier::as_render_target_texture() noexcept
		{
			return m_config;
		}

		// -- Buffer view configuration modifier -----------------------------------------------------------------------
	
		bc_buffer_view_configure_modifier::bc_buffer_view_configure_modifier(bc_resource_view_config p_config) noexcept: m_config(p_config)
		{
		}
		
		bc_resource_view_config bc_buffer_view_configure_modifier::as_structured_buffer() noexcept
		{
			if(m_config.get_platform_pack().m_type == bc_resource_view_type::unordered)
			{
				m_config.get_platform_pack().m_unordered_shader_view_desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
			}

			return m_config;
		}
				
		bc_resource_view_config bc_buffer_view_configure_modifier::as_append_consume() noexcept
		{
			if (m_config.get_platform_pack().m_type == bc_resource_view_type::unordered)
			{
				m_config.get_platform_pack().m_unordered_shader_view_desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND | D3D11_BUFFER_UAV_FLAG_COUNTER;
			}

			return m_config;
		}
				
		bc_resource_view_config bc_buffer_view_configure_modifier::as_byte_address_buffer() noexcept
		{
			if (m_config.get_platform_pack().m_type == bc_resource_view_type::unordered)
			{
				m_config.get_platform_pack().m_unordered_shader_view_desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
			}

			return m_config;
		}
				
		bc_resource_view_config bc_buffer_view_configure_modifier::as_indirect_args_buffer() noexcept
		{
			return m_config;
		}

		// -- Resource configuration -----------------------------------------------------------------------------------

		bc_buffer_configure_modifier bc_resource_configure::as_buffer(bcUINT p_num_element,
			bcUINT p_element_size,
			bc_resource_usage p_usage,
			bc_resource_view_type p_view_types,
			bool p_as_stream_output) noexcept
		{
			bc_buffer_config l_buffer_config;
			D3D11_BUFFER_DESC& l_dxbuffer_desc = l_buffer_config.get_platform_pack().m_desc;

			std::memset(&l_dxbuffer_desc, 0, sizeof(D3D11_BUFFER_DESC));

			bool l_has_shader_view = core::bc_enum::has(p_view_types, bc_resource_view_type::shader);
			bool l_has_unordered_view = core::bc_enum::has(p_view_types, bc_resource_view_type::unordered);

			l_dxbuffer_desc.ByteWidth = p_num_element * p_element_size;
			l_dxbuffer_desc.MiscFlags = 0;
			l_dxbuffer_desc.StructureByteStride = p_element_size;
			l_dxbuffer_desc.BindFlags =
				(l_has_shader_view ? D3D11_BIND_SHADER_RESOURCE : 0) |
				(l_has_unordered_view ? D3D11_BIND_UNORDERED_ACCESS : 0) |
				(p_as_stream_output ? D3D11_BIND_STREAM_OUTPUT : 0);
			l_dxbuffer_desc.Usage = bc_graphic_cast(p_usage);
			l_dxbuffer_desc.CPUAccessFlags =
				p_usage == bc_resource_usage::gpu_r_cpu_w ?
					D3D11_CPU_ACCESS_WRITE :
					p_usage == bc_resource_usage::gpu_r_cpu_r ?
						D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE :
						0;

			return bc_buffer_configure_modifier(l_buffer_config);
		}
				
		bc_texture_configure_modifier bc_resource_configure::as_texture2d(bcUINT p_width, 
			bcUINT p_height, 
			bool p_mip_generation, 
			bcUINT p_mip_levels, 
			bc_format p_format, 
			bc_resource_usage p_usage, 
			bc_resource_view_type p_view_types, 
			bcUINT p_sample_count, 
			bcUINT p_sample_quality, 
			bcUINT p_array_size) noexcept
		{
			bc_texture_config l_texture_config;
			D3D11_TEXTURE2D_DESC& l_dxtexture_desc = l_texture_config.get_platform_pack().m_desc;

			bool l_has_shader_view = (static_cast< bcINT >(p_view_types) &
				static_cast< bcINT >(bc_resource_view_type::shader)) ==
				static_cast< bcINT >(bc_resource_view_type::shader);
			bool l_has_unordered_view = (static_cast< bcINT >(p_view_types) &
				static_cast< bcINT >(bc_resource_view_type::unordered)) ==
				static_cast< bcINT >(bc_resource_view_type::unordered);
			bool l_has_render_target_view = (static_cast< bcINT >(p_view_types) &
				static_cast< bcINT >(bc_resource_view_type::render_target)) ==
				static_cast< bcINT >(bc_resource_view_type::render_target);
			bool l_has_depth_stencil_view = (static_cast< bcINT >(p_view_types) &
				static_cast< bcINT >(bc_resource_view_type::depth_stencil)) ==
				static_cast< bcINT >(bc_resource_view_type::depth_stencil);

			l_dxtexture_desc.Width = p_width;
			l_dxtexture_desc.Height = p_height;
			l_dxtexture_desc.MipLevels = p_mip_levels;
			l_dxtexture_desc.ArraySize = p_array_size;
			l_dxtexture_desc.Format = bc_graphic_cast(p_format);
			l_dxtexture_desc.SampleDesc.Count = p_sample_count;
			l_dxtexture_desc.SampleDesc.Quality = p_sample_quality;
			l_dxtexture_desc.Usage = bc_graphic_cast(p_usage);
			l_dxtexture_desc.BindFlags =
				(l_has_shader_view ? D3D11_BIND_SHADER_RESOURCE : 0) |
				(l_has_unordered_view ? D3D11_BIND_UNORDERED_ACCESS : 0) |
				(l_has_render_target_view ? D3D11_BIND_RENDER_TARGET : 0) |
				(l_has_depth_stencil_view ? D3D11_BIND_DEPTH_STENCIL : 0);
			l_dxtexture_desc.CPUAccessFlags =
				p_usage == bc_resource_usage::gpu_r_cpu_w ?
					D3D11_CPU_ACCESS_WRITE :
					p_usage == bc_resource_usage::gpu_r_cpu_r ?
						D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE :
						0;
			l_dxtexture_desc.MiscFlags = p_mip_generation && (p_mip_levels == 0 || p_mip_levels > 1) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

			return bc_texture_configure_modifier(l_texture_config);
		}

		// -- Buffer view configuration --------------------------------------------------------------------------------

		bc_buffer_view_configure::bc_buffer_view_configure(bc_format p_format) noexcept: m_format(p_format)
		{
		}

		bc_buffer_view_configure_modifier bc_buffer_view_configure::as_shader_view(bcUINT p_first_element, bcUINT p_num_elements) noexcept
		{
			bc_resource_view_config l_resource_view_config;
			D3D11_SHADER_RESOURCE_VIEW_DESC& l_srv_desc = l_resource_view_config.get_platform_pack().m_shader_view_desc;
			
			l_resource_view_config.get_platform_pack().m_type = bc_resource_view_type::shader;

			std::memset(&l_srv_desc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

			l_srv_desc.Format = bc_graphic_cast(m_format);
			l_srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
			l_srv_desc.Buffer.FirstElement = p_first_element;
			l_srv_desc.Buffer.NumElements = p_num_elements;

			return bc_buffer_view_configure_modifier(l_resource_view_config);
		}
				
		bc_buffer_view_configure_modifier bc_buffer_view_configure::as_unordered_view(bcUINT p_first_element, bcUINT p_num_elements) noexcept
		{
			bc_resource_view_config l_resource_view_config;
			D3D11_UNORDERED_ACCESS_VIEW_DESC& l_uav_desc = l_resource_view_config.get_platform_pack().m_unordered_shader_view_desc;
			
			l_resource_view_config.get_platform_pack().m_type = bc_resource_view_type::unordered;
			
			std::memset(&l_uav_desc, 0, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));

			l_uav_desc.Format = bc_graphic_cast(m_format);
			l_uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
			l_uav_desc.Buffer.FirstElement = p_first_element;
			l_uav_desc.Buffer.NumElements = p_num_elements;

			return bc_buffer_view_configure_modifier(l_resource_view_config);
		}

		// -- Texture view configuration --------------------------------------------------------------------------------

		bc_texture_view_configure_modifier::bc_texture_view_configure_modifier(bc_resource_view_config p_config) noexcept
			: m_config(p_config)
		{
		}

		bc_resource_view_config bc_texture_view_configure_modifier::on_texture2d() noexcept
		{
			return m_config;
		}

		bc_resource_view_config bc_texture_view_configure_modifier::on_depth_stencil_texture2d() noexcept
		{
			DXGI_FORMAT l_format = m_config.get_platform_pack().m_type == bc_resource_view_type::shader ?
				                       m_config.get_platform_pack().m_shader_view_desc.Format :
				                       m_config.get_platform_pack().m_unordered_shader_view_desc.Format;
			bc_format l_new_format;

			switch (l_format)
			{
			case DXGI_FORMAT_D16_UNORM:
				l_new_format = bc_format::R16_UNORM;
				break;
			case DXGI_FORMAT_D24_UNORM_S8_UINT:
				l_new_format = bc_format::R24_UNORM_X8_TYPELESS;
				break;
			case DXGI_FORMAT_D32_FLOAT:
				l_new_format = bc_format::R32_FLOAT;
				break;
			default:
				bcAssert(false);
				break;
			}

			(m_config.get_platform_pack().m_type == bc_resource_view_type::shader ?
				 m_config.get_platform_pack().m_shader_view_desc.Format :
				 m_config.get_platform_pack().m_unordered_shader_view_desc.Format) = bc_graphic_cast(l_new_format);

			return m_config;
		}

		bc_resource_view_config bc_texture_view_configure_modifier::on_render_target_texture2d() noexcept
		{
			return m_config;
		}

		bc_texture_view_configure::bc_texture_view_configure(bc_format p_format) noexcept
			: m_format(p_format)
		{
		}

		bc_texture_view_configure_modifier bc_texture_view_configure::as_tex2d_shader_view(bcUINT p_most_detailed_mip, bcUINT p_mip_levels) noexcept
		{
			bc_resource_view_config l_resource_view_config;
			D3D11_SHADER_RESOURCE_VIEW_DESC& l_srv_desc = l_resource_view_config.get_platform_pack().m_shader_view_desc;

			l_resource_view_config.get_platform_pack().m_type = bc_resource_view_type::shader;

			std::memset(&l_srv_desc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

			l_srv_desc.Format = bc_graphic_cast(m_format);
			l_srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			l_srv_desc.Texture2D.MostDetailedMip = p_most_detailed_mip;
			l_srv_desc.Texture2D.MipLevels = p_mip_levels;

			return bc_texture_view_configure_modifier(l_resource_view_config);
		}
		
		bc_texture_view_configure_modifier bc_texture_view_configure::as_tex2dms_shader_view() noexcept
		{
			bc_texture_view_configure_modifier l_resource_view_config = as_tex2d_shader_view(0, 0);
			l_resource_view_config.m_config.get_platform_pack().m_shader_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;

			return l_resource_view_config;
		}
				
		bc_texture_view_configure_modifier bc_texture_view_configure::as_tex2d_unordered_shader_view(bcUINT p_mip_slice) noexcept
		{
			bc_resource_view_config l_resource_view_config;
			D3D11_UNORDERED_ACCESS_VIEW_DESC& l_uav_desc = l_resource_view_config.get_platform_pack().m_unordered_shader_view_desc;

			l_resource_view_config.get_platform_pack().m_type = bc_resource_view_type::unordered;

			std::memset(&l_uav_desc, 0, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));

			l_uav_desc.Format = bc_graphic_cast(m_format);
			l_uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
			l_uav_desc.Texture2D.MipSlice = p_mip_slice;

			return bc_texture_view_configure_modifier(l_resource_view_config);
		}
				
		bc_render_target_view_config bc_texture_view_configure::as_tex2d_render_target_view(bcUINT p_mip_slice) noexcept
		{
			bc_render_target_view_config l_resource_view_config;
			D3D11_RENDER_TARGET_VIEW_DESC& l_srv_desc = l_resource_view_config.get_platform_pack().m_render_target_view_desc;

			std::memset(&l_srv_desc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

			l_srv_desc.Format = bc_graphic_cast(m_format);
			l_srv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			l_srv_desc.Texture2D.MipSlice = p_mip_slice;

			return l_resource_view_config;
		}
				
		bc_render_target_view_config bc_texture_view_configure::as_tex2dms_render_target_view() noexcept
		{
			bc_render_target_view_config l_resource_view_config = as_tex2d_render_target_view(0);
			l_resource_view_config.get_platform_pack().m_render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

			return l_resource_view_config;
		}
				
		bc_depth_stencil_view_config bc_texture_view_configure::as_tex2d_depth_stencil_view(bcUINT p_mip_slice) noexcept
		{
			bc_depth_stencil_view_config l_resource_view_config;
			D3D11_DEPTH_STENCIL_VIEW_DESC& l_dsv_desc = l_resource_view_config.get_platform_pack().m_depth_stencil_view_desc;

			std::memset(&l_dsv_desc, 0, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

			l_dsv_desc.Format = bc_graphic_cast(m_format);
			l_dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			l_dsv_desc.Texture2D.MipSlice = p_mip_slice;

			return l_resource_view_config;
		}
				
		bc_depth_stencil_view_config bc_texture_view_configure::as_tex2dms_depth_stencil_view() noexcept
		{
			bc_depth_stencil_view_config l_resource_view_config = as_tex2d_depth_stencil_view(0);
			l_resource_view_config.get_platform_pack().m_depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

			return l_resource_view_config;
		}
	}
}
