// [01/23/2016 MRB]

#include "GraphicImp/GraphicImpPCH.h"
#include "CorePlatform/CorePlatformPCH.h"
#include "Core/Container/bcAllocator.h"
#include "Core/Container/bcVector.h"
#include "Core/File/bcPath.h"
#include "Core/Utility/bcServiceManager.h"
#include "CorePlatformImp/bcUtility.h"
#include "GraphicImp/bcExport.h"
// Because explicit specialization of device pack must be included first and 
// some other header files has included bcDevice.h we must put this header at 
// top of other header files
#include "GraphicImp/Device/bcDevice.h"
#include "GraphicImp/bcUtility.h"
#include "GraphicImp/Resource/bcResource.h"
#include "GraphicImp/Resource/bcResourceBuilder.h"
#include "GraphicImp/Resource/Buffer/bcBuffer.h"
#include "GraphicImp/Resource/Texture/bcTexture2d.h"
#include "GraphicImp/Resource/View/bcResourceView.h"
#include "GraphicImp/Resource/View/bcRenderTargetView.h"
#include "GraphicImp/Resource/View/bcDepthStencilView.h"
#include "GraphicImp/Resource/State/bcSamplerState.h"
#include "GraphicImp/Shader/bcShader.h"
#include "GraphicImp/Shader/bcVertexShader.h"
#include "GraphicImp/Shader/bcHullShader.h"
#include "GraphicImp/Shader/bcDomainShader.h"
#include "GraphicImp/Shader/bcGeometryShader.h"
#include "GraphicImp/Shader/bcPixelShader.h"
#include "GraphicImp/Shader/bcComputeShader.h"
#include "GraphicImp/Device/bcDeviceOutput.h"
#include "GraphicImp/Device/bcDevicePipeline.h"
#include "GraphicImp/Device/bcDevicePipelineState.h"
#include "GraphicImp/Device/bcDeviceComputeState.h"
#include "GraphicImp/Device/Command/bcDeviceCommandExecutor.h"
#include "GraphicImp/Device/bcDeviceTextRenderer.h"

#include "3rdParty/DirectXTK-master/Include/DDSTextureLoader.h"
#include "3rdParty/DirectXTK-master/Include/WICTextureLoader.h"
#include "3rdParty/DirectXTK-master/Include/ScreenGrab.h"
#include <wincodec.h>
#include <locale>
#include <cwctype>

using namespace Microsoft::WRL;

namespace black_cat
{
	namespace graphic
	{
		ID3D11Buffer* _initialize_buffer(bc_device* p_device, bc_buffer_config* p_config, bc_subresource_data* p_data)
		{
			ID3D11Buffer* l_buffer;
			D3D11_BUFFER_DESC& l_buffer_desc = p_config->get_platform_pack().m_desc;
			D3D11_SUBRESOURCE_DATA l_data;

			if (p_data)
			{
				l_data.pSysMem = p_data->m_data;
				l_data.SysMemPitch = p_data->m_row_pitch;
				l_data.SysMemSlicePitch = p_data->m_depth_pitch;
			}

			dx_call(p_device->get_platform_pack().m_device->CreateBuffer(&l_buffer_desc, p_data ? &l_data : nullptr, &l_buffer));

			return l_buffer;
		}

		ID3D11Texture2D* _initialize_texture(bc_device* p_device, bc_texture_config* p_config, bc_subresource_data* p_data)
		{
			ID3D11Texture2D* l_texture;
			D3D11_TEXTURE2D_DESC& l_texture_desc = p_config->get_platform_pack().m_desc;
			D3D11_SUBRESOURCE_DATA l_data;

			bcAssert
			(
				l_texture_desc.SampleDesc.Count > 1 ?
				!(l_texture_desc.MiscFlags & D3D11_RESOURCE_MISC_GENERATE_MIPS) :
				true,
				"Multisampled texture can not have generated mip levels"
			);
			/*bcAssert(l_texture_desc.SampleDesc.Count == 1 ?
				(l_texture_desc.BindFlags & D3D11_BIND_SHADER_RESOURCE) || (l_texture_desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS) :
				true);*/

			if (p_data)
			{
				l_data.pSysMem = p_data->m_data;
				l_data.SysMemPitch = p_data->m_row_pitch;
				l_data.SysMemSlicePitch = p_data->m_depth_pitch;
			}

			dx_call(p_device->get_platform_pack().m_device->CreateTexture2D(&l_texture_desc, p_data ? &l_data : nullptr, &l_texture));

			return l_texture;
		}

		ID3D11Texture2D* _initialize_texture(bc_device* p_device, bc_texture_config* p_config, const bcBYTE* p_data, bcSIZE p_data_size, bc_image_format p_format)
		{
			ID3D11Resource* l_texture;
			D3D11_TEXTURE2D_DESC& l_texture_desc = p_config->get_platform_pack().m_desc;

			D3D11_USAGE l_usage = l_texture_desc.Usage;
			bcUINT l_bind_flags = l_texture_desc.BindFlags;
			bcUINT l_access_flags = l_texture_desc.CPUAccessFlags;
			bcUINT l_misc_flags = l_texture_desc.MiscFlags;

			if (p_format == bc_image_format::dds)
			{
				dx_call(DirectX::CreateDDSTextureFromMemoryEx
				(
					p_device->get_platform_pack().m_device.Get(),
					reinterpret_cast< const bcUBYTE* >(p_data),
					p_data_size,
					0,
					l_usage,
					l_bind_flags,
					l_access_flags,
					l_misc_flags,
					false,
					&l_texture,
					nullptr
				));
			}
			else // There are built-in WIC codecs in Windows for .BMP, .PNG, .GIF, .TIFF, .JPEG, and JPEG-XR / HD Photo images
			{
				CoInitialize(nullptr);
				dx_call(DirectX::CreateWICTextureFromMemoryEx
				(
					p_device->get_platform_pack().m_device.Get(),
					reinterpret_cast< const bcUBYTE* >(p_data),
					p_data_size,
					0,
					l_usage,
					l_bind_flags,
					l_access_flags,
					l_misc_flags,
					false,
					&l_texture,
					nullptr
				));
			}

			return static_cast< ID3D11Texture2D* >(l_texture);
		}

		void _save_texture(bc_device* p_device, bc_texture2d* p_texture, bc_image_format p_format, const bcECHAR* p_file_name)
		{
			{
				core_platform::bc_lock_guard<core_platform::bc_mutex> l_guard(p_device->get_platform_pack().m_context_mutex);

				if (p_format == bc_image_format::dds)
				{
					dx_call(DirectX::SaveDDSTextureToFile
					(
						p_device->get_platform_pack().m_immediate_context.Get(),
						p_texture->get_platform_pack().m_texture,
						p_file_name
					));
				}
				else
				{
					GUID l_format;

					switch (p_format)
					{
					case bc_image_format::bmp:
						l_format = GUID_ContainerFormatBmp;
						break;
					case bc_image_format::png:
						l_format = GUID_ContainerFormatPng;
						break;
					case bc_image_format::gif:
						l_format = GUID_ContainerFormatGif;
						break;
					case bc_image_format::tiff:
						l_format = GUID_ContainerFormatTiff;
						break;
					case bc_image_format::jpg:
						l_format = GUID_ContainerFormatJpeg;
						break;
					default:
						bcAssert(false);
						break;
					}

					dx_call(DirectX::SaveWICTextureToFile
					(
						p_device->get_platform_pack().m_immediate_context.Get(),
						p_texture->get_platform_pack().m_texture,
						l_format,
						p_file_name
					));
				}
			}
		}

		ID3D10Blob* _compile_shader(const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function_name, const bcCHAR* p_profile, const D3D_SHADER_MACRO* p_defines, const bcCHAR* p_source_file)
		{
			// Loop until we succeed, or an exception is thrown
			while (true)
			{
				bcUINT l_flags = 0;
#if defined( BC_DEBUG )
				l_flags |= D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION/*|D3D10_SHADER_WARNINGS_ARE_ERRORS*/;
#endif

				ID3D10Blob* l_compiled_shader;
				ComPtr< ID3D10Blob > l_error_messages;
				HRESULT l_hr = D3DCompile(p_data,
					p_data_size,
					p_source_file,
					p_defines,
					D3D_COMPILE_STANDARD_FILE_INCLUDE,
					p_function_name,
					p_profile,
					l_flags,
					0,
					&l_compiled_shader,
					l_error_messages.GetAddressOf());

				if (FAILED(l_hr))
				{
					DWORD l_error_code;
					core_platform::win32_from_hresult(l_hr, &l_error_code);
					core::bc_string l_full_message = "Error compiling shader file \"";

					if (l_error_messages)
					{
						bcCHAR l_message[3072];
						l_message[0] = NULL;
						bcCHAR* l_blobdata = reinterpret_cast< char* >(l_error_messages->GetBufferPointer());

						std::strcpy(l_message, l_blobdata);

						l_full_message += l_message;

						bcAssert(false);
						throw bc_graphic_exception(static_cast< bcINT >(l_error_code), l_full_message);
					}
					else
					{
						bcAssert(false);
						throw bc_graphic_exception(static_cast< bcINT >(l_error_code), l_full_message);
					}
				}

				return l_compiled_shader;
			}
		}

		std::pair<ID3D10Blob*, ID3D11VertexShader*> _initialize_vertex_shader(bc_device* p_device, const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function)
		{
			ID3D10Blob* l_compiled_shader;
			ID3D11VertexShader* l_shader;

			dx_call(p_device->get_platform_pack().m_device->CreateVertexShader
			(
				p_data,
				p_data_size,
				nullptr,
				&l_shader
			));

			dx_call(D3DCreateBlob(p_data_size, &l_compiled_shader));
			std::memcpy(l_compiled_shader->GetBufferPointer(), p_data, p_data_size);
			l_shader = l_shader;

			return std::make_pair(l_compiled_shader, l_shader);
		}

		ID3D11HullShader* _initialize_hull_shader(bc_device* p_device, const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function)
		{
			ID3D11HullShader* l_shader;

			dx_call(p_device->get_platform_pack().m_device->CreateHullShader
			(
				p_data,
				p_data_size,
				nullptr,
				&l_shader
			));

			return l_shader;
		}

		ID3D11DomainShader* _initialize_domain_shader(bc_device* p_device, const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function)
		{
			ID3D11DomainShader* l_shader;

			dx_call(p_device->get_platform_pack().m_device->CreateDomainShader
			(
				p_data,
				p_data_size,
				nullptr,
				&l_shader
			));

			return l_shader;
		}

		ID3D11GeometryShader* _initialize_geometry_shader(bc_device* p_device, const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function)
		{
			ID3D11GeometryShader* l_shader;

			dx_call(p_device->get_platform_pack().m_device->CreateGeometryShader
			(
				p_data,
				p_data_size,
				nullptr,
				&l_shader
			));

			return l_shader;
		}

		ID3D11PixelShader* _initialize_pixel_shader(bc_device* p_device, const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function)
		{
			ID3D11PixelShader* l_shader;

			dx_call(p_device->get_platform_pack().m_device->CreatePixelShader
			(
				p_data,
				p_data_size,
				nullptr,
				&l_shader
			));

			return l_shader;
		}

		ID3D11ComputeShader* _initialize_compute_shader(bc_device* p_device, const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function)
		{
			ID3D11ComputeShader* l_shader;

			dx_call(p_device->get_platform_pack().m_device->CreateComputeShader
			(
				p_data,
				p_data_size,
				nullptr,
				&l_shader
			));

			return l_shader;
		}

		ID3D11View* _initialize_shader_view(bc_device* p_device, bc_iresource* p_resource, bc_resource_view_config* p_view_config)
		{
			bcAssert
			(
				p_view_config->get_platform_pack().m_type == bc_resource_view_type::shader |
				p_view_config->get_platform_pack().m_type == bc_resource_view_type::unordered
			);

			ID3D11Resource* l_resource = p_resource->get_platform_pack().m_resource;
			ID3D11ShaderResourceView* l_shader_view;
			ID3D11UnorderedAccessView* l_unordered_view;

			if (p_view_config->get_platform_pack().m_type == bc_resource_view_type::shader)
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC& l_resource_view_desc = p_view_config->get_platform_pack().m_shader_view_desc;

				dx_call(p_device->get_platform_pack().m_device->CreateShaderResourceView
				(
					l_resource,
					&l_resource_view_desc,
					&l_shader_view
				));

				return l_shader_view;
			}
			else
			{
				D3D11_UNORDERED_ACCESS_VIEW_DESC& l_unordered_view_desc = p_view_config->get_platform_pack().m_unordered_shader_view_desc;

				dx_call(p_device->get_platform_pack().m_device->CreateUnorderedAccessView
				(
					l_resource,
					&l_unordered_view_desc,
					&l_unordered_view
				));

				return l_unordered_view;
			}
		}

		ID3D11DepthStencilView* _initialize_depth_stencil_view(bc_device* p_device, bc_iresource* p_resource, bc_depth_stencil_view_config* p_view_config)
		{
			ID3D11Resource* l_resource = p_resource->get_platform_pack().m_resource;
			ID3D11DepthStencilView* l_depth_stencil_view;

			D3D11_DEPTH_STENCIL_VIEW_DESC& l_depth_stencil_view_desc = p_view_config->get_platform_pack().m_depth_stencil_view_desc;

			dx_call(p_device->get_platform_pack().m_device->CreateDepthStencilView
			(
				l_resource,
				&l_depth_stencil_view_desc,
				&l_depth_stencil_view
			));

			return l_depth_stencil_view;
		}

		ID3D11RenderTargetView* _initialize_render_target_view(bc_device* p_device, bc_iresource* p_resource, bc_render_target_view_config* p_view_config)
		{
			ID3D11Resource* l_resource = p_resource->get_platform_pack().m_resource;
			ID3D11RenderTargetView* l_render_target_view;

			D3D11_RENDER_TARGET_VIEW_DESC& l_render_target_view_desc = p_view_config->get_platform_pack().m_render_target_view_desc;

			dx_call(p_device->get_platform_pack().m_device->CreateRenderTargetView
			(
				l_resource,
				&l_render_target_view_desc,
				&l_render_target_view
			));

			return l_render_target_view;
		}

		void _check_best_dxgi_fullscreen_setting(ID3D11Device* p_device, IDXGISwapChain* p_swap_chain, DXGI_MODE_DESC* p_new_desc, DXGI_MODE_DESC* p_best_desc)
		{
			ComPtr< IDXGIDevice > l_dxgi_device;
			ComPtr< IDXGIAdapter > l_adapter;
			ComPtr< IDXGIOutput > l_output;

			dx_call(p_swap_chain->GetDevice(__uuidof(IDXGIDevice), reinterpret_cast< void** >(l_dxgi_device.GetAddressOf())));
			l_dxgi_device->GetAdapter(l_adapter.GetAddressOf());
			l_adapter->EnumOutputs(0, l_output.GetAddressOf());

			dx_call(l_output->FindClosestMatchingMode(p_new_desc, p_best_desc, p_device));
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device< g_api_dx11 >::bc_platform_device()
		{
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device< g_api_dx11 >::bc_platform_device(bc_platform_device&& p_other) noexcept
		{
			operator=(std::move(p_other));
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device< g_api_dx11 >::~bc_platform_device()
		{
			if (m_initialized)
			{
				_destroy();
			}
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_platform_device< g_api_dx11 >& bc_platform_device< g_api_dx11 >::operator=(bc_platform_device&& p_other) noexcept
		{
			m_pack.m_vsync = p_other.m_pack.m_vsync;
			m_pack.m_device = std::move(p_other.m_pack.m_device);
			m_pack.m_immediate_context = std::move(p_other.m_pack.m_immediate_context);
			m_pack.m_swap_chain = std::move(p_other.m_pack.m_swap_chain);

			return *this;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_device< g_api_dx11 >::get_vsync() const
		{
			return m_pack.m_vsync;
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device< g_api_dx11 >::set_vsync(bool p_sync)
		{
			m_pack.m_vsync = p_sync;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bool bc_platform_device< g_api_dx11 >::get_full_screen() const
		{
			DXGI_SWAP_CHAIN_DESC l_desc;
			m_pack.m_swap_chain->GetDesc(&l_desc);

			return !l_desc.Windowed;
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device< g_api_dx11 >::set_full_screen(bool p_full_screen)
		{
			if (p_full_screen)
			{
				DXGI_SWAP_CHAIN_DESC l_swap_chain_desc;
				DXGI_MODE_DESC l_best_mode_desc;

				m_pack.m_swap_chain->GetDesc(&l_swap_chain_desc);

				_check_best_dxgi_fullscreen_setting
				(
					m_pack.m_device.Get(),
					m_pack.m_swap_chain.Get(),
					&l_swap_chain_desc.BufferDesc,
					&l_best_mode_desc
				);
				// Will send a WM_SIZE event so back buffer and all other buffers will be resizing by inter app events
				dx_call(m_pack.m_swap_chain->ResizeTarget(&l_best_mode_desc)); 
			}

			dx_call(m_pack.m_swap_chain->SetFullscreenState(p_full_screen, nullptr));
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_texture2d bc_platform_device< g_api_dx11 >::get_back_buffer_texture() const
		{
			ComPtr< ID3D11Texture2D > l_surface;
			dx_call(m_pack.m_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast< void** >(l_surface.GetAddressOf())));

			bc_texture2d::platform_pack l_pack;
			l_pack.m_texture = l_surface.Get();

			return bc_texture2d(l_pack);
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_device_info bc_platform_device< g_api_dx11 >::get_device_info() const
		{
			ComPtr< IDXGIDevice > l_dxgi_device;
			ComPtr< IDXGIAdapter > l_adapter;
			DXGI_ADAPTER_DESC l_adapter_desc;

			dx_call(m_pack.m_swap_chain->GetDevice(__uuidof(IDXGIDevice), reinterpret_cast< void** >(l_dxgi_device.GetAddressOf())));
			l_dxgi_device->GetAdapter(l_adapter.GetAddressOf());
			l_adapter->GetDesc(&l_adapter_desc);

			bc_device_info l_info;
			l_info.m_name = l_adapter_desc.Description;

			return l_info;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bcUINT bc_platform_device< g_api_dx11 >::check_multi_sampling(bc_format p_textue_format, bcUINT p_sample_count) const
		{
			bcUINT l_sample_quality;

			m_pack.m_device->CheckMultisampleQualityLevels(bc_graphic_cast(p_textue_format), p_sample_count, &l_sample_quality);

			return l_sample_quality;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_buffer_ptr bc_platform_device< g_api_dx11 >::create_buffer(bc_buffer_config& p_config, bc_subresource_data* p_data)
		{
			auto* l_dx_buffer = _initialize_buffer(static_cast< bc_device* >(this), &p_config, p_data);

			bc_buffer::platform_pack l_pack;
			l_pack.m_buffer = l_dx_buffer;

			bc_buffer l_buffer(l_pack);
			bc_buffer_ptr l_buffer_ptr(l_buffer);

			l_dx_buffer->Release();

			return l_buffer_ptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_texture2d_ptr bc_platform_device< g_api_dx11 >::create_texture2d(bc_texture_config& p_config, bc_subresource_data* p_data)
		{
			auto* l_dx_texture = _initialize_texture(static_cast< bc_device* >(this), &p_config, p_data);

			bc_texture2d::platform_pack l_pack;
			l_pack.m_texture = l_dx_texture;

			bc_texture2d l_texture2(l_pack);
			bc_texture2d_ptr l_texture_ptr(l_texture2);

			l_dx_texture->Release();

			return l_texture_ptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_texture2d_ptr bc_platform_device< g_api_dx11 >::create_texture2d(bc_texture_config& p_config, const bcBYTE* p_data, bcSIZE p_data_size, bc_image_format p_format)
		{
			auto* l_dx_texture = _initialize_texture(static_cast< bc_device* >(this), &p_config, p_data, p_data_size, p_format);
			
			bc_texture2d::platform_pack l_pack;
			l_pack.m_texture = l_dx_texture;

			bc_texture2d l_texture(l_pack);
			bc_texture2d_ptr l_texture_ptr(l_texture);

			l_dx_texture->Release();

			return l_texture_ptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device< g_api_dx11 >::save_texture2d(bc_texture2d p_texture, bc_image_format p_format, const bcECHAR* p_path)
		{
			_save_texture(static_cast<bc_device*>(this), &p_texture, p_format, p_path);
		}
		 
		template<>
		BC_GRAPHICIMP_DLL
		bc_sampler_state_ptr bc_platform_device< g_api_dx11 >::create_sampler_state(const bc_sampler_state_config& p_config)
		{
			ID3D11SamplerState* l_dx_sampler;
			D3D11_SAMPLER_DESC l_dx_sampler_desc;

			l_dx_sampler_desc.Filter = bc_graphic_cast(p_config.m_filter);
			l_dx_sampler_desc.AddressU = bc_graphic_cast(p_config.m_address_u);
			l_dx_sampler_desc.AddressV = bc_graphic_cast(p_config.m_address_v);
			l_dx_sampler_desc.AddressW = bc_graphic_cast(p_config.m_address_w);
			l_dx_sampler_desc.MipLODBias = p_config.m_mip_lod_bias;
			l_dx_sampler_desc.MaxAnisotropy = p_config.m_max_anisotropy;
			l_dx_sampler_desc.ComparisonFunc = bc_graphic_cast(p_config.m_comparison_func);
			l_dx_sampler_desc.BorderColor[0] = p_config.m_border_color[0];
			l_dx_sampler_desc.BorderColor[1] = p_config.m_border_color[1];
			l_dx_sampler_desc.BorderColor[2] = p_config.m_border_color[2];
			l_dx_sampler_desc.BorderColor[3] = p_config.m_border_color[3];
			l_dx_sampler_desc.MaxLOD = p_config.m_max_lod;
			l_dx_sampler_desc.MinLOD = p_config.m_min_lod;

			dx_call(m_pack.m_device->CreateSamplerState(&l_dx_sampler_desc, &l_dx_sampler));
			
			bc_sampler_state::platform_pack l_pack;
			l_pack.m_sampler_state = l_dx_sampler;

			bc_sampler_state l_sampler(l_pack);
			bc_sampler_state_ptr l_sampler_ptr(l_sampler);

			l_dx_sampler->Release();

			return l_sampler_ptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_compiled_shader_ptr bc_platform_device< g_api_dx11 >::compile_vertex_shader(const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function_name, const bcCHAR* p_source_file)
		{
			auto* l_dx_blob = _compile_shader
			(
				p_data,
				p_data_size,
				p_function_name,
				"vs_5_0",
				nullptr,
				p_source_file
			);

			bc_compiled_shader::platform_pack l_pack;
			l_pack.m_blob = l_dx_blob;

			bc_compiled_shader l_shader(l_pack);
			bc_compiled_shader_ptr l_shader_ptr(l_shader);

			l_dx_blob->Release();

			return l_shader_ptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_vertex_shader_ptr bc_platform_device< g_api_dx11 >::create_vertex_shader(const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function)
		{
			auto l_dx_shader = _initialize_vertex_shader(static_cast< bc_device* >(this), p_data, p_data_size, p_function);

			bc_vertex_shader::platform_pack l_pack;
			l_pack.m_compiled_shader = l_dx_shader.first;
			l_pack.m_shader = l_dx_shader.second;

			bc_vertex_shader l_sahder(l_pack);
			bc_vertex_shader_ptr l_sahder_ptr(l_sahder);

			l_dx_shader.first->Release();
			l_dx_shader.second->Release();

			return l_sahder_ptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_compiled_shader_ptr bc_platform_device< g_api_dx11 >::compile_hull_shader(const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function_name, const bcCHAR* p_source_file)
		{
			auto* l_dx_shader = _compile_shader
			(
				p_data,
				p_data_size,
				p_function_name,
				"hs_5_0",
				nullptr,
				p_source_file
			);

			bc_compiled_shader::platform_pack l_pack;
			l_pack.m_blob = l_dx_shader;

			bc_compiled_shader l_shader(l_pack);
			bc_compiled_shader_ptr l_shader_ptr(l_shader);

			l_dx_shader->Release();

			return l_shader_ptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_hull_shader_ptr bc_platform_device< g_api_dx11 >::create_hull_shader(const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function)
		{
			auto* l_dx_shader = _initialize_hull_shader(static_cast< bc_device* >(this), p_data, p_data_size, p_function);

			bc_hull_shader::platform_pack l_pack;
			l_pack.m_shader = l_dx_shader;

			bc_hull_shader l_shader(l_pack);
			bc_hull_shader_ptr l_shader_ptr(l_shader);

			l_dx_shader->Release();

			return l_shader_ptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_compiled_shader_ptr bc_platform_device< g_api_dx11 >::compile_domain_shader(const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function_name, const bcCHAR* p_source_file)
		{
			auto* l_dx_shader = _compile_shader
			(
				p_data,
				p_data_size,
				p_function_name,
				"ds_5_0",
				nullptr,
				p_source_file
			);

			bc_compiled_shader::platform_pack l_pack;
			l_pack.m_blob = l_dx_shader;

			bc_compiled_shader l_shader(l_pack);
			bc_compiled_shader_ptr l_shader_ptr(l_shader);

			l_dx_shader->Release();

			return l_shader_ptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_domain_shader_ptr bc_platform_device< g_api_dx11 >::create_domain_shader(const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function)
		{
			auto* l_dx_shader = _initialize_domain_shader(static_cast< bc_device* >(this), p_data, p_data_size, p_function);

			bc_domain_shader::platform_pack l_pack;
			l_pack.m_shader = l_dx_shader;

			bc_domain_shader l_shader(l_pack);
			bc_domain_shader_ptr l_shader_ptr(l_shader);

			l_dx_shader->Release();

			return l_shader_ptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_compiled_shader_ptr bc_platform_device< g_api_dx11 >::compile_geometry_shader(const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function_name, const bcCHAR* p_source_file)
		{
			auto* l_dx_shader = _compile_shader
			(
				p_data,
				p_data_size,
				p_function_name,
				"gs_5_0",
				nullptr,
				p_source_file
			);

			bc_compiled_shader::platform_pack l_pack;
			l_pack.m_blob = l_dx_shader;

			bc_compiled_shader l_shader(l_pack);
			bc_compiled_shader_ptr l_shader_ptr(l_shader);

			l_dx_shader->Release();

			return l_shader_ptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_geometry_shader_ptr bc_platform_device< g_api_dx11 >::create_geometry_shader(const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function)
		{
			auto* l_dx_shader = _initialize_geometry_shader(static_cast< bc_device* >(this), p_data, p_data_size, p_function);

			bc_geometry_shader::platform_pack l_pack;
			l_pack.m_shader = l_dx_shader;

			bc_geometry_shader l_shader(l_pack);
			bc_geometry_shader_ptr l_shader_ptr(l_shader);

			l_dx_shader->Release();

			return l_shader_ptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_compiled_shader_ptr bc_platform_device< g_api_dx11 >::compile_pixel_shader(const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function_name, const bcCHAR* p_source_file)
		{
			auto* l_dx_shader = _compile_shader
			(
				p_data,
				p_data_size,
				p_function_name,
				"ps_5_0",
				nullptr,
				p_source_file
			);

			bc_compiled_shader::platform_pack l_pack;
			l_pack.m_blob = l_dx_shader;

			bc_compiled_shader l_shader(l_pack);
			bc_compiled_shader_ptr l_shader_ptr(l_shader);

			l_dx_shader->Release();

			return l_shader_ptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_pixel_shader_ptr bc_platform_device< g_api_dx11 >::create_pixel_shader(const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function)
		{
			auto* l_dx_shader = _initialize_pixel_shader(static_cast< bc_device* >(this), p_data, p_data_size, p_function);

			bc_pixel_shader::platform_pack l_pack;
			l_pack.m_shader = l_dx_shader;

			bc_pixel_shader l_shader(l_pack);
			bc_pixel_shader_ptr l_shader_ptr(l_shader);

			l_dx_shader->Release();

			return l_shader_ptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_compiled_shader_ptr bc_platform_device< g_api_dx11 >::compile_compute_shader(const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function_name, const bcCHAR* p_source_file)
		{
			auto* l_dx_shader = _compile_shader
			(
				p_data,
				p_data_size,
				p_function_name,
				"cs_5_0",
				nullptr,
				p_source_file
			);

			bc_compiled_shader::platform_pack l_pack;
			l_pack.m_blob = l_dx_shader;

			bc_compiled_shader l_shader(l_pack);
			bc_compiled_shader_ptr l_shader_ptr(l_shader);

			l_dx_shader->Release();

			return l_shader_ptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_compute_shader_ptr bc_platform_device< g_api_dx11 >::create_compute_shader(const bcBYTE* p_data, bcSIZE p_data_size, const bcCHAR* p_function)
		{
			auto* l_dx_shader = _initialize_compute_shader(static_cast< bc_device* >(this), p_data, p_data_size, p_function);

			bc_compute_shader::platform_pack l_pack;
			l_pack.m_shader = l_dx_shader;

			bc_compute_shader l_shader(l_pack);
			bc_compute_shader_ptr l_shader_ptr(l_shader);

			l_dx_shader->Release();

			return l_shader_ptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_resource_view_ptr bc_platform_device< g_api_dx11 >::create_resource_view(bc_iresource& p_resource, bc_resource_view_config& p_view_config)
		{
			auto* l_dx_view = _initialize_shader_view(static_cast< bc_device* >(this), &p_resource, &p_view_config);

			bc_resource_view::platform_pack l_pack;
			bc_resource_view l_view;

			if(p_view_config.get_platform_pack().m_type == bc_resource_view_type::shader)
			{
				l_pack.m_shader_view = static_cast< ID3D11ShaderResourceView* >(l_dx_view);
				l_pack.m_unordered_shader_view = nullptr;

				l_view = bc_resource_view(l_pack, bc_resource_view_type::shader);
			}
			else
			{
				l_pack.m_shader_view = nullptr;
				l_pack.m_unordered_shader_view = static_cast< ID3D11UnorderedAccessView* >(l_dx_view);

				l_view = bc_resource_view(l_pack, bc_resource_view_type::unordered);
			}

			bc_resource_view_ptr l_view_ptr(l_view);

			l_dx_view->Release();

			return l_view_ptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_depth_stencil_view_ptr bc_platform_device< g_api_dx11 >::create_depth_stencil_view(bc_iresource& p_resource, bc_depth_stencil_view_config& p_view_config)
		{
			auto* l_dx_view = _initialize_depth_stencil_view(static_cast< bc_device* >(this), &p_resource, &p_view_config);

			bc_depth_stencil_view::platform_pack l_pack;
			l_pack.m_depth_stencil_view = l_dx_view;

			bc_depth_stencil_view l_depth_view(l_pack);
			bc_depth_stencil_view_ptr l_depth_view_ptr(l_depth_view);

			l_dx_view->Release();

			return l_depth_view_ptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_render_target_view_ptr bc_platform_device< g_api_dx11 >::create_render_target_view(bc_iresource& p_resource, bc_render_target_view_config& p_view_config)
		{
			auto* l_dx_view = _initialize_render_target_view(static_cast< bc_device* >(this), &p_resource, &p_view_config);

			bc_render_target_view::platform_pack l_pack;
			l_pack.m_render_target_view = l_dx_view;

			bc_render_target_view l_render_view(l_pack);
			bc_render_target_view_ptr l_render_view_ptr(l_render_view);

			l_dx_view->Release();

			return l_render_view_ptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_device_pipeline_state_ptr bc_platform_device< g_api_dx11 >::create_pipeline_state(bc_device_pipeline_state_config& p_config)
		{
			ID3D11BlendState* l_dx_blend_state = nullptr;
			ID3D11DepthStencilState* l_dx_depth_stencil = nullptr;
			ID3D11RasterizerState* l_dx_rasterizer_state = nullptr;
			ID3D11InputLayout* l_dx_input_layout = nullptr;
			D3D11_BLEND_DESC l_dx_blend_desc;
			D3D11_DEPTH_STENCIL_DESC l_dx_depth_stencil_desc;
			D3D11_RASTERIZER_DESC l_dx_rasterizer_desc;
			core::bc_vector_frame< D3D11_INPUT_ELEMENT_DESC > l_dx_input_elements;

			l_dx_input_elements.reserve(p_config.m_input_layout_config.m_input_elements.size());

			l_dx_blend_desc.AlphaToCoverageEnable = p_config.m_blend_state_config.m_alpha_to_coverage_enable;
			l_dx_blend_desc.IndependentBlendEnable = p_config.m_blend_state_config.m_independent_blend_enable;
			for (bcUINT i = 0; i < bc_render_api_info::number_of_om_render_target_slots(); ++i)
			{
				l_dx_blend_desc.RenderTarget[i].BlendEnable = p_config.m_blend_state_config.m_render_target[i].m_blend_enable;
				l_dx_blend_desc.RenderTarget[i].SrcBlend = bc_graphic_cast(p_config.m_blend_state_config.m_render_target[i].m_src_blend);
				l_dx_blend_desc.RenderTarget[i].DestBlend = bc_graphic_cast(p_config.m_blend_state_config.m_render_target[i].m_dest_blend);
				l_dx_blend_desc.RenderTarget[i].BlendOp = bc_graphic_cast(p_config.m_blend_state_config.m_render_target[i].m_blend_op);
				l_dx_blend_desc.RenderTarget[i].SrcBlendAlpha = bc_graphic_cast(p_config.m_blend_state_config.m_render_target[i].m_src_blend_alpha);
				l_dx_blend_desc.RenderTarget[i].DestBlendAlpha = bc_graphic_cast(p_config.m_blend_state_config.m_render_target[i].m_dest_blend_alpha);
				l_dx_blend_desc.RenderTarget[i].BlendOpAlpha = bc_graphic_cast(p_config.m_blend_state_config.m_render_target[i].m_blend_op_alpha);
				l_dx_blend_desc.RenderTarget[i].RenderTargetWriteMask = p_config.m_blend_state_config.m_render_target[0].m_render_target_write_mask;
			}

			l_dx_depth_stencil_desc.DepthEnable = p_config.m_depth_stencil_state_config.m_depth_enable;
			l_dx_depth_stencil_desc.DepthWriteMask = bc_graphic_cast(p_config.m_depth_stencil_state_config.m_depth_write_mask);
			l_dx_depth_stencil_desc.DepthFunc = bc_graphic_cast(p_config.m_depth_stencil_state_config.m_depth_func);
			l_dx_depth_stencil_desc.StencilEnable = p_config.m_depth_stencil_state_config.m_stencil_enable;
			l_dx_depth_stencil_desc.StencilReadMask = p_config.m_depth_stencil_state_config.m_stencil_read_mask;
			l_dx_depth_stencil_desc.StencilWriteMask = p_config.m_depth_stencil_state_config.m_stencil_write_mask;
			l_dx_depth_stencil_desc.FrontFace.StencilFailOp = bc_graphic_cast(p_config.m_depth_stencil_state_config.m_front_face.m_stencil_fail_op);
			l_dx_depth_stencil_desc.FrontFace.StencilDepthFailOp = bc_graphic_cast(p_config.m_depth_stencil_state_config.m_front_face.m_stencil_depth_fail_op);
			l_dx_depth_stencil_desc.FrontFace.StencilPassOp = bc_graphic_cast(p_config.m_depth_stencil_state_config.m_front_face.m_stencil_pass_op);
			l_dx_depth_stencil_desc.FrontFace.StencilFunc = bc_graphic_cast(p_config.m_depth_stencil_state_config.m_front_face.m_stencil_func);
			l_dx_depth_stencil_desc.BackFace.StencilFailOp = bc_graphic_cast(p_config.m_depth_stencil_state_config.m_back_face.m_stencil_fail_op);
			l_dx_depth_stencil_desc.BackFace.StencilDepthFailOp = bc_graphic_cast(p_config.m_depth_stencil_state_config.m_back_face.m_stencil_depth_fail_op);
			l_dx_depth_stencil_desc.BackFace.StencilPassOp = bc_graphic_cast(p_config.m_depth_stencil_state_config.m_back_face.m_stencil_pass_op);
			l_dx_depth_stencil_desc.BackFace.StencilFunc = bc_graphic_cast(p_config.m_depth_stencil_state_config.m_back_face.m_stencil_func);

			l_dx_rasterizer_desc.FillMode = bc_graphic_cast(p_config.m_rasterizer_state_config.m_fill_mode);
			l_dx_rasterizer_desc.CullMode = bc_graphic_cast(p_config.m_rasterizer_state_config.m_cull_mode);
			l_dx_rasterizer_desc.FrontCounterClockwise = p_config.m_rasterizer_state_config.m_front_counter_clockwise;
			l_dx_rasterizer_desc.DepthBias = p_config.m_rasterizer_state_config.m_depth_bias;
			l_dx_rasterizer_desc.DepthBiasClamp = p_config.m_rasterizer_state_config.m_depth_bias_clamp;
			l_dx_rasterizer_desc.SlopeScaledDepthBias = p_config.m_rasterizer_state_config.m_slope_scaled_depth_bias;
			l_dx_rasterizer_desc.DepthClipEnable = p_config.m_rasterizer_state_config.m_depth_clip_enable;
			l_dx_rasterizer_desc.ScissorEnable = p_config.m_rasterizer_state_config.m_scissor_enable;
			l_dx_rasterizer_desc.MultisampleEnable = p_config.m_rasterizer_state_config.m_multisample_enable;
			l_dx_rasterizer_desc.AntialiasedLineEnable = p_config.m_rasterizer_state_config.m_antialiased_line_enable;

			for (auto& l_input_element : p_config.m_input_layout_config.m_input_elements)
			{
				D3D11_INPUT_ELEMENT_DESC l_element_desc;
				l_element_desc.SemanticName = l_input_element.m_semantic_name;
				l_element_desc.SemanticIndex = l_input_element.m_semantic_index;
				l_element_desc.Format = bc_graphic_cast(l_input_element.m_format);
				l_element_desc.InputSlot = l_input_element.m_input_slot;
				l_element_desc.AlignedByteOffset = l_input_element.m_aligned_byte_offset;
				l_element_desc.InputSlotClass = bc_graphic_cast(l_input_element.m_input_slot_class);
				l_element_desc.InstanceDataStepRate = l_input_element.m_instance_data_step_rate;

				l_dx_input_elements.push_back(l_element_desc);
			}

			dx_call(m_pack.m_device->CreateBlendState(&l_dx_blend_desc, &l_dx_blend_state));
			dx_call(m_pack.m_device->CreateDepthStencilState(&l_dx_depth_stencil_desc, &l_dx_depth_stencil));
			dx_call(m_pack.m_device->CreateRasterizerState(&l_dx_rasterizer_desc, &l_dx_rasterizer_state));
			if (p_config.m_vertex_shader != nullptr && !l_dx_input_elements.empty())
			{
				dx_call(m_pack.m_device->CreateInputLayout
				(
					l_dx_input_elements.data(),
					l_dx_input_elements.size(),
					p_config.m_vertex_shader.get_platform_pack().m_compiled_shader->GetBufferPointer(),
					p_config.m_vertex_shader.get_platform_pack().m_compiled_shader->GetBufferSize(),
					&l_dx_input_layout
				));
			}

			//auto l_pipeline_state_poxy = allocate< bc_device_pipeline_state_proxy >();
			auto l_pipeline_state_poxy = bcNew(bc_device_pipeline_state_proxy, core::bc_alloc_type::unknown);

			l_pipeline_state_poxy->m_config = std::move(p_config);
			l_pipeline_state_poxy->m_blend_state = l_dx_blend_state;
			l_pipeline_state_poxy->m_depth_stencil_state = l_dx_depth_stencil;
			l_pipeline_state_poxy->m_rasterizer_state = l_dx_rasterizer_state;
			l_pipeline_state_poxy->m_input_layout = l_dx_input_layout;

			bc_device_pipeline_state::platform_pack l_pack;
			l_pack.m_pipeline_state_proxy = l_pipeline_state_poxy;

			bc_device_pipeline_state l_pipeline_state(l_pack);
			bc_device_pipeline_state_ptr l_pipeline_state_ptr(l_pipeline_state);

			l_pipeline_state_poxy->m_blend_state->Release();
			l_pipeline_state_poxy->m_depth_stencil_state->Release();
			l_pipeline_state_poxy->m_rasterizer_state->Release();
			if(l_pipeline_state_poxy->m_input_layout)
			{
				l_pipeline_state_poxy->m_input_layout->Release();
			}

			return l_pipeline_state_ptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_device_compute_state_ptr bc_platform_device< g_api_dx11 >::create_compute_state(bc_device_compute_state_config& p_config)
		{
			/*auto l_compute_state_proxy = allocate< bc_device_compute_state_proxy >();*/
			auto l_compute_state_proxy = bcNew(bc_device_compute_state_proxy, core::bc_alloc_type::unknown);
			
			l_compute_state_proxy->m_config = std::move(p_config);

			bc_device_compute_state::platform_pack l_pack;
			l_pack.m_compute_state_proxy = l_compute_state_proxy;

			bc_device_compute_state l_compute_state(l_pack);
			bc_device_compute_state_ptr l_compute_state_ptr(l_compute_state);

			return l_compute_state_ptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_device_pipeline_ptr bc_platform_device< g_api_dx11 >::create_pipeline()
		{
			/*auto l_pipeline_proxy = allocate< bc_device_pipeline_proxy >();*/
			auto l_pipeline_proxy = bcNew(bc_device_pipeline_proxy, core::bc_alloc_type::unknown);

			ID3D11Query* l_query;
			ID3D11DeviceContext* l_context;

			D3D11_QUERY_DESC l_query_desc;
			l_query_desc.Query = D3D11_QUERY_PIPELINE_STATISTICS;
			l_query_desc.MiscFlags = 0;
			dx_call(m_pack.m_device->CreateQuery(&l_query_desc, &l_query));
			dx_call(m_pack.m_device->CreateDeferredContext(0, &l_context));

			bc_device_pipeline::platform_pack l_pack;
			l_pack.m_pipeline_proxy = l_pipeline_proxy;
			l_pack.m_pipeline_proxy->m_device = this;
			l_pack.m_pipeline_proxy->m_query = l_query;
			l_pack.m_pipeline_proxy->m_context = l_context;

			bc_device_pipeline l_pipeline(l_pack);
			bc_device_pipeline_ptr l_pipeline_ptr(l_pipeline);

			l_pack.m_pipeline_proxy->m_context->Release();
			l_pack.m_pipeline_proxy->m_query->Release();

			return l_pipeline_ptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_device_command_list_ptr bc_platform_device< g_api_dx11 >::create_command_list()
		{
			auto l_command_list_proxy = bcNew(bc_device_command_list_proxy, core::bc_alloc_type::unknown);

			bc_device_command_list::platform_pack l_pack;
			l_pack.m_command_list_proxy = l_command_list_proxy;
			l_pack.m_command_list_proxy->m_command_list = nullptr;

			bc_device_command_list l_command_list(l_pack);
			bc_device_command_list_ptr l_command_list_ptr(l_command_list);

			return l_command_list_ptr;
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_device_command_executor_ptr bc_platform_device< g_api_dx11 >::create_command_executor()
		{
			bc_device_command_executor::platform_pack l_pack;
			l_pack.m_device = this;
			bc_device_command_executor l_pointer(l_pack);

			return bc_device_command_executor_ptr(l_pointer);
		}

		template< >
		BC_GRAPHICIMP_DLL
		bc_device_text_renderer bc_platform_device< g_api_dx11 >::create_text_renderer()
		{
			// TODO use parameter
			auto l_sprite_path = core::bc_path::get_absolute_path(L"Content\\Data\\DX11.spritefont");
			
			bc_device_text_renderer::platform_pack l_pack;
			l_pack.m_sprite_font = core::bc_make_unique<DirectX::SpriteFont>(m_pack.m_device.Get(), l_sprite_path.c_str());
			l_pack.m_sprite_batch = core::bc_make_unique<DirectX::SpriteBatch>(m_pack.m_immediate_context.Get());

			return bc_device_text_renderer(std::move(l_pack));
		}

		template<>
		BC_GRAPHICIMP_DLL
		bc_mapped_resource bc_platform_device< g_api_dx11 >::map_resource(bc_iresource& p_resource, bcUINT p_subresource, bc_resource_map p_map_type)
		{
			bc_mapped_resource l_result;
			D3D11_MAPPED_SUBRESOURCE l_mapped_resource;

			{
				core_platform::bc_lock_guard<core_platform::bc_mutex> l_guard(m_pack.m_context_mutex);

				dx_call(m_pack.m_immediate_context->Map
				(
					p_resource.get_platform_pack().m_resource,
					p_subresource,
					bc_graphic_cast(p_map_type),
					0,
					&l_mapped_resource
				));
			}

			l_result.m_data = l_mapped_resource.pData;
			l_result.m_row_pitch = l_mapped_resource.RowPitch;
			l_result.m_depth_pitch = l_mapped_resource.DepthPitch;

			return l_result;
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device< g_api_dx11 >::unmap_resource(bc_iresource& p_resource, bcUINT p_subresource)
		{
			{
				core_platform::bc_lock_guard<core_platform::bc_mutex> l_guard(m_pack.m_context_mutex);

				m_pack.m_immediate_context->Unmap(p_resource.get_platform_pack().m_resource, p_subresource);
			}
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device< g_api_dx11 >::resize_back_buffer(bcUINT p_width, bcUINT p_height)
		{
			resize_back_buffer(p_width, p_height, get_back_buffer_format());
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device< g_api_dx11 >::resize_back_buffer(bcUINT p_width, bcUINT p_height, bc_format p_format)
		{
			bc_texture2d l_back_buffer = get_back_buffer_texture();

			// If we are in fullscreen state change resolution too
			if (get_full_screen()) // TODO check for 
			{
				DXGI_SWAP_CHAIN_DESC l_new_mode_desc;
				DXGI_MODE_DESC l_best_mode_desc;

				m_pack.m_swap_chain->GetDesc(&l_new_mode_desc);
				l_new_mode_desc.BufferDesc.Width = p_width;
				l_new_mode_desc.BufferDesc.Height = p_height;
				l_new_mode_desc.BufferDesc.Format = bc_graphic_cast(p_format);

				_check_best_dxgi_fullscreen_setting
				(
					m_pack.m_device.Get(),
					m_pack.m_swap_chain.Get(),
					&l_new_mode_desc.BufferDesc,
					&l_best_mode_desc
				);
				dx_call(m_pack.m_swap_chain->ResizeTarget(&l_best_mode_desc));
			}

			if (l_back_buffer.get_width() != p_width ||
				l_back_buffer.get_height() != p_height)
			{
				dx_call(m_pack.m_swap_chain->ResizeBuffers
				(
					2,
					p_width,
					p_height,
					bc_graphic_cast(p_format),
					DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
				));
			}
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device< g_api_dx11 >::resize_texture2d(bc_texture2d& p_texture,
			bcUINT p_width,
			bcUINT p_height,
			bc_iresource_view** p_views,
			bcUINT p_num_views)
		{
			bc_texture_config l_texture_config;
			p_texture.get_platform_pack().m_texture->GetDesc(&l_texture_config.get_platform_pack().m_desc);

			l_texture_config.get_platform_pack().m_desc.Width = p_width;
			l_texture_config.get_platform_pack().m_desc.Height = p_height;

			auto* l_dx_texture = _initialize_texture
			(
				this,
				&l_texture_config,
				nullptr
			);

			p_texture.get_platform_pack().m_texture = l_dx_texture;

			for (bcUINT l_view_i = 0; l_view_i < p_num_views; ++l_view_i)
			{
				bc_iresource_view* l_view = p_views[l_view_i];

				if (l_view->get_view_type() == bc_resource_view_type::shader)
				{
					bc_resource_view* l_shader_view = static_cast< bc_resource_view* >(l_view);

					bc_resource_view_config l_view_config;
					l_shader_view->get_platform_pack().m_shader_view->GetDesc(&l_view_config.get_platform_pack().m_shader_view_desc);

					auto* l_dx_view = _initialize_shader_view
					(
						this,
						&p_texture,
						&l_view_config
					);

					l_shader_view->get_platform_pack().m_shader_view = static_cast< ID3D11ShaderResourceView* >(l_dx_view);
				}
				else if (l_view->get_view_type() == bc_resource_view_type::unordered)
				{
					bc_resource_view* l_shader_view = static_cast< bc_resource_view* >(l_view);

					bc_resource_view_config l_view_config;
					l_shader_view->get_platform_pack().m_unordered_shader_view->GetDesc(&l_view_config.get_platform_pack().m_unordered_shader_view_desc);

					auto* l_dx_view = _initialize_shader_view
					(
						this,
						&p_texture,
						&l_view_config
					);

					l_shader_view->get_platform_pack().m_unordered_shader_view = static_cast< ID3D11UnorderedAccessView* >(l_dx_view);
				}
				else if (l_view->get_view_type() == bc_resource_view_type::render_target)
				{
					bc_render_target_view* l_target_view = static_cast< bc_render_target_view* >(l_view);

					bc_render_target_view_config l_render_view_config;
					l_target_view->get_platform_pack().m_render_target_view->GetDesc(&l_render_view_config.get_platform_pack().m_render_target_view_desc);

					auto* l_dx_view = _initialize_render_target_view
					(
						static_cast< bc_device* >(this),
						&p_texture,
						&l_render_view_config
					);

					l_target_view->get_platform_pack().m_render_target_view = l_dx_view;
				}
				else if (l_view->get_view_type() == bc_resource_view_type::depth_stencil)
				{
					bc_depth_stencil_view* l_depth_view = static_cast< bc_depth_stencil_view* >(l_view);

					bc_depth_stencil_view_config l_depth_view_config;
					l_depth_view->get_platform_pack().m_depth_stencil_view->GetDesc(&l_depth_view_config.get_platform_pack().m_depth_stencil_view_desc);

					auto* l_dx_view = _initialize_depth_stencil_view
					(
						this,
						&p_texture,
						&l_depth_view_config
					);

					l_depth_view->get_platform_pack().m_depth_stencil_view = l_dx_view;
				}
			}
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device< g_api_dx11 >::present()
		{
			m_pack.m_swap_chain->Present(m_pack.m_vsync ? 1 : 0, 0);
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device< g_api_dx11 >::_initialize(bcUINT p_width, bcUINT p_height, bc_format p_back_buffer_format, bc_platform_device_output< g_api_dx11 > p_output)
		{
			HRESULT l_result = S_OK;

			ComPtr< IDXGIFactory > l_factory;
			CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast< void** >(l_factory.GetAddressOf()));

			bcUINT l_adapter_num = 0;
			ComPtr< IDXGIAdapter > l_current_adapter;
			core::bc_vector_frame< ComPtr< IDXGIAdapter > > l_adapters;

			core::bc_wstring_frame l_nvidia(L"nvidia");
			core::bc_wstring_frame l_amd(L"amd");
			while (l_factory->EnumAdapters(l_adapter_num++, l_current_adapter.ReleaseAndGetAddressOf()) != DXGI_ERROR_NOT_FOUND)
			{
				DXGI_ADAPTER_DESC l_adapter_desc;
				l_current_adapter->GetDesc(&l_adapter_desc);

				core::bc_wstring_frame l_adapter_title = l_adapter_desc.Description;
				
				auto l_nvidia_pos = std::search(std::begin(l_adapter_title), std::end(l_adapter_title), std::begin(l_nvidia), std::end(l_nvidia), [](bcWCHAR p_1, bcWCHAR p_2)
				{
					return std::towlower(p_1) == std::towlower(p_2);
				});
				if(l_nvidia_pos != std::end(l_adapter_title))
				{
					l_adapters.insert(std::begin(l_adapters), l_current_adapter);
					continue;
				}

				auto l_amd_pos = std::search(std::begin(l_adapter_title), std::end(l_adapter_title), std::begin(l_amd), std::end(l_amd), [](bcWCHAR p_1, bcWCHAR p_2)
				{
					return std::towlower(p_1) == std::towlower(p_2);
				});
				if (l_amd_pos != std::end(l_adapter_title))
				{
					l_adapters.insert(std::begin(l_adapters), l_current_adapter);
					continue;
				}

				l_adapters.push_back(l_current_adapter);
			}

			bcUINT l_device_flags = 0;
			// Because we create device with specific adapter we must use D3D_DRIVER_TYPE_UNKNOWN
			D3D_DRIVER_TYPE l_device_driver_type = D3D_DRIVER_TYPE_UNKNOWN;
			D3D_FEATURE_LEVEL l_device_feature_levels[] = { D3D_FEATURE_LEVEL_11_0 };
			D3D_FEATURE_LEVEL l_device_created_feature;

#ifdef  BC_DEBUG
			l_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

			for (auto& l_adapter : l_adapters)
			{
				l_result = D3D11CreateDevice
				(
					l_adapter.Get(),
					l_device_driver_type,
					nullptr,
					l_device_flags,
					l_device_feature_levels,
					1,
					D3D11_SDK_VERSION,
					m_pack.m_device.GetAddressOf(),
					&l_device_created_feature,
					m_pack.m_immediate_context.GetAddressOf()
				);

				if (SUCCEEDED(l_result))
				{
					break;
				}
			}

			if (FAILED(l_result))
			{
				DWORD l_error_code;
				core_platform::win32_from_hresult(l_result, &l_error_code);
				throw bc_graphic_exception(static_cast< bcINT >(l_error_code), "Can not found an adapter compatible with DirectX11");
			}

			DXGI_SWAP_CHAIN_DESC l_swap_chain_desc;
			std::memset(&l_swap_chain_desc, 0, sizeof(DXGI_SWAP_CHAIN_DESC));

			l_swap_chain_desc.BufferCount = 2;
			l_swap_chain_desc.BufferDesc.Format = bc_graphic_cast(p_back_buffer_format);
			l_swap_chain_desc.BufferDesc.Width = p_width;
			l_swap_chain_desc.BufferDesc.Height = p_height;
			l_swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			l_swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			l_swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
			l_swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
			l_swap_chain_desc.SampleDesc.Count = 1;
			l_swap_chain_desc.SampleDesc.Quality = 0;
			l_swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			l_swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			l_swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			l_swap_chain_desc.OutputWindow = p_output.get_platform_pack().m_output_handle;
			l_swap_chain_desc.Windowed = true;

			l_result = l_factory->CreateSwapChain(m_pack.m_device.Get(), &l_swap_chain_desc, m_pack.m_swap_chain.GetAddressOf());

			if (FAILED(l_result))
			{
				DWORD l_error_code;
				core_platform::win32_from_hresult(l_result, &l_error_code);
				throw bc_graphic_exception(static_cast< bcINT >(l_error_code), "Failed to create DirectX11 SwapChain");
			}

			set_vsync(false);
			set_allocator_alloc_type(core::bc_alloc_type::unknown_movable);
		}

		template<>
		BC_GRAPHICIMP_DLL
		void bc_platform_device< g_api_dx11 >::_destroy()
		{
			m_pack.m_swap_chain.ReleaseAndGetAddressOf();
			m_pack.m_immediate_context.ReleaseAndGetAddressOf();

#ifdef BC_DEBUG
			ComPtr< ID3D11Debug > l_debug_layer;
			m_pack.m_device.Get()->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast< void** >(l_debug_layer.GetAddressOf()));
			l_debug_layer->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
			l_debug_layer.ReleaseAndGetAddressOf();
#endif

			m_pack.m_device.ReleaseAndGetAddressOf();
		}
	}
}